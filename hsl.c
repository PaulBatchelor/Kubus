#include <math.h>
#include <stdio.h>

int my_max(int r, int g, int b) 
{
    if (r > g && r > b) return r;
    else 
    if (g > r && g > b) return g;
    else
    if (b > r && b > g) return b;
    return 0;
}

int my_min(int r, int g, int b) 
{
    if (r < g && r < b) return r;
    else 
    if (g < r && g < b) return g;
    else
    if (b < r && b < g) return b;
    return 0;
}

float get_hue(int r, int g, int b)
{
    int min = my_min(r, g, b);
    int max = my_max(r, g, b);
    int c = max - min;

    float H1 = 0;

    if( c == 0) return 0;

    if (max == r) {
        H1 = (float)(g - b) / c; 
        H1 = fmod(H1, 6);
    } else if (max == g) {
        H1 = (float)(b - r) / c; 
        H1 += 2;
    } else if (max == b) {
        H1 = (float)(r - g) / c; 
        H1 += 4;
    } 

    return H1 * 60.0;
}

float get_lightness(int r, int g, int b)
{
    return ((0.5 * (my_max(r, g, b) + my_min(r, g, b))) / 255.0);
}

float get_saturation(int r, int g, int b)
{
    float max = my_max(r, g, b) / 255.0;
    float min = my_min(r, g, b) / 255.0;
    float c = (max - min);
    float L = 0.5 * (max + min);

    if (c == 0) return 0;

    return (1.0 *  c / (1 - fabs(2 * L - 1)));
}


/* Algorithms and math based on "Math behind colorspace conversions, RGB-HSL" 
 * by Nikolai Waldman, found here:
 * http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
 */
float HSL_to_rgb(float h, float s, float l, float *rgb) 
{
    float color[3], tmpclr[3];
    float tmp1, tmp2;
    int i;

    if(l < 0.5) {
        tmp1 = l * (1 + s);
    } else {
        tmp1 = (l + s) - (l * s);
    }
    
    tmp2 = 2 * l - tmp1; 

    h = h / 360.0;

    tmpclr[0] = h + 0.333;
    tmpclr[1] = h;
    tmpclr[2] = h - 0.333;

    for(i = 0; i < 3; i++) {
        if(tmpclr[i] < 0) tmpclr[i]++;
        else if(tmpclr[i] > 1) tmpclr[i]--;

        if(6 * tmpclr[i] < 1) { 
            color[i] = tmp2 + (tmp1 - tmp2) * 6 * tmpclr[i];
        } else if (2 * tmpclr[i] < 1) {
            color[i] = tmp1;
        } else if (3 * tmpclr[i] < 2) {
            color[i] = tmp2 + (tmp1 - tmp2) * (0.666 - tmpclr[i]) * 6;
        } else {
            color[i] = tmp2;
        }
    }

    //printf("The r value is %g\n", color[0] * 255);
    //printf("The g value is %g\n", color[1] * 255);
    //printf("The b value is %g\n", color[2] * 255);

    rgb[0] = color[0];
    rgb[1] = color[1];
    rgb[2] = color[2];

    return 0;
}


/* From https://www.cs.rit.edu/~ncs/color/t_convert.html */
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}

}

/*
int main() {
    int r = 145, g = 200, b = 47;

    float h = 82, s = 62, l = 48;

    float rgb[3];

    printf("The RGB values are %d, %d, and %d\n", r, g, b);
    printf("The HSV values are %g, %g, and %g\n", h, s, l);
    printf("The values we got are %g, %g, and %g\n", 
            get_hue(r, g, b),
            get_saturation(r, g, b),
            get_lightness(r, g, b));

    HSL_to_rgb(
            get_hue(r, g, b),
            get_saturation(r, g, b),
            get_lightness(r, g, b),
            rgb
    );

    return 0;
}
*/
