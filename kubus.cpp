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


#include "RtAudio.h"
#include "kubus.h"

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


//-----------------------------------------------------------------------------
// name: callme()
// desc: audio callback
//-----------------------------------------------------------------------------
int callme( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
            double streamTime, RtAudioStreamStatus status, void * data )
{
    // cast!
    SAMPLE * input = (SAMPLE *)inputBuffer;
    SAMPLE * output = (SAMPLE *)outputBuffer;
    
    // fill
    for( int i = 0; i < numFrames; i++ )
    {
        // assume mono
        g_data.buffer[i] = input[i];
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
    unsigned int bufferBytes = 0;
    // frame size
    unsigned int bufferFrames = BUFSIZE;
    
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
        // audio.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, (void *)&bufferBytes, &options );
        audio.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, NULL, &options );
    }
    catch( RtError& e )
    {
        // error!
        cout << e.getMessage() << endl;
        exit( 1 );
    }
    
    // compute
    bufferBytes = bufferFrames * MY_CHANNELS * sizeof(SAMPLE);
    // allocate global buffer
    g_data.bufferSize = bufferFrames;
    g_data.buffer = new SAMPLE[g_data.bufferSize];
    memset( g_data.buffer, 0, sizeof(SAMPLE)*BUFSIZE);

	// FFT init
	g_data.cfg = kiss_fftr_alloc(FFTSIZE, 0, NULL, NULL);

	g_data.fftbuf = (kiss_fft_cpx *) KISS_FFT_MALLOC(sizeof(float) * FFTSIZE);	
    
	g_data.window  = new SAMPLE[BUFSIZE];
    memset( g_data.window , 0, sizeof(SAMPLE)*g_data.bufferSize );
	hanning(g_data.window, BUFSIZE);

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
    // close if open
    if( audio.isStreamOpen() )
        audio.closeStream();
    // done
    kiss_fftr_free(g_data.cfg);
	KISS_FFT_FREE(g_data.fftbuf);
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
            exit(1);
            break;
            
        case 'd':
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
