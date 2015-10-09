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
#include "port.h"
#include "rms.h"
#include "kubus.h"

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

void kcolor_scale(KColor *clr, float scale)
{
    float h, s, l;
    float rgb[3];
    rgb[0] = clr->r * 255;
    rgb[1] = clr->g * 255;
    rgb[2] = clr->b * 255;

    h = get_hue(rgb[0], rgb[1], rgb[2]);
    s = get_saturation(rgb[0], rgb[1], rgb[2]);
    l = get_lightness(rgb[0], rgb[1], rgb[2]);
    
    h = (scale * 360);
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
    // local state
    static GLfloat zrot = 0.0f, c = 0.0f;
    
    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	GLfloat scale;

    KColor clr1, clr2, out;
    clr1.r = 0.1607;
    clr1.g = 0.6784;
    clr1.b = 1;
    
    clr2.r = 1;
    clr2.g = 0;
    clr2.b = 0.3019;

    if(kd->tog_pulse) {
        scale = kd->scale;
    } else {
        scale = kd->scaleDefault;
    }

	GLfloat div = 1.0 / 32;
    // line width
    glLineWidth( 1.0 );
    // define a starting point
    // increment
    float slope = (scale - kd->scaleMin ) / (kd->scaleMax - kd->scaleMin);
    int x, y;
    int binNum = 0;
	// start primitive
    GLfloat jitX = 0, jitY = 0;
    binNum = 0; 
    apply_window(kd->buffer, kd->wbuffer, kd->window, kd->bufferSize);
    kiss_fftr(kd->cfg, kd->wbuffer, kd->fftbuf);
    for(int i = 0; i < kd->bufferSize; i++) {
        x = i % 32;
        y = i / 32; 
        if(x % 2 == 0) { 
            binNum++;
            binNum %= 128;   
        } 
        kcolor_set(&clr2, &out); 
        //kcolor_scale(&out, scale_samp(kd->buffer[32 * y + x]));
        kcolor_scale(&out, kd->buffer[32 * y + x]);
        if(kd->showFFT) {
            kcolor_fft(&out,  0.2 * cmp_abs(kd->fftbuf[binNum]));
        }
        kcolor_color(&out);

        if( scale >= kd->scaleMax * kd->jit_thresh && rand() % 20 == 0 && kd->tog_jit ) {
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

    glFlush( );
    glutSwapBuffers( );
}
