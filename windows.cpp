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

void hamming( float * window, unsigned long length )
{
    unsigned long i;
    double pi, phase = 0, delta;

    pi = 4.*::atan(1.0);
    delta = 2 * pi / (double) length;

    for( i = 0; i < length; i++ )
    {
        window[i] = (float)(0.54 - .46*cos(phase));
        phase += delta;
    }
}

void hanning( float * window, unsigned long length )
{
    unsigned long i;
    double pi, phase = 0, delta;

    pi = 4.*atan(1.0);
    delta = 2 * pi / (double) length;

    for( i = 0; i < length; i++ )
    {
        window[i] = (float)(0.5 * (1.0 - cos(phase)));
        phase += delta;
    }
}

//-----------------------------------------------------------------------------
// name: apply_window()
// desc: apply a window to data
//-----------------------------------------------------------------------------
void apply_window( float * data, float * window, unsigned long length )
{
    unsigned long i;

    for( i = 0; i < length; i++ )
        data[i] *= window[i];
}
