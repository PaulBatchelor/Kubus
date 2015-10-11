#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    SPFLOAT sr, freq, bw, istor;
    SPFLOAT lkf, lkb;
    SPFLOAT a[8];
    SPFLOAT pidsr, tpidsr;
} sp_butbp;

int sp_butbp_create(sp_butbp **p);
int sp_butbp_destroy(sp_butbp **p);
int sp_butbp_init(int sr, sp_butbp *p);
int sp_butbp_compute(sp_butbp *p, SPFLOAT *in, SPFLOAT *out);

#ifdef __cplusplus
}
#endif
