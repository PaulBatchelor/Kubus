#include "kissfft/kiss_fftr.h"
#define SAMPLE float
#define cmp_abs(x) ( sqrt( (x).r * (x).r + (x).i * (x).i ) )

typedef struct {
long width;
long height;
SAMPLE *buffer;
SAMPLE *window;
long bufferSize;
kiss_fftr_cfg cfg;
kiss_fft_cpx *fftbuf;
} KubusData;

void initGfx();
void idleFunc();
void displayFunc();
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void hamming( float * window, unsigned long length );
void hanning( float * window, unsigned long length );
void apply_window( float * data, float * window, unsigned long length );

void kubus_draw(KubusData *kd);
