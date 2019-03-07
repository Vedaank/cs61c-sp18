#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <png.h>
#include <x86intrin.h>
#include <omp.h>
#include "ced.h"
#include "student.h"

void canny_edge_detection(char* src, char* dst) {
	char header[8];
	png_structp png_read_ptr;
	png_infop read_info_ptr;
	png_infop read_end_ptr;
	png_structp png_write_ptr;
	png_infop write_info_ptr;

	//Open the source and destination file
	FILE *src_file = fopen(src, "rb");
	if (src_file == NULL) {
		fprintf(stderr, "Unable to open source file.\n");
		exit(1);
	}
	FILE *dst_file = fopen(dst, "wb");
	if (dst_file == NULL) {
		fprintf(stderr, "Unable to create destination file.\n");
		fclose(src_file);
		exit(1);
	}

	//Call library function to set up the information for reading
	setup_read(src_file, dst_file, &png_read_ptr, &read_info_ptr, &read_end_ptr);

	//Determines image features such as height and width
	setup_info(png_read_ptr, read_info_ptr);


	//Allocate memory to read the image data into
	png_bytep row_pointers[png_get_image_height(png_read_ptr, read_info_ptr)];
	allocate_read_mem(png_read_ptr, row_pointers, png_get_image_height(png_read_ptr, read_info_ptr), png_get_rowbytes(png_read_ptr, read_info_ptr));

	//Execute the actual read
	execute_read(png_read_ptr, read_info_ptr, read_end_ptr, row_pointers);

	//Call library function to set up the information for writing
	setup_write(src_file, dst_file, png_read_ptr, read_info_ptr, read_end_ptr, &png_write_ptr, &write_info_ptr);   


	//Allocate memory to perform for the various steps of the algorithm
	png_bytep output_pointers[png_get_image_height(png_read_ptr, read_info_ptr)];
	png_bytep Gy_applied[png_get_image_height(png_read_ptr, read_info_ptr)];
	png_bytep Gx_applied[png_get_image_height(png_read_ptr, read_info_ptr)];
	png_bytep nms[png_get_image_height(png_read_ptr, read_info_ptr)];
	png_bytep final_output[png_get_image_height(png_read_ptr, read_info_ptr)];

	//Allocate the actual memory
	allocate_write_mem(png_write_ptr, output_pointers, Gy_applied, Gx_applied, nms, final_output, png_get_image_height(png_read_ptr, read_info_ptr), png_get_rowbytes(png_read_ptr, read_info_ptr));

	//Allocate enough space for intermediate arrays
	float *G = calloc(png_get_rowbytes(png_read_ptr, read_info_ptr) * png_get_image_height(png_read_ptr, read_info_ptr), sizeof(float));
	float *dir = calloc(png_get_rowbytes(png_read_ptr, read_info_ptr) * png_get_image_height(png_read_ptr, read_info_ptr), sizeof(float));


	//The four steps for the canny edge detection.
	gaussian_filter(row_pointers, output_pointers, png_get_rowbytes(png_read_ptr, read_info_ptr), png_get_image_height(png_read_ptr, read_info_ptr), 1.0);
	intensity_gradients(output_pointers, Gx_applied, Gy_applied, G, dir, png_get_rowbytes(png_read_ptr, read_info_ptr), png_get_image_height(png_read_ptr, read_info_ptr));
	non_maximum_suppression(nms, G, dir, png_get_rowbytes(png_read_ptr, read_info_ptr), png_get_image_height(png_read_ptr, read_info_ptr));
	hysteresis(final_output, nms, png_get_rowbytes(png_read_ptr, read_info_ptr), png_get_image_height(png_read_ptr, read_info_ptr), 105, 45);
	
	free(G);
	free(dir);
	
	//Complete the actual write
	execute_write(png_write_ptr, write_info_ptr, final_output);


	//Clear memory allocated for reading and writing
	cleanup_rows(png_write_ptr, output_pointers, Gy_applied, Gx_applied, nms, final_output, png_read_ptr, row_pointers, png_get_image_height(png_read_ptr, read_info_ptr));



	//Clear memory alloacted by the library
	cleanup_struct_mem(png_read_ptr, read_info_ptr, read_end_ptr, png_write_ptr, write_info_ptr);


	//Close out the files
	fclose(src_file);
	fclose(dst_file);
}


/*
    Compute a gaussian filter and then perform a convolution of it with the input pixels read from the file (which
    have previously been set to be grayscale.)

    Refer to wikipedia for how to generate the gaussian filter if you need to check any changes.

    https://en.wikipedia.org/wiki/Canny_edge_detector

    C comments can't do the formula format justice
*/
void gaussian_filter(png_bytep *input, png_bytep *output, const unsigned width, const unsigned height, const float sigma) {
	unsigned n;
	if (sigma < 0.5) {
		n = 3;
	} else if (sigma < 1.0) {
		n = 5;
	} else if (sigma < 1.5) {
		n = 7;
	} else if (sigma < 2.0) {
		n = 9;
	} else if (sigma < 2.5) {
		n = 11;
	} else {
		n = 13;
	}
	const float k = (n - 1) / 2.0;
	float kernel[n * n];
    #pragma omp parallel for collapse (2)
        for (unsigned j = 0; j < n; j++) {
            for (unsigned i = 0; i < n; i++) {
                kernel[j + i*n] = exp(-0.5 * ((pow((i - (k + 1)), 2.0) + pow((j - (k + 1)), 2.0)) / (sigma * sigma))) / (2 * M_PI * sigma * sigma);
            }
        }
	convolution(input, output, kernel, width, height, n, true);
}


/*
    Performs a convolution of the input and a specified kernel.
    If you are curious about what a convolution is, look at 
    https://en.wikipedia.org/wiki/Convolution
    but for our purposes we can think of it as a transformation
    on the input using the kernel.
*/
void convolution(png_bytep *input, png_bytep *output, float *kernel, const unsigned width, const unsigned height, const int z, const bool normalize) {

	const int half = z / 2;
	float min = FLT_MAX, max = -FLT_MAX;
    int rows = width - half;
    int columns = height - half;
    float **pixels;
    pixels = (float **) malloc(rows * sizeof(float *));
    for(int i = 0; i < rows; i++){
        pixels[i] = (float *) malloc(columns * sizeof(float));
    }

    if (normalize) {
        for (int m = half; m < width - half; m++) {
            for (int n = half; n < height - half; n++) {
                float pixel = 0.0;
                size_t c = 0;
                for (int i = -half; i <= half; i++) {
                    for (int j = -half; j <= half; j++) {
                            pixel += input[n - j][m - i] * kernel[c];
                            c++;
                    }
                }
                pixels[m - half][n - half] = pixel;
                if (pixel < min) {
                    min = pixel;
                }
                if (pixel > max) {
                    max = pixel;
                }
            }
        }
  	}
    #pragma omp parallell for collapse (2)
        for (int m = half; m < width - half; m++) {
            for (int n = half; n < height - half; n++) {
                float pixel = 0.0;
                size_t c = 0;
                if (normalize) {
                    pixel = pixels[m - half][n - half];
                    pixel = MAX_BRIGHTNESS * (pixel - min) / (max - min);
                } else {
                    for (int i = -half; i <= half; i++) {
                        for (int j = -half; j <= half; j++) {
                            pixel += input[n - j][m - i] * kernel[c];
                            c++;
                        }
                    }
                }
                output[n][m] = (png_byte) pixel;
            }
        }
}

/*
    Takes two known matrices and performs convolutions on them with the output of the previous
    step (the input to this function). The Gradient G is calculated using the two convolutions
    and the angles can be calculated using the arctan of the two convultion results.
*/
void intensity_gradients(png_bytep *output, png_bytep *Gx_applied, png_bytep *Gy_applied, float *G, float *dir, const unsigned width, const unsigned height) {
	float Gx[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
	float Gy[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
	convolution(output, Gx_applied, Gx, width, height, 3, false);
	convolution(output, Gy_applied, Gx, width, height, 3, false);
    #pragma omp parallel for collapse (2)
        for (int i = 1; i < width - 1; i++) {
            for (int j = 1; j < height - 1; j++) {
                int c = i + width * j;
                G[c] = hypot(Gx_applied[j][i], Gy_applied[j][i]);
                dir[c] = (float)(fmod(atan2(Gy_applied[j][i], Gx_applied[j][i]) + M_PI, M_PI) / M_PI) * 8;
            }
        }
}


/*
    Takes the input G which consists of the gradient values and using the direction to determine
    the direction of the gradient. Then checks if in the direction of the gradient (given by
    dir) it is a local maximum. If it is the value remains on, otherwise it is turned off.
*/
void non_maximum_suppression(png_bytep *nms, float *G, float *dir, const unsigned width, const unsigned height) {
	#pragma omp parallel for collapse (2)
        for (int i = 1; i < width - 1; i++) {
            for (int j = 1; j < height - 1; j++) {
                int c = i + width * j;
                int nn = c - width;
                int ss = c + width;
                int ww = c + 1;
                int ee = c - 1;
                int nw = nn + 1;
                int ne = nn - 1;
                int sw = ss + 1;
                int se = ss - 1;
                if ((dir[c] <= 1 || dir[c] > 7) && G[c] > G[ee] && G[c] > G[ww]) {
                    nms[j][i] = G[c];
                } else if ((dir[c] > 1 && dir[c] <= 3) && G[c] > G[nw] && G[c] > G[se]) {
                    nms[j][i] = G[c];
                } else if ((dir[c] > 3 && dir[c] <= 5) && G[c] > G[nn] && G[c] > G[ss]) {
                    nms[j][i] = G[c];
                } else if ((dir[c] > 5 && dir[c] <= 7) && G[c] > G[ne] && G[c] > G[sw]) {
                    nms[j][i] = G[c];
                } else {
                    nms[j][i] = 0;
                }
            }
        }
}

/*
     Takes the pixel values in nms and determines if the values are greater than tmax or tmin.
     If the value is greater than tmax then the brightness of the pixel is set to be maximal.
     If the value is greater than min then it will be turned on if any of its neighbors have
     been set to be edges. The output results are written to out.
*/
void hysteresis(png_bytep *out, png_bytep *nms, const unsigned width, const unsigned height, const unsigned tmax, const unsigned tmin) {
    int *edges = calloc(sizeof(int), width * height);
    #pragma omp parallel for collapse (2)
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                out[j][i] = 0;
            }
        }

    for (int j = 1; j < height - 1; j++) {
        for (int i = 1; i < width - 1; i++) {
            int c = i + width * j;
            if (nms[j][i] >= tmax && out[j][i] == 0) {
                out[j][i] = MAX_BRIGHTNESS;
                int nedges = 1;
                edges[0] = c;
                do {
                    nedges--;
                    int t = edges[nedges];

                    int nbs[8]; // neighbors
                    nbs[0] = t - width;     // nn
                    nbs[1] = t + width;     // ss
                    nbs[2] = t + 1;         // ww
                    nbs[3] = t - 1;         // ee
                    nbs[4] = nbs[0] + 1;    // nw
                    nbs[5] = nbs[0] - 1;    // ne
                    nbs[6] = nbs[1] + 1;    // sw
                    nbs[7] = nbs[1] - 1;    // se

                    for (int k = 0; k < 8; k++) {
                        if (nms[nbs[k] / width][nbs[k] % width] >= tmin &&
                            out[nbs[k] / width][nbs[k] % width] == 0) {
                            out[nbs[k] / width][nbs[k] % width] = MAX_BRIGHTNESS;
                            edges[nedges] = nbs[k];
                            nedges++;
                        }
                    }
                } while (nedges > 0);
            }
        }
    }
    free(edges);
}


/*
    Allocates the memory necessary to read from the png file. The png_ functions
    all interact with the PNG_LIB library and should be used as opposed
    to the traditional malloc. Note that a png_bytep * is used because to set the
    rows to contain the data a value of type png_bytep * must be presented.
    Also note a png_bytep is an unsigned char *
*/
void allocate_read_mem(png_structp png_read_ptr, png_bytep *row_pointers, unsigned height, unsigned width) {
    #pragma omp parallel for
        for (unsigned row = 0; row < height; row++) {
            row_pointers[row] = NULL;
        }
    for (unsigned row = 0; row < height; row++) {
        row_pointers[row] = png_malloc(png_read_ptr, width);
        for (int j = 0; j < width; j++) {
            row_pointers[row][j] = 0;
        }
    }
}


/*
    Allocates the memory necessary to perform all of the steps of the edge detection algorithm.
    The png_ functions all interact with the PNG_LIB library and should be used as opposed to 
    the traditional malloc. Note that a png_bytep * is used because on must be final type for 
    rows presented to be written must be a png_bytep *. Also note a png_bytep is an unsigned char *
*/
void allocate_write_mem(png_structp png_write_ptr, png_bytep *rows1, png_bytep *rows2, png_bytep *rows3, png_bytep *rows4, png_bytep *rows5, unsigned height, unsigned width) {
    #pragma omp parallel for
        for (unsigned row = 0; row < height; row++) {
            rows1[row] = NULL;
            rows2[row] = NULL;
            rows3[row] = NULL;
            rows4[row] = NULL;
            rows5[row] = NULL;
        }
    for (unsigned row = 0; row < height; row++) {
        rows1[row] = png_malloc(png_write_ptr, width);
        rows2[row] = png_malloc(png_write_ptr, width);
        rows3[row] = png_malloc(png_write_ptr, width);
        rows4[row] = png_malloc(png_write_ptr, width);
        rows5[row] = png_malloc(png_write_ptr, width);
        for (int j = 0; j < width; j++) {
            rows1[row][j] = 0;
            rows2[row][j] = 0;
            rows3[row][j] = 0;
            rows4[row][j] = 0;
            rows5[row][j] = 0;
        }
    }
}

/*
    Frees all of the memory allocated to hold the png information. The png_
    functions are all functions that interact with the PNG_LIB library and should
    be used as opposed to the traditional free.
*/
void cleanup_rows(png_structp png_write_ptr, png_bytep *rows1, png_bytep *rows2, png_bytep *rows3, png_bytep *rows4, png_bytep *rows5, png_structp png_read_ptr, png_bytep *readrows, unsigned height) {
    #pragma omp parallel for
        for (unsigned row = 0; row < height; row++) {
            png_free(png_write_ptr, rows1[row]);
            png_free(png_write_ptr, rows2[row]);
            png_free(png_write_ptr, rows3[row]);
            png_free(png_write_ptr, rows4[row]);
            png_free(png_write_ptr, rows5[row]);
            png_free(png_read_ptr, readrows[row]);
        }
}


/*
    Function responsible for initiating the edge detection program on 1 or more png images.
    This function is the first location in which processing begins.
*/
void handle_batch(char **src_values, char **dst_values, unsigned count) {
    #pragma omp parallel for
        for (int i = 0; i < count; i++) {
            canny_edge_detection(src_values[i], dst_values[i]);
        }
}
