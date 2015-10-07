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
    // increment
    float slope = (scale - kd->scaleMin ) / (kd->scaleMax - kd->scaleMin);
    int x, y;
	// start primitive
    glColor3f( 1, 0, 0.3019 );
    GLfloat jitX = 0, jitY = 0;
	if(kd->showFFT) {	
        /* this makes the time domain look bad, fix */
        apply_window(kd->buffer, kd->window, kd->bufferSize);
		kiss_fftr(kd->cfg, kd->buffer, kd->fftbuf);
		for( int i = 0; i < kd->bufferSize / 2; i++ )
		{
			glColor3f( 
				1 * scale_samp(cmp_abs(kd->fftbuf[i])), 
				0 * scale_samp(cmp_abs(kd->fftbuf[i])), 
				0.3019 * scale_samp(cmp_abs(kd->fftbuf[i])) 
			);
			x = i % 16;
			y = i / 16; 
			draw_square(
				(-1 + div + x * 2 * div) * scale, 
				(-1 + div + (31 - y) * 2 * div ) * scale, 
				div * scale); 

		}
	}
    
    for(int i = 0; i < kd->bufferSize; i++) {
        x = i % 32;
        y = i / 32; 

        glColor3f( 
            0.1607 * scale_samp(kd->buffer[32 * y + x]), 
            0.6784 * scale_samp(kd->buffer[32 * y + x]), 
            1 * scale_samp(kd->buffer[32 * y + x])
        );
    

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

        //kd->grid[i].color[0] = 0.1607 * scale_samp(kd->buffer[32 * y + x]);
        //kd->grid[i].color[1] = 0.6784 * scale_samp(kd->buffer[32 * y + x]);
        //kd->grid[i].color[2] = 1 * scale_samp(kd->buffer[32 * y + x]);

        
        //kd->grid[i].x = ((-1 + div + x * 2 * div) + jitX) * scale;
        //kd->grid[i].y = ((-1 + div + (31 - y) * 2 * div) + jitY) * scale; 

        draw_square(
            ((-1 + div + x * 2 * div) + jitX) * scale, 
            ((-1 + div + (31 - y) * 2 * div) + jitY) * scale, 
            div * scale); 
	}

    //for(int i = 0; i < kd->bufferSize; i++) {
    //    glColor3f( 
    //        kd->grid[i].color[0], 
    //        kd->grid[i].color[1], 
    //        kd->grid[i].color[2]
    //    );
    //    draw_square(kd->grid[i].x, kd->grid[i].y, scale);
    //}

    
    // end primitive

    // flush!
    glFlush( );
    // swap the double buffer
    glutSwapBuffers( );
}
