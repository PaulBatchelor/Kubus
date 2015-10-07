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

#include "port.h"
#include "rms.h"
#include "kubus.h"

float scale_samp(float x) 
{
    float s = floor((fabs(x) * 8)) / 8.0;
	//return 1.3 * log(x + 1);
    return s;
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

void kubus_draw(KubusData *kd) 
{
    // local state
    static GLfloat zrot = 0.0f, c = 0.0f;
    
    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	GLfloat scale;

    if(kd->tog_pulse) {
        scale = kd->scale;
    } else {
        scale = kd->scaleDefault;
    }

	GLfloat div = 1.0 / 32;
    // line width
    glLineWidth( 1.0 );
    // define a starting point
    GLfloat x = -5;
    // increment
    GLfloat xinc = ::fabs(x*2 / kd->bufferSize);
    float slope = (scale - kd->scaleMin ) / (kd->scaleMax - kd->scaleMin);

	// start primitive
    glColor3f( 1, 0, 0.3019 );
	x = -5; 
    xinc = ::fabs(x * 4 / kd->bufferSize);
    GLfloat jitX = 0, jitY = 0;
	if(kd->showFFT) {	
		kiss_fftr(kd->cfg, kd->buffer, kd->fftbuf);
		for( int i = 0; i < kd->bufferSize / 2; i++ )
		{
			//// plot
			////glVertex2f( x, 2 * g_buffer[i] - 2);
			//glVertex2f( x, 0.1 * cmp_abs(kd->fftbuf[i]) - 2 );
			//// increment x
			//x += xinc;
			glColor3f( 
				1 * scale_samp(cmp_abs(kd->fftbuf[i])), 
				0 * scale_samp(cmp_abs(kd->fftbuf[i])), 
				0.3019 * scale_samp(cmp_abs(kd->fftbuf[i])) 
			);
			int x = i % 32;
			int y = i / 32; 
			draw_square(
				(-1 + div + x * 2 * div) * scale, 
				(-1 + div + (31 - y) * 2 * div ) * scale, 
				div * scale); 

		}
	}
    
    // color
    for(int y = 0; y < 32; y++) {
		for(int x = 0; x < 32; x++) {
			glColor3f( 
				0.1607 * scale_samp(kd->buffer[32 * y + x]), 
				0.6784 * scale_samp(kd->buffer[32 * y + x]), 
				1 * scale_samp(kd->buffer[32 * y + x])
			);

            if( scale >= kd->scaleMax * 0.5 && rand() % 20 == 0 && kd->tog_jit) {
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
	}
	
	//glColor3f( 0, 1, 1 );
	//draw_square(0, 0, 1 * scale); 
    // start primitive
    glBegin( GL_LINE_STRIP );

	// apply windowing function
	apply_window(kd->buffer, kd->window, kd->bufferSize);


    // loop over buffer
    //for( int i = 0; i < kd->bufferSize; i++ )
    //{
    //    // plot
    //    glVertex2f( x, 2*kd->buffer[i] + 2 );
    //    // increment x
    //    x += xinc;
    //}
    
    // end primitive
    glEnd();
    
    
    // end primitive

    // flush!
    glFlush( );
    // swap the double buffer
    glutSwapBuffers( );
}
