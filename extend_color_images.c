
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
uint32_t lpixelwiseCGA_rgb( struct xvimage* red_image, struct xvimage* green_image, struct xvimage* bleu_image, int sigma) {

    double windows_size, research_window_size, hfp; // hfp e filtering parameter set depending on the value of σ
    int  r , f ; //r: kernel 'radius'  might pass this as function parameter
    
    // the relation between r and research window size is dim(research window size)=(2xr+1)*(2xr+1)
    // the relation between f and windows size is dim(window size)=(2xf+1)*(2xf+1)
    // this part of the program for give us windows size and research window size and h parameters from sigma value ==> look table 1 in part 4
    

    printf("the research window size is %lf \n",research_window_size);
    printf("the  window size is %lf \n",windows_size);
    printf("the filtering parameter  is %lf \n",hfp);
    printf("the standard deviation(sigma) is %d \n",sigma);
    printf("the r reduis is %d \n",r);
    printf("the f reduis s %d \n",f);


   
    double dis, max_value, weight, CP, up;  
    int  J, I, j, i;
    uint8_t* ptrimage, * ptrimagetemp; 
    uint32_t rs, cs, N, Im;
    uint32_t rsr, csr, Nr;// for red image same dim for green and blue
    struct xvimage* imagetemp;
    // the red image and green image and blue image are the same  dim so let calculate just for once image:
    
    
    
    // for red image
    rsr = red_image->row_size;
    csr = red_image->col_size;
    Nr = rsr * csr;

    imagetemp = allocimage(NULL, rs, cs, 1, VFF_TYP_1_BYTE);

    
    ptrimagetemp = UCHARDATA(imagetemp);
    // for red image
    uint8_t* ptrimage_red, * ptrimagetemp_red;
    struct xvimage* imagetemp_red;
    imagetemp_red = allocimage(NULL, rsr, csr, 1, VFF_TYP_1_BYTE);

    ptrimage_red = UCHARDATA(red_image);
    ptrimagetemp_red = UCHARDATA(imagetemp_red);
    // for green image
    uint8_t* ptrimage_green, * ptrimagetemp_green;
    struct xvimage* imagetemp_green;
    imagetemp_green = allocimage(NULL, rsr, csr, 1, VFF_TYP_1_BYTE);

    ptrimage_green = UCHARDATA(green_image);
    ptrimagetemp_green = UCHARDATA(imagetemp_green);
    // for blue image
    uint8_t* ptrimage_blue, * ptrimagetemp_blue;
    struct xvimage* imagetemp_blue;
    imagetemp_blue = allocimage(NULL, rsr, csr, 1, VFF_TYP_1_BYTE);

    ptrimage_blue = UCHARDATA(bleu_image);
    ptrimagetemp_blue = UCHARDATA(imagetemp_blue);
    

    
    // i will make the traitement for each images, i can creat function but not much with me hh
    // let start with red
/*===================================================red image=========================*/
    for (Im = 0; Im < Nr; Im++) {
        if (Im <= rsr * (r + f) || Im >= Nr - rsr * (r - 1 + f))
            ptrimagetemp_red[Im] = ptrimage_red[Im];


        else if (Im % rsr <= r - 1 + f || Im % rsr >= rsr - 1 - r - f)
            ptrimagetemp_red[Im] = ptrimage_red[Im];


        else {                                   

            up = 0;
            CP = 0;
            for (I = -r; I < r + 1; I++) {           
                for (J = -r; J < r + 1; J++) {
                    
                    dis = 0;

                    for (i = -f; i < f + 1; i++) {
                        for (j = -f; j < f + 1; j++) {

                            dis += pow((ptrimage_red[(Im + j + i * rsr)] / 1. - ptrimage_red[(Im + j + J + (I + i) * rsr)] / 1.), 2.);   
                            
                        }
                    }
                    max_value = max((pow(2 * f + 1, -2) * dis) - 2 * sigma * sigma, 0);
                    if (max_value < 0) max_value = 0;
                    weight = exp(-max_value / (hfp * hfp));
                    CP += weight;
                    up += weight * ptrimage_red[Im + J + I * rsr];
                }
            }

            ptrimagetemp_red[Im] = up / CP;

        }
    }

    for (Im = 0; Im < N; Im++)
        ptrimage_red[Im] = ptrimagetemp_red[Im];

    /*===================================================green image=========================*/
    for (Im = 0; Im < N; Im++) {
        if (Im <= rsr * (r + f) || Im >= N - rsr * (r - 1 + f))
            ptrimagetemp_green[Im] = ptrimage_green[Im];


        else if (Im % rsr <= r - 1 + f || Im % rsr >= rsr - 1 - r - f)
            ptrimagetemp_green[Im] = ptrimage_green[Im];


        else {

            up = 0;
            CP = 0;
            for (I = -r; I < r + 1; I++) {
                for (J = -r; J < r + 1; J++) {

                    dis = 0;

                    for (i = -f; i < f + 1; i++) {
                        for (j = -f; j < f + 1; j++) {

                            dis += pow((ptrimage_green[(Im + j + i * rsr)] / 1. - ptrimage_green[(Im + j + J + (I + i) * rsr)] / 1.), 2.);

                        }
                    }
                    max_value = max((pow(2 * f + 1, -2) * dis) - 2 * sigma * sigma, 0);
                    if (max_value < 0) max_value = 0;
                    weight = exp(-max_value / (hfp * hfp));
                    CP += weight;
                    up += weight * ptrimage_green[Im + J + I * rsr];
                }
            }

            ptrimagetemp_green[Im] = up / CP;

        }
    }

    for (Im = 0; Im < Nr; Im++)
        ptrimage_green[Im] = ptrimagetemp_green[Im];

    /*===================================================blue image=========================*/
    for (Im = 0; Im < N; Im++) {
        if (Im <= rsr * (r + f) || Im >= N - rsr * (r - 1 + f))
            ptrimagetemp_blue[Im] = ptrimage_blue[Im];


        else if (Im % rsr <= r - 1 + f || Im % rsr >= rsr - 1 - r - f)
            ptrimagetemp_blue[Im] = ptrimage_blue[Im];


        else {

            up = 0;
            CP = 0;
            for (I = -r; I < r + 1; I++) {
                for (J = -r; J < r + 1; J++) {

                    dis = 0;

                    for (i = -f; i < f + 1; i++) {
                        for (j = -f; j < f + 1; j++) {

                            dis += pow((ptrimage_blue[(Im + j + i * rsr)] / 1. - ptrimage_blue[(Im + j + J + (I + i) * rsr)] / 1.), 2.);

                        }
                    }
                    max_value = max((pow(2 * f + 1, -2) * dis) - 2 * sigma * sigma, 0);
                    if (max_value < 0) max_value = 0;
                    weight = exp(-max_value / (hfp * hfp));
                    CP += weight;
                    up += weight * ptrimage_blue[Im + J + I * rsr];
                }
            }

            ptrimagetemp_blue[Im] = up / CP;

        }
    }

    for (Im = 0; Im < Nr; Im++)
        ptrimage_blue[Im] = ptrimagetemp_blue[Im];

    
    return 1;

}



/* =============================================================== */
int main(int argc, char** argv)
/* =============================================================== */
{
    struct xvimage* image;
    struct xvimage* r; // for read red grayscale
    struct xvimage* g;// fro read green grayscale
    struct xvimage* b; // for read bleu color

    if (argc != 4)
    {
        fprintf(stderr, "usage: %s in1.pgm sigma out.pgm \n", argv[0]);
        exit(0);
    }
    

    if (readrgbimage(argv[1], &r, &g, &b) == 0)
    {
        fprintf(stderr, "rgb2bw: readrgbimage failed\n");
        exit(1);
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


    if (!lpixelwiseCGA_rgb(r,g,b, sigma))
    {
        fprintf(stderr, " pixelWiseCGA_rgb failed\n");
        exit(0);
    }
    writergbimage(r, g, b, argv[argc - 1]);
    freeimage(r);
    freeimage(g);
    freeimage(b);
    printf("good! function succed\n");
    
    

    return 0;
} /* main */