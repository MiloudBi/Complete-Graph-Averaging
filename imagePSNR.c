
#include <stdint.h>
#include <stdio.h>
#include <windows.h> 
#include <string.h>
#include <mcimage.h>
#include <stdlib.h>
#include<math.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX

#define MAX (255*255)

/* ==================================== */
uint32_t imagePSNR(struct xvimage* image1, struct xvimage* image2) {

    
    int  J, I, j, i;
    uint8_t* ptrimage1,*ptrimage2; 
    uint32_t rs1, cs1, N1, rs2, cs2, N2, Im;

    rs1 = image1->row_size;
    cs1 = image1->col_size;
    N1 = rs1 * cs1;
    rs2 = image1->row_size;
    cs2 = image1->col_size;
    N2 = rs2 * cs2;
    float MSE,sum=0,error, psnr;

    ptrimage1 = UCHARDATA(image1);
    ptrimage2= UCHARDATA(image2);
    if (N1 != N2) {
        printf("two image hasn't some dim we can't calculte the PSNR");
        exit(EXIT_SUCCESS);
    }
    //Calculate MSE
    for (i = 0;i < N1;i++)
    {
        error = pow(ptrimage1[i] - ptrimage2[i],2);
        sum += error;
    }
    MSE = sum / N1;

    //Avoid division by zero
    if (MSE == 0) return 99.0;

    //The PSNR
    psnr = 10 * log10(MAX / MSE);

    //Thats it.
    return psnr;
   


   

}



/* =============================================================== */
int main(int argc, char** argv)
/* =============================================================== */
{
    struct xvimage* image1;
    struct xvimage* image2;
    float psnr;


    if (argc != 3)
    {
        fprintf(stderr, "usage: %s in1.pgm sigma out.pgm \n", argv[0]);
        exit(0);
    }


    image1 = readimage(argv[1]);
    if (image1 == NULL)
    {
        fprintf(stderr, "PixelWise: readimage failed\n");
        exit(0);
    }
    image2 = readimage(argv[2]);
    if (image2 == NULL)
    {
        fprintf(stderr, "PixelWise: readimage failed\n");
        exit(0);
    }
    psnr = imagePSNR(image1, image2);
    printf("the value of Peak signal-to-noise ratio is %f \n", psnr);
    printf("good! function succed\n");
    
    freeimage(image1);
    freeimage(image2);

    return 0;
} /* main */