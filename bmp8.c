#include <stdio.h>
#include "bmp8.h"
#include <string.h>
#include <stdlib.h>

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!img) {
        fclose(file);
        return NULL;
    }

    // Read header
    if (fread(img->header, sizeof(unsigned char), 54, file) != 54) {
        fprintf(stderr, "Error: Could not read BMP header\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // Read color table
    if (fread(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        fprintf(stderr, "Error: Could not read color table\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // Extract image information from header
    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned int *)&img->header[28];
    img->dataSize = *(unsigned int *)&img->header[34];

    // Check if image is 8-bit grayscale
    if (img->colorDepth != 8) {
        fprintf(stderr, "Error: Image is not 8-bit grayscale\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // Allocate memory for image data
    img->data = (unsigned char *)malloc(img->dataSize);
    if (!img->data) {
        fclose(file);
        free(img);
        return NULL;
    }

    // Read image data
    if (fread(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Error: Could not read image data\n");
        fclose(file);
        free(img->data);
        free(img);
        return NULL;
    }

    fclose(file);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (!img || !filename) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Could not create file %s\n", filename);
        return;
    }

    // Write header
    if (fwrite(img->header, sizeof(unsigned char), 54, file) != 54) {
        fprintf(stderr, "Error: Could not write BMP header\n");
        fclose(file);
        return;
    }

    // Write color table
    if (fwrite(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        fprintf(stderr, "Error: Could not write color table\n");
        fclose(file);
        return;
    }

    // Write image data
    if (fwrite(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Error: Could not write image data\n");
        fclose(file);
        return;
    }

    fclose(file);
}

void bmp8_free(t_bmp8 *img) {
    if (img) {
        free(img->data);
        free(img);
    }
}

void bmp8_printInfo(t_bmp8 *img) {
    if (!img) {
        fprintf(stderr, "Error: Invalid image\n");
        return;
    }

    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
}

void bmp8_negative(t_bmp8 *img) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Invalid image\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}

void bmp8_brightness(t_bmp8 *img, int value) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Invalid image\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        int new_value = img->data[i] + value;
        img->data[i] = (new_value > 255) ? 255 : (new_value < 0) ? 0 : new_value;
    }
}

void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Invalid image\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }
}

void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (!img || !img->data || !kernel || kernelSize % 2 == 0) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return;
    }

    int halfSize = kernelSize / 2;
    unsigned char *tempData = (unsigned char *)malloc(img->dataSize);
    if (!tempData) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }

    memcpy(tempData, img->data, img->dataSize);

    for (int y = halfSize; y < img->height - halfSize; y++) {
        for (int x = halfSize; x < img->width - halfSize; x++) {
            float sum = 0.0f;
            for (int ky = -halfSize; ky <= halfSize; ky++) {
                for (int kx = -halfSize; kx <= halfSize; kx++) {
                    int pixelX = x + kx;
                    int pixelY = y + ky;
                    sum += tempData[pixelY * img->width + pixelX] * 
                           kernel[ky + halfSize][kx + halfSize];
                }
            }
            img->data[y * img->width + x] = (unsigned char)(sum > 255 ? 255 : (sum < 0 ? 0 : sum));
        }
    }

    free(tempData);
} 