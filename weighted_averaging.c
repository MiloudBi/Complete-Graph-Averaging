
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <mcimage.h>
#include <stdlib.h>


int weight_avr(struct xvimage* image1,int dim_mask) {
	uint32_t i,j,k,l;
	uint32_t sum=0, sum_mask=0;
	uint8_t mask[3][3]= { // for me i choose the 3*3 dim
		{1,1,1},
		{1,1,1},
		{1,1,1}
	};

	dim_mask = 3;

	uint32_t N, M;
	N = image1->row_size;
	M = image1->col_size;
	uint8_t* ptrimage;
	ptrimage = UCHARDATA(image1);
	// sum of kernel
		for (k = 0;k < dim_mask;k++)
			for (l = 0;l < dim_mask;l++)
				sum_mask += mask[k][l];


	for(i= (dim_mask-1)/2; i < N-(dim_mask-1)/2; i++) {
		for (j = (dim_mask - 1)/2;j < M- (dim_mask - 1) / 2; j++) {
			for (k = 0;k <dim_mask;k++) {
				for (l = 0;l < dim_mask;l++) {
					sum += ((uint8_t)mask[k * dim_mask + l]) * ptrimage[(i - ((dim_mask-1)/2) + k) * M + j - ((dim_mask - 1) / 2) + l];
					
				}
			}
			ptrimage[i * M + j] = sum/sum_mask;
		}// fin j
		
	}




	return 1;
}
/* =============================================================== */
int main(int argc, char** argv)
/* =============================================================== */
{
	struct xvimage* image;


	if (argc != 3)
	{
		fprintf(stderr, "usage: %s in1.pgm image2.pgm out.pgm \n", argv[0]);
		exit(0);
	}

	image = readimage(argv[1]);
	if (image == NULL)
	{
		fprintf(stderr, "normalize: readimage failed\n");
		exit(0);
	}
	char* p;
	int dim_mask; // dim of mask is dim_mask*dim_mask

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
		dim_mask = conv;
		printf("mask dim  value is %d\n", dim_mask);
	}


	if (!weight_avr(image,dim_mask))
	{
		fprintf(stderr, "normalize: function normalize failed\n");
		exit(0);
	}
	printf("good! function succed\n");
	writeimage(image, argv[2]);
	freeimage(image);
	freeimage(image);

	return 0;
} /* main */
