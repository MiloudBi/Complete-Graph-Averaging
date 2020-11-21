
#include <stdint.h>
#include <stdio.h>
#include <windows.h> 
#include <string.h>
#include <mcimage.h>
#include <stdlib.h>
#include<math.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX

#define MAX(a,b) ((a)>(b)?(a):(b))

/* ==================================== */
uint32_t lpixelwiseCGA(struct xvimage* image, int sigma) {

    double windows_size, research_window_size, hfp; // hfp e filtering parameter set depending on the value of σ
    int  r, f; //r: kernel 'radius'  might pass this as function parameter

    // the relation between r and research window size is dim(research window size)=(2xr+1)*(2xr+1)
    // the relation between f and windows size is dim(window size)=(2xf+1)*(2xf+1)
    // this part of the program for give us windows size and research window size and h parameters from sigma value ==> look table 1 in part 4
    if (0 < sigma && sigma <= 15) {
        windows_size = 3;
        research_window_size = 21;
        hfp = 0.40 * sigma;
        r = (research_window_size - 1) / 2;
        f = (windows_size - 1) / 2;
    }
    else {
        if (15 < sigma && sigma <= 30) {
            windows_size = 5;
            research_window_size = 21;
            hfp = 0.40 * sigma;
            r = (research_window_size - 1) / 2;
            f = (windows_size - 1) / 2;

        }
        else {
            if (30 < sigma && sigma <= 45) {
                windows_size = 7;
                research_window_size = 35;
                hfp = 0.35 * sigma;
                r = (research_window_size - 1) / 2;
                f = (windows_size - 1) / 2;
            }
            else {
                if (45 < sigma && sigma <= 75) {
                    windows_size = 9;
                    research_window_size = 35;
                    hfp = 0.35 * sigma;
                    r = (research_window_size - 1) / 2;
                    f = (windows_size - 1) / 2;
                }
                else {
                    if (75 < sigma && sigma <= 100) {
                        windows_size = 11;
                        research_window_size = 35;
                        hfp = 0.30 * sigma;
                        r = (research_window_size - 1) / 2;
                        f = (windows_size - 1) / 2;
                    }
                }
            }
        }
    }

    printf("the research window size is %lf \n", research_window_size);
    printf("the  window size is %lf \n", windows_size);
    printf("the filtering parameter  is %lf \n", hfp);
    printf("the standard deviation(sigma) is %d \n", sigma);
    printf("the r reduis is %d \n", r);
    printf("the f reduis s %d \n", f);



    double dis, max_value, weight, CP, up;
    int  J, I, j, i;
    uint8_t* ptrimage, * ptrimagetemp; // = {1,2,1,2,4,2,1,2,1}, gaussian kernel
    uint32_t rs, cs, N, Im;
    struct xvimage* imagetemp;

    rs = image->row_size;
    cs = image->col_size;
    N = rs * cs;

    imagetemp = allocimage(NULL, rs, cs, 1, VFF_TYP_1_BYTE);

    ptrimage = UCHARDATA(image);
    ptrimagetemp = UCHARDATA(imagetemp);


    for (Im = 0; Im < N; Im++) {
        if (Im <= rs * (r + f) || Im >= N - rs * (r - 1 + f))
            ptrimagetemp[Im] = ptrimage[Im];


        else if (Im % rs <= r - 1 + f || Im % rs >= rs - 1 - r - f)
            ptrimagetemp[Im] = ptrimage[Im];


        else {                                    

            up = 0;
            CP = 0;
            for (I = -r; I < r + 1; I++) {           //research window for loops 
                for (J = -r; J < r + 1; J++) {
                    
                    dis = 0;

                    for (i = -f; i < f + 1; i++) {
                        for (j = -f; j < f + 1; j++) {

                            dis += pow((ptrimage[(Im + j + i * rs)] / 1. - ptrimage[(Im + j + J + (I + i) * rs)] / 1.), 2.);  //distace between two window u(p + j)  - u (q- j)   

                        }
                    }
                    max_value = max((pow(2 * f + 1, -2) * dis) - 2 * sigma * sigma, 0);
                    if (max_value < 0) max_value = 0;
                    weight = exp(-max_value / (hfp * hfp));
                    CP += weight;
                    up += weight * ptrimage[Im + J + I * rs];
                }
            }

            ptrimagetemp[Im] = up / CP;

        }
    }

    for (Im = 0; Im < N; Im++)
        ptrimage[Im] = ptrimagetemp[Im];


    return 1;

}



/* =============================================================== */
int main(int argc, char** argv)
/* =============================================================== */
{
    struct xvimage* image;


    if (argc != 4)
    {
        fprintf(stderr, "usage: %s in1.pgm sigma out.pgm \n", argv[0]);
        exit(0);
    }


    image = readimage(argv[1]);
    if (image == NULL)
    {
        fprintf(stderr, "PixelWise: readimage failed\n");
        exit(0);
    }
    char* p;
    int sigma;

    errno = 0;
    long conv = strtol(argv[2], &p, 10);

    // Check for errors: e.g., the string does not represent an integer
    // or the integer is larger than int
    if (errno != 0 || *p != '\0' || conv > INT_MAX) {
        // Put here the handling of the error, like exiting the program with
        // an error message
    }
    else {
        // No error
        sigma = conv;
        printf("sigma value is %d\n", sigma);
    }


    if (!lpixelwiseCGA(image, sigma))
    {
        fprintf(stderr, " function pixelWise failed\n");
        exit(0);
    }
    printf("good! function succed\n");
    writeimage(image, argv[3]);
    freeimage(image);
    freeimage(image);

    return 0;
} /* main */