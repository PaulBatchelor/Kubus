//-----------------------------------------------------------------------------
// name: VisualSine.cpp
// desc: hello sine wave, real-time
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//   date: fall 2014
//   uses: RtAudio by Gary Scavone
//-----------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#ifdef __MACOSX_CORE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string.h>
#endif

#include <time.h>

#include "RtAudio.h"

#include "kubus.h"
#include "inih/ini.h"



// our datetype
#define SAMPLE float
// corresponding format for RtAudio
#define MY_FORMAT RTAUDIO_FLOAT32
// sample rate
#ifndef MY_SRATE
#define MY_SRATE 48000
#endif
// number of channels
#define MY_CHANNELS 1
// for convenience
#define MY_PIE 3.14159265358979
#define BUFSIZE 1024
#define FFTSIZE 1024

KubusData g_data;

void kubus_cleanup(KubusData *kd)
{
    RtAudio *audio = (RtAudio *) kd->audio; 
    if( audio->isStreamOpen() )
    {
        audio->stopStream();
        audio->closeStream();
    }
    kiss_fftr_free(kd->cfg);
	KISS_FFT_FREE(kd->fftbuf);
    sp_rms_destroy(&kd->rms);
    sp_port_destroy(&kd->port);
    sp_rms_destroy(&kd->rms_bp);
    sp_butbp_destroy(&kd->bp);
}

static int ini_handler(void* user, const char* section, const char* name,
                   const char* value)
{
    KubusData* kd = (KubusData *)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("audio", "sr")) {
        kd->sr = atoi(value);
    }else if (MATCH("audio", "gain")) {
        kd->gain = atof(value);
    }else if (MATCH("audio", "bp_freq")) {
        kd->bp_freq = atof(value);
    }else if (MATCH("audio", "bp_bw")) {
        kd->bp_bw = atof(value);
    }else if (MATCH("audio", "bp_gain")) {
        kd->bp_gain = atof(value);
    }else if (MATCH("modes", "pulse")) {
        kd->tog_pulse = atoi(value);
    }else if (MATCH("modes", "jitter")) {
        kd->tog_jit = atoi(value);
    }else if (MATCH("modes", "fft")) {
        kd->showFFT = atoi(value);
    }else if (MATCH("modes", "rainbow")) {
        kd->tog_rainbow= atoi(value);
    }else if (MATCH("modes", "8bit")) {
        kd->tog_8bit = atoi(value);
    }else if (MATCH("visual", "scaleDefault")) {
        kd->scaleDefault = atof(value);
    }else if (MATCH("visual", "scaleMin")) {
        kd->scaleMin = atof(value);
    }else if (MATCH("visual", "scaleMax")) {
        kd->scaleMax = atof(value);
    }else if (MATCH("visual", "jit_thresh")) {
        kd->jit_thresh = atof(value);
    }else if (MATCH("visual", "fftWrap")) {
        kd->fftWrap = atoi(value);
    }else if (MATCH("visual", "color_r")) {
        kd->clr.r= atof(value) / 255.0;
    }else if (MATCH("visual", "color_g")) {
        kd->clr.g= atof(value) / 255.0;
    }else if (MATCH("visual", "color_b")) {
        kd->clr.b= atof(value) / 255.0;
    }else if (MATCH("visual", "inertia")) {
        kd->inertia = atof(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    #undef MATCH
    return 1;
}

void kubus_init(KubusData *kd)
{
    // allocate global buffer
    kd->bufferSize = BUFSIZE;
    kd->buffer = new SAMPLE[kd->bufferSize];
    kd->wbuffer = new SAMPLE[kd->bufferSize];
    memset( kd->buffer, 0, sizeof(SAMPLE)*BUFSIZE);

	// FFT init
	kd->cfg = kiss_fftr_alloc(FFTSIZE, 0, NULL, NULL);
	kd->fftbuf = (kiss_fft_cpx *) KISS_FFT_MALLOC(sizeof(float) * FFTSIZE);	
	kd->window  = new SAMPLE[BUFSIZE];
    memset( kd->window , 0, sizeof(SAMPLE)*kd->bufferSize );
	hanning(kd->window, BUFSIZE);
    kd->fftWrap = 128;

    // set scale
    kd->scaleMax = 3.0;
    kd->scaleMin = 1.0;
    kd->scaleDefault = 2.0;
    kd->scale = kd->scaleMin;
    kd->scale = 3.0;
    kd->scale_bp = 0;
    kd->sr = 44100;
    kd->jit_thresh = 0.5;

    // Set default color
    kd->clr.r = 0.1607;
    kd->clr.g = 0.6784;
    kd->clr.b = 1;

    // set default toggles
	kd->showFFT = 0; 
    kd->tog_jit = 1;
    kd->tog_pulse = 1;
    /* downsamples input signal */
    kd->tog_8bit = 1;
    /* time-signal modifies hue */
    kd->tog_rainbow = 0;
    /* time-signal modifies lightness */
    kd->tog_amp = 1;


    /* Input gain */
    kd->gain = 1;

    /* inertia of pulses */
    kd->inertia = 0.01;

    /* bandpass for jitter */
    kd->bp_bw = 10;
    kd->bp_freq = 300;
    kd->bp_gain = 4;

    //RNG seed
    srand(time(NULL));

    if(kd->useConfig) {
        if (ini_parse(kd->config, ini_handler, kd) < 0) {
            printf("Can't load '%s'\n", kd->config);
        }
    } else {
        if (ini_parse("config.ini", ini_handler, kd) < 0) {
            printf("Can't load 'config.ini', using defaults\n");
        }
    }
    
    sp_rms_create(&kd->rms);
    sp_rms_init(kd->sr, kd->rms);
    sp_port_create(&kd->port);
    sp_port_init(kd->sr, kd->port, kd->inertia);
    
    sp_rms_create(&kd->rms_bp);
    sp_rms_init(kd->sr, kd->rms_bp);

    sp_butbp_create(&kd->bp);
    sp_butbp_init(kd->sr, kd->bp);
    kd->bp->freq = kd->bp_freq;
    kd->bp->bw = kd->bp_bw;
}

//-----------------------------------------------------------------------------
// name: callme()
// desc: audio callback
//-----------------------------------------------------------------------------
int callme( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
            double streamTime, RtAudioStreamStatus status, void * data )
{

    KubusData *kd = (KubusData *)data;
    // cast!
    SAMPLE * input = (SAMPLE *)inputBuffer;
    SAMPLE * output = (SAMPLE *)outputBuffer;
    float rms = 0, port = 0, bp = 0, rms_bp; 
    // fill
    for( int i = 0; i < numFrames; i++ )
    {
        // assume mono
        g_data.buffer[i] = input[i] * kd->gain;
        sp_rms_compute(kd->rms, &g_data.buffer[i], &rms);
        sp_port_compute(kd->port, &rms, &port);

        sp_butbp_compute(kd->bp, &g_data.buffer[i], &bp);
        bp *= pow(10, kd->bp_gain / 10.0);
        sp_rms_compute(kd->rms_bp, &bp, &rms_bp);

        kd->scale = kd->scaleMin  + (kd->scaleMax - kd->scaleMin) * port;
        kd->scale_bp = rms_bp; 
        // zero output
        output[i] = 0;
    }
    
    return 0;
}


//-----------------------------------------------------------------------------
// name: main()
// desc: entry point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    // instantiate RtAudio object
    RtAudio audio;
    // variables
    // frame size
    unsigned int bufferFrames = BUFSIZE;
    
    if(argc > 1) {
        g_data.useConfig = 1;
        g_data.config = argv[1];
    } else {
        g_data.useConfig = 0;
    }
    
    kubus_init(&g_data);
    g_data.audio = &audio;


    // check for audio devices
    if( audio.getDeviceCount() < 1 )
    {
        // nopes
        cout << "no audio devices found!" << endl;
        exit( 1 );
    }
    
    // initialize GLUT
    glutInit( &argc, argv );
    // init gfx
    initGfx();
    
    // let RtAudio print messages to stderr.
    audio.showWarnings( true );
    
    // set input and output parameters
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = audio.getDefaultInputDevice();
    iParams.nChannels = MY_CHANNELS;
    iParams.firstChannel = 0;
    oParams.deviceId = audio.getDefaultOutputDevice();
    oParams.nChannels = MY_CHANNELS;
    oParams.firstChannel = 0;
    
    // create stream options
    RtAudio::StreamOptions options;
    
    // go for it
    try {
        // open a stream
        audio.openStream( &oParams, &iParams, MY_FORMAT, g_data.sr, &bufferFrames, &callme, &g_data, &options );
    }
    catch( RtError& e )
    {
        // error!
        cout << e.getMessage() << endl;
        exit( 1 );
    }
   

    // go for it
    try {
        // start stream
        audio.startStream();
        
        // let GLUT handle the current thread from here
        glutMainLoop();
        
        // stop the stream.
        audio.stopStream();
    }
    catch( RtError& e )
    {
        // print error message
        cout << e.getMessage() << endl;
        goto cleanup;
    }
    
cleanup:
    kubus_cleanup(&g_data);
    // close if open
    //if( audio.isStreamOpen() )
    //    audio.closeStream();
    // done
    //kiss_fftr_free(g_data.cfg);
	//KISS_FFT_FREE(g_data.fftbuf);
    //sp_rms_destroy(&g_data.rms);
    //sp_port_destroy(&g_data.port);
    return 0;
}


void initGfx()
{
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( g_data.width, g_data.height );
    // set the window postion
    glutInitWindowPosition( 100, 100 );
    // create the window
    glutCreateWindow( "kubus" );
    
    // set the idle function - called when idle
    glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );
    
    // set clear color
    glClearColor( 0, 0, 0, 1 );
    // enable color material
    glEnable( GL_COLOR_MATERIAL );
    // enable depth test
    glEnable( GL_DEPTH_TEST );
}




void reshapeFunc( GLsizei w, GLsizei h )
{
    // save the new window size
    g_data.width = w; g_data.height = h;
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // create the viewing frustum
    gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0, 300.0 );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    gluLookAt( 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}




void keyboardFunc( unsigned char key, int x, int y )
{
    switch( key )
    {
        case 'Q':
        case 'q':
            kubus_cleanup(&g_data);
            exit(1);
            break;
            
        case 'd':
            break;
		case 'f':
			g_data.showFFT = (g_data.showFFT == 1) ? 0 : 1;
			break;
        case 'p':
            g_data.tog_pulse = (g_data.tog_pulse == 1) ? 0 : 1;
            break;
        case 'j':
            g_data.tog_jit = (g_data.tog_jit == 1) ? 0 : 1;
            break;
        case 'b':
            g_data.tog_8bit = (g_data.tog_8bit== 1) ? 0 : 1;
            break;
        case 'r':
            g_data.tog_rainbow = (g_data.tog_rainbow == 1) ? 0 : 1;
            break;
        default:
            break;
    }
    
    glutPostRedisplay( );
}

void mouseFunc( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON )
    {
        // when left mouse button is down
        if( state == GLUT_DOWN )
        {
        }
        else
        {
        }
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        // when right mouse button down
        if( state == GLUT_DOWN )
        {
        }
        else
        {
        }
    }
    else
    {
    }
    
    glutPostRedisplay( );
}




void idleFunc( )
{
    // render the scene
    glutPostRedisplay( );
}

void displayFunc( )
{
    kubus_draw(&g_data);
}
