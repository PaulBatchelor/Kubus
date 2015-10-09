#include "kissfft/kiss_fftr.h"
#define SAMPLE float
#define cmp_abs(x) ( sqrt( (x).r * (x).r + (x).i * (x).i ) )

typedef struct {
float r, g, b;
} KColor;

typedef struct {
    long width;
    long height;
    SAMPLE *buffer;
    SAMPLE *window;
    SAMPLE *wbuffer;
    long bufferSize;
    kiss_fftr_cfg cfg;
    kiss_fft_cpx *fftbuf;
    int showFFT;
    float scale;
    float scaleMax, scaleMin, scaleDefault;
    sp_rms *rms;
    sp_port *port;
    /* skip upper bins and wrap */
    int fftWrap;

    int tog_jit;
    int tog_pulse;
    int tog_8bit;
    int tog_rainbow;
    int tog_amp;

    float jit_thresh;

    int sr;

    void *audio;

    KColor clr;
} KubusData;

void initGfx();
void idleFunc();
void displayFunc();
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void hamming( float * window, unsigned long length );
void hanning( float * window, unsigned long length );
void apply_window( float * data, float *out, float * window, unsigned long length );

void kubus_draw(KubusData *kd);
void kubus_init(KubusData *kd);
void kubus_cleanup(KubusData *kd);

void kcolor_set(KColor *clr, KColor *out);
void kcolor_scale(KColor *clr, float scale, int rainbows);
void kcolor_blend(KColor *clr1, KColor *clr2, KColor *out, float blend);
void kcolor_color(KColor *clr);
