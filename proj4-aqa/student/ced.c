#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <png.h>
#include "ced.h"
#include "student.h"


/* Local functions */
static void open_images(char *, char *);

/*
	This is a program designed to run Canny Edge Detection on input either color or grayscale
	images. It is an adaptation of the tutorial on canny edge detection for bmp files available
	on https://rosettacode.org/wiki/Canny_edge_detector adapted using the free to use library
	PNG_LIB to process PNG files. I do not claim to have created this code and it is presented
	as an exercise in the power of speeding up code.

	The general format of the code is as follows:

	1. Process the command line args. There are three acceptable option values that can be passed
	   in anywhere among the command line args, -b, -o, or -v.

	  	-b: 
	  		Run a batch input of conversions on many files passsed in. 
	  		This must be the only option passed in (there cannot even
	  		by multiple -b). This is the option that you will be tested
	  		on to determine the speedup and correctness of the program.

	  	-o:
	  		When not in batch mode (and thus run on a single file) this
	  		can be used to indicate the output location of the algorithm.
	  		The user must have write permissions to this file.

	  	-v:
	  		When not in batch mode (and thus run on a single file) this
	  		can be used to display both the original image and the output
	  		image. This occurs by spawning 2 child processes which will
	  		execute xgd-open on each file name. For this reason the current
	  		code is only compatable with linux (which should be no problem
	  		for this project as it is only built for the have machines.)

	 2. Call handle_batch to begin processing the image(s).

	 	This will lead to executing the code in student.c where you should make the
	 	majority of your changes. You are welcome to change anything in this file as
	 	well but the code is setup so that you can complete the project without modifying
	 	this file. The goal here is to abstract away PNG_LIB as the goal of this project
	 	is not to get you familiar with its documentation.

	 3. Peform the actual image processing. This will execute the 4 steps of the algorithm

	 	- Noise reduction via convolution with a gaussian filter

	 	- Compute the intensity gradient of two matrices via a convolution

	 	- Compute the orientation of each pixel's intensity gradient vector specified by
	 	  atan2(G_x, G_y) and use that to set some pixel values to 0.

	 	- Use hysteresis to determine which of the remaining pixels correspond to actual edges

	 	More information on each of these steps can be found student.c

	 4. Handle any display if -v is present.

*/
int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: Pass in an image file to apply Canny Edge Detection to the image. \
By default the image will be saved in canny_[filename given]. \
To change the name of the destination file use the -o flag \
and specify an output name immediately after. Names cannot begin with a \'-\'. \
To have the images open upon completion use the -v flag to \
view both the original and the modified image in xdg-open.\n");
		exit(1);
	}
	bool display = false;
	bool is_batch = false;
	bool is_option = false;
	extern int optind;
	int src_location = 1;
	int c;
	char *optstring;
	char *src;
	char *dst = NULL;
	while ((c = getopt(argc, argv, "bov")) != -1) {
		switch (c) {
			case 'b':
				if (is_option) {
					fprintf(stderr, "Batch option cannot be selected alongside any other options.\n");
					exit(1);
				}
				is_batch = true;
				src_location++;
				break;
			case 'o':
				if (is_batch) {
					fprintf(stderr, "Batch option cannot be selected alongside any other options.\n");
					exit(1);
				}
				dst = argv[optind];
				src_location++;
				if (src_location == optind) {
					src_location++;
				}
				break;
			case 'v':
				if (is_batch) {
					fprintf(stderr, "Batch option cannot be selected alongside any other options.\n");
					exit(1);
				}
				src_location++;
				display = true;
				break;
			default:
				fprintf(stderr, "Bad option selected.\n");
				exit(1);
		}
		is_option = true;
	}
	unsigned length;
	if (is_batch) {
		length = argc - 2;
		if (length == 0) {
			fprintf(stderr, "No files selected.\n");
			exit(1);
		}
	} else {
		length = 1;
	}
	char** src_values = argv + src_location;
	char* dst_values[length];
	char *value = "out/canny_";
	if (dst != NULL) {
		dst_values[0] = dst;
		
	} else {
		char *src;
		for (unsigned i = 0; i < length; i++) {
			src = src_values[i];
			unsigned folder_length = strlen(src);
			unsigned total_length = folder_length;
			while (folder_length > 0) {
				if (src[folder_length - 1] == '/') { //Find the break for end of the file path;
					break;
				}
				folder_length -= 1;
			}
			dst_values[i] = malloc(sizeof(char) * (11 + total_length));
			strcpy(dst_values[i], value);
			strcpy(dst_values[i] + 10, src + folder_length);
		}
	}
	handle_batch(src_values, dst_values, length);
	if (display) {
		open_images(src_values[0], dst_values[0]);
	}
	if (dst == NULL) {
		free(dst_values[0]);
	} else {
		for (unsigned i = 0; i < length; i++) {
			free(dst_values[i]);
		}
	}
}

/*
	Performs the preliminary steps necessary to perform a read using PNG_LIB. In particular
	it sets up the read struct, the information struct, and the end struct for peforming
	the read. It also uses setjump to create a error destination if there is an error in
	the read.
*/

void setup_read(FILE *src_file, FILE *dst_file, png_structp *png_read_ptr, png_infop *read_info_ptr, png_infop *read_end_ptr) {
	char header[8];
	int val = fread(header, 1, 8, src_file);
	if (png_sig_cmp(header, 0, val)) {
		fprintf(stderr, "File is not a png file.\n");
		fclose(src_file);
		fclose(dst_file);
		exit(1);
	}
	*(png_read_ptr) = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_read_ptr == NULL) {
		fprintf(stderr, "Failed to allocate space for the png file.\n");
		fclose(src_file);
		fclose(dst_file);
		exit(1);
	}
	*(read_info_ptr) = png_create_info_struct(*png_read_ptr);
	if (read_info_ptr == NULL) {
		png_destroy_read_struct(png_read_ptr, NULL, NULL);
		fprintf(stderr, "Failed to allocate space for the png file information.\n");
		fclose(src_file);
		fclose(dst_file);
		exit(1);
	}
	*(read_end_ptr) = png_create_info_struct(*png_read_ptr);
	if (read_end_ptr == NULL) {
		png_destroy_read_struct(png_read_ptr, read_info_ptr, NULL);
		fprintf(stderr, "Failed to allocate space for the png file end information.\n");
		fclose(src_file);
		fclose(dst_file);
		exit(1);
	}
	if (setjmp(png_jmpbuf(*png_read_ptr))) {
		png_destroy_read_struct(png_read_ptr, read_info_ptr, read_end_ptr);
		fprintf(stderr, "Error encountered while reading the png file.\n");
		fclose(src_file);
		fclose(dst_file);
		exit(1);
	}
	png_init_io(*png_read_ptr, src_file);
	png_set_sig_bytes(*png_read_ptr, val);
}


/*
	Place the read informaton into the read information struct and also converts the
	image to grayscale if it is not already. This is necessary because the algorithm
	only functions on grayscale images.
*/
void setup_info(png_structp png_read_ptr, png_infop read_info_ptr) {
	png_read_info(png_read_ptr, read_info_ptr);
    if (png_get_color_type(png_read_ptr, read_info_ptr) == PNG_COLOR_TYPE_RGB || png_get_color_type(png_read_ptr, read_info_ptr) == PNG_COLOR_TYPE_RGB == PNG_COLOR_TYPE_RGB_ALPHA) {
        png_set_rgb_to_gray_fixed(png_read_ptr, 1, 21268, 71514);
    }
}


/*
	Performs the actual read and places in the data in the rows presented. Notice that the api requires
	row pointers to be of type png_bytep* and have HEIGHT rows. As a result you will need to present a 2D
	array here though you are free to allocate it how you please.
*/
void execute_read(png_structp png_read_ptr, png_infop read_info_ptr, png_infop read_end_ptr, png_bytep* row_pointers) {
	png_set_rows(png_read_ptr, read_info_ptr, row_pointers);
	png_read_image(png_read_ptr, row_pointers);
	png_read_end(png_read_ptr, read_end_ptr);
}


/*
	Performs the preliminary steps necessary to perform a write using PNG_LIB. In particular
	it sets up the write struct and the information struct for peforming
	the write. It also uses setjump to create a error destination if there is an error in
	the write.
*/
void setup_write(FILE *src_file, FILE *dst_file, png_structp png_read_ptr, png_infop read_info_ptr, png_infop read_end_ptr, png_structp *png_write_ptr, png_infop *write_info_ptr) {
	*(png_write_ptr) = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (*png_write_ptr == NULL) {
		png_destroy_read_struct(&png_read_ptr, &read_info_ptr, &read_end_ptr);
		fprintf(stderr, "Failed to allocate space for writing struct.\n");
		fclose(src_file);
		fclose(dst_file);
		exit(1);
	}
	*(write_info_ptr) = png_create_info_struct(*png_write_ptr);
	if (*write_info_ptr == NULL) {
		png_destroy_write_struct(png_write_ptr, NULL);
		png_destroy_read_struct(&png_read_ptr, &read_info_ptr, &read_end_ptr);
		fprintf(stderr, "Failed to allocate space for writing struct.\n");
		fclose(src_file);
		fclose(dst_file);
		exit(1);
	}
	if (setjmp(png_jmpbuf(*png_write_ptr))) {
		png_destroy_write_struct(png_write_ptr, write_info_ptr);
		png_destroy_read_struct(&png_read_ptr, &read_info_ptr, &read_end_ptr);
		fprintf(stderr, "Error encountered while writing the png file.\n");
		fclose(src_file);
		fclose(dst_file);
		exit(1);
	}
	png_set_IHDR(*png_write_ptr, *write_info_ptr, png_get_image_width(png_read_ptr, read_info_ptr), png_get_image_height(png_read_ptr, read_info_ptr), png_get_bit_depth(png_read_ptr, read_info_ptr), PNG_COLOR_TYPE_GRAY, png_get_interlace_type(png_read_ptr, read_info_ptr), png_get_compression_type(png_read_ptr, read_info_ptr), png_get_filter_type(png_read_ptr, read_info_ptr));
	png_init_io(*png_write_ptr, dst_file);
}


/*
	Performs the actual write from the data in the rows presented. Notice that the api requires
	row pointers to be of type png_bytep* and have HEIGHT rows. As a result you will need to present a 2D
	array here though you are free to allocate it how you please.
*/
void execute_write(png_structp png_write_ptr, png_infop write_info_ptr, png_bytep *final_output) {
	png_set_rows(png_write_ptr, write_info_ptr, final_output);
	png_write_png(png_write_ptr, write_info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
}


/*
	Frees the memory in the structs allocated using the PNG_LIB functions. There probably
	isn't much you can change here.
*/
void cleanup_struct_mem(png_structp png_read_ptr, png_infop read_info_ptr, png_infop read_end_ptr, png_structp png_write_ptr, png_infop write_info_ptr) {
	png_destroy_write_struct(&png_write_ptr, &write_info_ptr);
	png_destroy_read_struct(&png_read_ptr, &read_info_ptr, &read_end_ptr);
}

/*
	Opens the pngs using xdg-open. Note that this makes viewing only compatable
	with a linux machine. This is unused in the graded portion of the project.
*/
static void open_images(char* png_1, char* png_2) {
	pid_t pid;
	pid = fork();
	char* args[3];
	char* path = "/usr/bin/xdg-open";
	args[0] = path;
	args[1] = png_1;
	args[2] = NULL;	
	if (pid == 0) {
		execv(path, args);
	} else if (pid > 0) {
		pid = fork();
		if (pid == 0) {
			args[1] = png_2;
			execv(path, args);
		} else if (pid > 0) {
			wait(NULL);
			wait(NULL);
		} else {
			exit(1);
		}
	} else {
		exit(1);
	}
}
