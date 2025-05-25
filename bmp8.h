//
// Created by Ryan Bou Jaoude on 20/03/2025.
//

#ifndef BMP8_H
#define BMP8_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char * data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

// Function prototypes
t_bmp8 *bmp8_loadImage(const char *filename);
void bmp8_saveImage(const char *filename, t_bmp8 *img);
void bmp8_free(t_bmp8 *img);
void bmp8_printInfo(t_bmp8 *img);

// Image processing functions
void bmp8_negative(t_bmp8 *img);
void bmp8_brightness(t_bmp8 *img, int value);
void bmp8_threshold(t_bmp8 *img, int threshold);
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

// Histogram equalization functions
unsigned int *bmp8_computeHistogram(t_bmp8 *img);
unsigned int *bmp8_computeCDF(unsigned int *hist);
void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq);
#endif //BMP8_H