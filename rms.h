#ifdef __cplusplus
extern "C" {
#endif

typedef struct sp_rms{
    SPFLOAT ihp, istor;
    SPFLOAT c1, c2, prvq;
} sp_rms;

int sp_rms_create(sp_rms **p);
int sp_rms_destroy(sp_rms **p);
int sp_rms_init(int sr, sp_rms *p);
int sp_rms_compute(sp_rms *p, SPFLOAT *in, SPFLOAT *out);

#ifdef __cplusplus
}
#endif
