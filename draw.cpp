#include <math.h>
#include <stdlib.h>
#include <time.h>
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

#include "hsl.h"
#include "kubus.h"

void kubus_fftrotate(KubusData *kd)
{
    for(int i = 0; i < FFT_HIST; i++) {
        if(kd->fftblock_pos[i] > 0) {
            kd->fftblock_pos[i] = kd->fftblock_pos[i] - 1;
        } else {
            kd->fftblock_pos[i] = FFT_HIST -1;
        }
    }
}

void kubus_draw_fft_line(KubusData *kd, kiss_fft_cpx *fftbuf, float strength)
{
    // start primitive
    KColor clr;
    kcolor_set(&kd->clr, &clr);
    kcolor_scale(&clr, strength, 0);
    kcolor_color(&clr);
    //glColor3f(1 * strength, 0 * strength, 0 * strength); 
    glBegin( GL_LINE_STRIP );
    GLfloat x = -5;
    GLfloat xinc = ::fabs(x*2 / (kd->bufferSize / 2));
    GLfloat div = (1 / 32.0);
    GLfloat y = (-1 + (1/32.0) + (68.0 * 2.0 * div));
    // loop over buffer
    for( int i = 0; i < kd->bufferSize / 2; i++ )
    {
        // plot
        glVertex2f( x , -y + 0.01 * 
                cmp_abs(fftbuf[i]));
        // increment x
        x += xinc;
    }
    
    // end primitive
    glEnd();

}

float scale_samp(float x) 
{
    float s = floor((fabs(x) * 8)) / 8.0;
	//return 1.3 * log(x + 1);
    return s;
}
void kcolor_set(KColor *clr, KColor *out)
{
    out->r = clr->r;
    out->g = clr->g;
    out->b = clr->b;
}

void kcolor_scale(KColor *clr, float scale, int rainbows)
{
    if(rainbows) {
        HSVtoRGB(&clr->r, &clr->g, &clr->b, scale * 360, 1, 1);
        return;
    }

    float h, s, l;
    float rgb[3];

    rgb[0] = clr->r * 255;
    rgb[1] = clr->g * 255;
    rgb[2] = clr->b * 255;

    h = get_hue(rgb[0], rgb[1], rgb[2]);
    s = get_saturation(rgb[0], rgb[1], rgb[2]);
    l = get_lightness(rgb[0], rgb[1], rgb[2]);
   
    l *= scale;

    HSL_to_rgb(h, s, l, rgb);

    clr->r = rgb[0];
    clr->g = rgb[1];
    clr->b = rgb[2];
    
}

void kcolor_fft(KColor *clr, float scale)
{
    float h, s, l;
    float rgb[3];

    if(scale > 1) scale = 1;
    rgb[0] = clr->r * 255;
    rgb[1] = clr->g * 255;
    rgb[2] = clr->b * 255;

    h = get_hue(rgb[0], rgb[1], rgb[2]);
    s = get_saturation(rgb[0], rgb[1], rgb[2]);
    l = get_lightness(rgb[0], rgb[1], rgb[2]);


    s *= scale;
    
    HSL_to_rgb(h, s, l, rgb);

    clr->r = rgb[0];
    clr->g = rgb[1];
    clr->b = rgb[2];
}

void kcolor_color(KColor *clr)
{
    glColor3f(clr->r, clr->g, clr->b); 
}

static void draw_square(float x, float y, float scale) 
{
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(x + -1.0f * scale , y + -1.0f * scale);
    glVertex2f(x + -1.0f * scale, y + 1.0f * scale);
    glVertex2f(x + 1.0f * scale, y + -1.0f * scale);
    glVertex2f(x + 1.0f * scale, y + 1.0f * scale);
    glEnd();
}

void kcolor_blend(KColor *clr1, KColor *clr2, KColor *out, float blend)
{
    out->r = (1.0 * blend * clr2->r) + ((1.0 - blend) * clr1->r);
    out->g = (1.0 * blend * clr2->g) + ((1.0 - blend) * clr1->g);
    out->b = (1.0 * blend * clr2->b) + ((1.0 - blend) * clr1->b);
}


void kubus_draw(KubusData *kd) 
{
    static GLfloat zrot = 0.0f, c = 0.0f;
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	GLfloat scale;


    KColor out;

    if(kd->tog_pulse) {
        scale = kd->scale;
    } else {
        scale = kd->scaleDefault;
    }
    
	GLfloat div = 1.0 / 32;
    glLineWidth( 1.0 );
    float slope = (scale - kd->scaleMin ) / (kd->scaleMax - kd->scaleMin);
    int x, y;
    int binNum = 0;
    GLfloat jitX = 0, jitY = 0;
    binNum = 0; 
    apply_window(kd->buffer, kd->wbuffer, kd->window, kd->bufferSize);
    kiss_fftr(kd->cfg, kd->wbuffer, kd->fftblock[kd->fftblock_pos[0]]);


    for(int i = 0; i < kd->bufferSize; i++) {
        x = i % 32;
        y = i / 32; 
        if(x % 2 == 0) { 
            binNum++;
            binNum %= kd->fftWrap;   
        } 
        kcolor_set(&kd->clr, &out); 

        if(kd->tog_8bit) {
            kcolor_scale(&out, scale_samp(kd->buffer[32 * y + x]), kd->tog_rainbow);
        } else {
            kcolor_scale(&out, fabs(kd->buffer[32 * y + x]), kd->tog_rainbow);
        }

        //if(kd->showFFT) {
        //    kcolor_fft(&out,  0.2 * cmp_abs(kd->fftblock[
        //                kd->fftblock_pos[0]][binNum]));
        //}
        kcolor_color(&out);

        if( kd->scale_bp >= kd->jit_thresh && rand() % 20 == 0 && kd->tog_jit ) {
            jitX = 1.0 * rand() / RAND_MAX;
            jitX *= 2;
            jitX -= 1;
            jitX = scale * jitX * slope;
            jitY = 1.0 * rand() / RAND_MAX;
            jitY *= 2;
            jitY -= 1;
            jitY = scale * jitY * slope;
        }

        draw_square(
            ((-1 + div + x * 2 * div) + jitX) * scale, 
            ((-1 + div + (31 - y) * 2 * div) + jitY) * scale, 
            div * scale); 
	}

    if(kd->showFFT) {
        for(int i = 0; i < FFT_HIST - 1; i++) {
            kubus_draw_fft_line(kd, kd->fftblock[kd->fftblock_pos[i]], 
                    (1.0 / FFT_HIST) * (FFT_HIST - i));
        }
        kubus_fftrotate(kd);
    }


    glFlush( );
    glutSwapBuffers( );
}
