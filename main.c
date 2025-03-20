#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include "bmp24.h"

void printMenu() {
    printf("\nPlease choose an option:\n");
    printf("1. Open an image\n");
    printf("2. Save an image\n");
    printf("3. Apply a filter\n");
    printf("4. Display image information\n");
    printf("5. Quit\n");
    printf(">>> Your choice: ");
}

void printFilterMenu() {
    printf("\nPlease choose a filter:\n");
    printf("1. Negative\n");
    printf("2. Brightness\n");
    printf("3. Black and white\n");
    printf("4. Box Blur\n");
    printf("5. Gaussian blur\n");
    printf("6. Sharpness\n");
    printf("7. Outline\n");
    printf("8. Emboss\n");
    printf("9. Return to the previous menu\n");
    printf(">>> Your choice: ");
}

int main() {
    t_bmp8 *grayImage = NULL;
    t_bmp24 *colorImage = NULL;
    char filename[256];
    int choice, filterChoice, value;

    while (1) {
        printMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: // Open an image
                printf("File path: ");
                scanf("%255s", filename);

                // Try to open as 8-bit grayscale first
                grayImage = bmp8_loadImage(filename);
                if (grayImage) {
                    printf("8-bit grayscale image loaded successfully!\n");
                } else {
                    // Try to open as 24-bit color
                    colorImage = bmp24_loadImage(filename);
                    if (colorImage) {
                        printf("24-bit color image loaded successfully!\n");
                    } else {
                        printf("Error: Could not load image\n");
                    }
                }
                break;

            case 2: // Save an image
                printf("File path: ");
                scanf("%255s", filename);

                if (grayImage) {
                    bmp8_saveImage(filename, grayImage);
                    printf("8-bit grayscale image saved successfully!\n");
                } else if (colorImage) {
                    bmp24_saveImage(colorImage, filename);
                    printf("24-bit color image saved successfully!\n");
                } else {
                    printf("Error: No image loaded\n");
                }
                break;

            case 3: // Apply a filter
                printFilterMenu();
                scanf("%d", &filterChoice);

                if (grayImage) {
                    switch (filterChoice) {
                        case 1:
                            bmp8_negative(grayImage);
                            break;
                        case 2:
                            printf("Enter brightness value (-255 to 255): ");
                            scanf("%d", &value);
                            bmp8_brightness(grayImage, value);
                            break;
                        case 3:
                            printf("Enter threshold value (0 to 255): ");
                            scanf("%d", &value);
                            bmp8_threshold(grayImage, value);
                            break;
                        case 4:
                            // Box blur kernel
                            {
                                float *kernel[3];
                                float k[3][3] = {
                                    {1.0f/9, 1.0f/9, 1.0f/9},
                                    {1.0f/9, 1.0f/9, 1.0f/9},
                                    {1.0f/9, 1.0f/9, 1.0f/9}
                                };
                                for (int i = 0; i < 3; i++) kernel[i] = k[i];
                                bmp8_applyFilter(grayImage, kernel, 3);
                            }
                            break;
                        case 5:
                            // Gaussian blur kernel
                            {
                                float *kernel[3];
                                float k[3][3] = {
                                    {1.0f/16, 2.0f/16, 1.0f/16},
                                    {2.0f/16, 4.0f/16, 2.0f/16},
                                    {1.0f/16, 2.0f/16, 1.0f/16}
                                };
                                for (int i = 0; i < 3; i++) kernel[i] = k[i];
                                bmp8_applyFilter(grayImage, kernel, 3);
                            }
                            break;
                        case 6:
                            // Sharpen kernel
                            {
                                float *kernel[3];
                                float k[3][3] = {
                                    {0, -1, 0},
                                    {-1, 5, -1},
                                    {0, -1, 0}
                                };
                                for (int i = 0; i < 3; i++) kernel[i] = k[i];
                                bmp8_applyFilter(grayImage, kernel, 3);
                            }
                            break;
                        case 7:
                            // Outline kernel
                            {
                                float *kernel[3];
                                float k[3][3] = {
                                    {-1, -1, -1},
                                    {-1, 8, -1},
                                    {-1, -1, -1}
                                };
                                for (int i = 0; i < 3; i++) kernel[i] = k[i];
                                bmp8_applyFilter(grayImage, kernel, 3);
                            }
                            break;
                        case 8:
                            // Emboss kernel
                            {
                                float *kernel[3];
                                float k[3][3] = {
                                    {-1, 1, 1},
                                    {-2, -1, 0},
                                    {0, 1, 2}
                                };
                                for (int i = 0; i < 3; i++) kernel[i] = k[i];
                                bmp8_applyFilter(grayImage, kernel, 3);
                            }
                            break;
                        case 9:
                            continue;
                        default:
                            printf("Invalid filter choice\n");
                            continue;
                    }
                    printf("Filter applied successfully!\n");
                } else if (colorImage) {
                    switch (filterChoice) {
                        case 1:
                            bmp24_negative(colorImage);
                            break;
                        case 2:
                            printf("Enter brightness value (-255 to 255): ");
                            scanf("%d", &value);
                            bmp24_brightness(colorImage, value);
                            break;
                        case 3:
                            bmp24_grayscale(colorImage);
                            break;
                        case 4:
                            bmp24_boxBlur(colorImage);
                            break;
                        case 5:
                            bmp24_gaussianBlur(colorImage);
                            break;
                        case 6:
                            bmp24_sharpen(colorImage);
                            break;
                        case 7:
                            bmp24_outline(colorImage);
                            break;
                        case 8:
                            bmp24_emboss(colorImage);
                            break;
                        case 9:
                            continue;
                        default:
                            printf("Invalid filter choice\n");
                            continue;
                    }
                    printf("Filter applied successfully!\n");
                } else {
                    printf("Error: No image loaded\n");
                }
                break;

            case 4: // Display image information
                if (grayImage) {
                    bmp8_printInfo(grayImage);
                } else if (colorImage) {
                    printf("Image Info:\n");
                    printf("Width: %d\n", colorImage->width);
                    printf("Height: %d\n", colorImage->height);
                    printf("Color Depth: %d\n", colorImage->colorDepth);
                } else {
                    printf("Error: No image loaded\n");
                }
                break;

            case 5: // Quit
                if (grayImage) bmp8_free(grayImage);
                if (colorImage) bmp24_free(colorImage);
                return 0;

            default:
                printf("Invalid choice\n");
                break;
        }
    }

    return 0;
}