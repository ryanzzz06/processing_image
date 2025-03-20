#include "bmp24.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Memory management functions
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));
    if (!pixels) return NULL;

    for (int i = 0; i < height; i++) {
        pixels[i] = (t_pixel *)malloc(width * sizeof(t_pixel));
        if (!pixels[i]) {
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }
    return pixels;
}

void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (pixels) {
        for (int i = 0; i < height; i++) {
            free(pixels[i]);
        }
        free(pixels);
    }
}

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    img->data = bmp24_allocateDataPixels(width, height);

    if (!img->data) {
        free(img);
        return NULL;
    }

    return img;
}

void bmp24_free(t_bmp24 *img) {
    if (img) {
        bmp24_freeDataPixels(img->data, img->height);
        free(img);
    }
}

// File I/O functions
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    if (!image || !file || x < 0 || x >= image->width || y < 0 || y >= image->height) {
        return;
    }

    uint32_t offset = BITMAP_OFFSET + (image->height - 1 - y) * image->width * 3 + x * 3;
    fseek(file, offset, SEEK_SET);

    // Read BGR order
    fread(&image->data[y][x].blue, 1, 1, file);
    fread(&image->data[y][x].green, 1, 1, file);
    fread(&image->data[y][x].red, 1, 1, file);
}

void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    if (!image || !file) return;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}

void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    if (!image || !file || x < 0 || x >= image->width || y < 0 || y >= image->height) {
        return;
    }

    uint32_t offset = BITMAP_OFFSET + (image->height - 1 - y) * image->width * 3 + x * 3;
    fseek(file, offset, SEEK_SET);

    // Write BGR order
    fwrite(&image->data[y][x].blue, 1, 1, file);
    fwrite(&image->data[y][x].green, 1, 1, file);
    fwrite(&image->data[y][x].red, 1, 1, file);
}

void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    if (!image || !file) return;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}

// Main image processing functions
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    // Read header
    t_bmp_header header;
    file_rawRead(BITMAP_MAGIC, &header, sizeof(t_bmp_header), 1, file);

    if (header.type != BMP_TYPE) {
        fprintf(stderr, "Error: Not a BMP file\n");
        fclose(file);
        return NULL;
    }

    // Read info header
    t_bmp_info info;
    file_rawRead(HEADER_SIZE, &info, sizeof(t_bmp_info), 1, file);

    if (info.bits != DEFAULT_DEPTH) {
        fprintf(stderr, "Error: Not a 24-bit image\n");
        fclose(file);
        return NULL;
    }

    // Allocate image structure
    t_bmp24 *img = bmp24_allocate(info.width, info.height, info.bits);
    if (!img) {
        fclose(file);
        return NULL;
    }

    // Copy headers
    img->header = header;
    img->header_info = info;

    // Read pixel data
    bmp24_readPixelData(img, file);

    fclose(file);
    return img;
}

void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    if (!img || !filename) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Could not create file %s\n", filename);
        return;
    }

    // Write headers
    file_rawWrite(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawWrite(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    // Write pixel data
    bmp24_writePixelData(img, file);

    fclose(file);
}

// Image processing functions
void bmp24_negative(t_bmp24 *img) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Invalid image\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 *img) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Invalid image\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            uint8_t gray = (img->data[y][x].red + img->data[y][x].green + img->data[y][x].blue) / 3;
            img->data[y][x].red = gray;
            img->data[y][x].green = gray;
            img->data[y][x].blue = gray;
        }
    }
}

void bmp24_brightness(t_bmp24 *img, int value) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Invalid image\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int new_red = img->data[y][x].red + value;
            int new_green = img->data[y][x].green + value;
            int new_blue = img->data[y][x].blue + value;

            img->data[y][x].red = (new_red > 255) ? 255 : (new_red < 0) ? 0 : new_red;
            img->data[y][x].green = (new_green > 255) ? 255 : (new_green < 0) ? 0 : new_green;
            img->data[y][x].blue = (new_blue > 255) ? 255 : (new_blue < 0) ? 0 : new_blue;
        }
    }
}

t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    t_pixel result = {0, 0, 0};
    if (!img || !img->data || !kernel || kernelSize % 2 == 0) {
        return result;
    }

    int halfSize = kernelSize / 2;
    float sum_red = 0.0f, sum_green = 0.0f, sum_blue = 0.0f;

    for (int ky = -halfSize; ky <= halfSize; ky++) {
        for (int kx = -halfSize; kx <= halfSize; kx++) {
            int pixelX = x + kx;
            int pixelY = y + ky;

            if (pixelX >= 0 && pixelX < img->width && pixelY >= 0 && pixelY < img->height) {
                float weight = kernel[ky + halfSize][kx + halfSize];
                sum_red += img->data[pixelY][pixelX].red * weight;
                sum_green += img->data[pixelY][pixelX].green * weight;
                sum_blue += img->data[pixelY][pixelX].blue * weight;
            }
        }
    }

    result.red = (uint8_t)(sum_red > 255 ? 255 : (sum_red < 0 ? 0 : sum_red));
    result.green = (uint8_t)(sum_green > 255 ? 255 : (sum_green < 0 ? 0 : sum_green));
    result.blue = (uint8_t)(sum_blue > 255 ? 255 : (sum_blue < 0 ? 0 : sum_blue));

    return result;
}

// Filter functions
void bmp24_boxBlur(t_bmp24 *img) {
    float kernel[3][3] = {
        {1.0f/9, 1.0f/9, 1.0f/9},
        {1.0f/9, 1.0f/9, 1.0f/9},
        {1.0f/9, 1.0f/9, 1.0f/9}
    };

    float **kernel_ptr = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel_ptr[i] = kernel[i];
    }

    t_pixel **temp = bmp24_allocateDataPixels(img->width, img->height);
    if (!temp) return;

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            temp[y][x] = bmp24_convolution(img, x, y, kernel_ptr, 3);
        }
    }

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = temp[y][x];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free(kernel_ptr);
}

void bmp24_gaussianBlur(t_bmp24 *img) {
    float kernel[3][3] = {
        {1.0f/16, 2.0f/16, 1.0f/16},
        {2.0f/16, 4.0f/16, 2.0f/16},
        {1.0f/16, 2.0f/16, 1.0f/16}
    };

    float **kernel_ptr = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel_ptr[i] = kernel[i];
    }

    t_pixel **temp = bmp24_allocateDataPixels(img->width, img->height);
    if (!temp) return;

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            temp[y][x] = bmp24_convolution(img, x, y, kernel_ptr, 3);
        }
    }

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = temp[y][x];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free(kernel_ptr);
}

void bmp24_outline(t_bmp24 *img) {
    float kernel[3][3] = {
        {-1, -1, -1},
        {-1, 8, -1},
        {-1, -1, -1}
    };

    float **kernel_ptr = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel_ptr[i] = kernel[i];
    }

    t_pixel **temp = bmp24_allocateDataPixels(img->width, img->height);
    if (!temp) return;

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            temp[y][x] = bmp24_convolution(img, x, y, kernel_ptr, 3);
        }
    }

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = temp[y][x];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free(kernel_ptr);
}

void bmp24_emboss(t_bmp24 *img) {
    float kernel[3][3] = {
        {-1, 1, 1},
        {-2, -1, 0},
        {0, 1, 2}
    };

    float **kernel_ptr = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel_ptr[i] = kernel[i];
    }

    t_pixel **temp = bmp24_allocateDataPixels(img->width, img->height);
    if (!temp) return;

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            temp[y][x] = bmp24_convolution(img, x, y, kernel_ptr, 3);
        }
    }

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = temp[y][x];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free(kernel_ptr);
}

void bmp24_sharpen(t_bmp24 *img) {
    float kernel[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };

    float **kernel_ptr = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel_ptr[i] = kernel[i];
    }

    t_pixel **temp = bmp24_allocateDataPixels(img->width, img->height);
    if (!temp) return;

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            temp[y][x] = bmp24_convolution(img, x, y, kernel_ptr, 3);
        }
    }

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = temp[y][x];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free(kernel_ptr);
}