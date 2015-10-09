#ifdef __cplusplus
extern "C" {
#endif
float get_hue(int r, int g, int b);
float get_lightness(int r, int g, int b);
float get_saturation(int r, int g, int b);
float HSL_to_rgb(float h, float s, float l, float *rgb);
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );
#ifdef __cplusplus
}
#endif
