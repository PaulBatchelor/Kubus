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

#include "kubus.h"

float scale_samp(float x) 
{
	return 1.3 * log(fabs(x) + 1);
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
	GLfloat scale = 3.0;
	GLfloat div = 1.0 / 32;
    // line width
    glLineWidth( 1.0 );
    // define a starting point
    GLfloat x = -5;
    // increment
    GLfloat xinc = ::fabs(x*2 / kd->bufferSize);
	
	// start primitive
    glColor3f( 1, 0, 0.3019 );
	x = -5; 
    xinc = ::fabs(x * 4 / kd->bufferSize);

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
				(-1 + div + (31 - y) * 2 * div) * scale, 
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
			draw_square(
				(-1 + div + x * 2 * div) * scale, 
				(-1 + div + (31 - y) * 2 * div) * scale, 
				div * scale); 
		}
	}
	
	glColor3f( 0, 1, 1 );
	draw_square(0, 0, 1 * scale); 
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
