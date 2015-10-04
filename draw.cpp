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

void kubus_draw(KubusData *kd) 
{
    // local state
    static GLfloat zrot = 0.0f, c = 0.0f;
    
    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // line width
    glLineWidth( 1.0 );
    // define a starting point
    GLfloat x = -5;
    // increment
    GLfloat xinc = ::fabs(x*2 / kd->bufferSize);
    
    // color
    glColor3f( 1, 0, .3019 );
    
    // start primitive
    glBegin( GL_LINE_STRIP );

	// apply windowing function
	apply_window(kd->buffer, kd->window, kd->bufferSize);
 
    // loop over buffer
    for( int i = 0; i < kd->bufferSize; i++ )
    {
        // plot
        glVertex2f( x, 2*kd->buffer[i] + 2 );
        // increment x
        x += xinc;
    }
    
    // end primitive
    glEnd();
    
	// start primitive
    glColor3f( 0.1607, 0.6784, 1 );
    glBegin( GL_LINE_STRIP );
	x = -5; 
    xinc = ::fabs(x * 4 / kd->bufferSize);

	kiss_fftr(kd->cfg, kd->buffer, kd->fftbuf);

	for( int i = 0; i < kd->bufferSize / 2; i++ )
	{
		// plot
		//glVertex2f( x, 2 * g_buffer[i] - 2);
		glVertex2f( x, 0.1 * cmp_abs(kd->fftbuf[i]) - 2 );
		// increment x
		x += xinc;
	}
    
    // end primitive
    glEnd();

    // flush!
    glFlush( );
    // swap the double buffer
    glutSwapBuffers( );
}
