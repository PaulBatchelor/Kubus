#include "kissfft/kiss_fftr.h"
#define SAMPLE float
#define cmp_abs(x) ( sqrt( (x).r * (x).r + (x).i * (x).i ) )

typedef struct {
    float color[3];
    float x, y;
} KSquare;

typedef struct {
long width;
long height;
SAMPLE *buffer;
SAMPLE *window;
long bufferSize;
kiss_fftr_cfg cfg;
kiss_fft_cpx *fftbuf;
int showFFT;
float scale;
float scaleMax, scaleMin, scaleDefault;
sp_rms *rms;
sp_port *port;

int tog_jit;
int tog_pulse;

float jit_thresh;

int sr;

void *audio;

KSquare grid[1024];

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
void kubus_init(KubusData *kd);
void kubus_cleanup(KubusData *kd);

