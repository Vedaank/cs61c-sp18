#include <stdlib.h>
#include <stdbool.h>
#include <png.h>

bool is_correct (char *ref, char *naive, char *student, int picnum);
void setup_read(FILE* file, png_structp *png_read_ptr, png_infop *read_info_ptr, png_infop *read_end_ptr);
void setup_info(png_structp png_read_ptr, png_infop read_info_ptr);
void allocate_read_mem(png_structp png_read_ptr, png_bytep *row_pointers, unsigned height, unsigned width);
void execute_read(png_structp png_read_ptr, png_infop read_info_ptr, png_infop read_end_ptr, png_bytep* row_pointers);
void cleanup_rows(png_structp png_read_ptr, png_bytep *readrows, unsigned height);
void cleanup_struct_mem(png_structp png_read_ptr, png_infop read_info_ptr, png_infop read_end_ptr);



char *refs[30] = {"ref/ref_canny_baboon.png", "ref/ref_canny_balloons.png", "ref/ref_canny_bigbrain.png", "ref/ref_canny_bird.png", "ref/ref_canny_bottle.png", "ref/ref_canny_bowtie.png", "ref/ref_canny_chair.png", "ref/ref_canny_c.png", "ref/ref_canny_darkknight.png", "ref/ref_canny_darknight.png", "ref/ref_canny_dollar.png", "ref/ref_canny_flag.png", "ref/ref_canny_guitar.png", "ref/ref_canny_house.png", "ref/ref_canny_knight.png", "ref/ref_canny_night.png", "ref/ref_canny_ocean.png", "ref/ref_canny_oski.png", "ref/ref_canny_playground.png", "ref/ref_canny_rainbow.png", "ref/ref_canny_silver.png", "ref/ref_canny_smile.png", "ref/ref_canny_snorlax.png", "ref/ref_canny_square.png", "ref/ref_canny_stool.png", "ref/ref_canny_sword.png", "ref/ref_canny_tree.png", "ref/ref_canny_valve.png", "ref/ref_canny_wallet.png", "ref/ref_canny_weaver.png"};
char *naive[30] = {"naive/out/canny_baboon.png" ,"naive/out/canny_balloons.png" ,"naive/out/canny_bigbrain.png" ,"naive/out/canny_bird.png" ,"naive/out/canny_bottle.png" ,"naive/out/canny_bowtie.png" ,"naive/out/canny_chair.png" ,"naive/out/canny_c.png" ,"naive/out/canny_darkknight.png" ,"naive/out/canny_darknight.png" ,"naive/out/canny_dollar.png" ,"naive/out/canny_flag.png" ,"naive/out/canny_guitar.png" ,"naive/out/canny_house.png" ,"naive/out/canny_knight.png" ,"naive/out/canny_night.png" ,"naive/out/canny_ocean.png" ,"naive/out/canny_oski.png" ,"naive/out/canny_playground.png" ,"naive/out/canny_rainbow.png" ,"naive/out/canny_silver.png" ,"naive/out/canny_smile.png" ,"naive/out/canny_snorlax.png" ,"naive/out/canny_square.png" ,"naive/out/canny_stool.png" ,"naive/out/canny_sword.png" ,"naive/out/canny_tree.png" ,"naive/out/canny_valve.png" ,"naive/out/canny_wallet.png" ,"naive/out/canny_weaver.png"};
char *student[30] = {"student/out/canny_baboon.png" ,"student/out/canny_balloons.png" ,"student/out/canny_bigbrain.png" ,"student/out/canny_bird.png" ,"student/out/canny_bottle.png" ,"student/out/canny_bowtie.png" ,"student/out/canny_chair.png" ,"student/out/canny_c.png" ,"student/out/canny_darkknight.png" ,"student/out/canny_darknight.png" ,"student/out/canny_dollar.png" ,"student/out/canny_flag.png" ,"student/out/canny_guitar.png" ,"student/out/canny_house.png" ,"student/out/canny_knight.png" ,"student/out/canny_night.png" ,"student/out/canny_ocean.png" ,"student/out/canny_oski.png" ,"student/out/canny_playground.png" ,"student/out/canny_rainbow.png" ,"student/out/canny_silver.png" ,"student/out/canny_smile.png" ,"student/out/canny_snorlax.png" ,"student/out/canny_square.png" ,"student/out/canny_stool.png" ,"student/out/canny_sword.png" ,"student/out/canny_tree.png" ,"student/out/canny_valve.png" ,"student/out/canny_wallet.png" ,"student/out/canny_weaver.png"};

int main(int argc, char*argv[]) {
	bool valid = true;
	for (int i = 0; i < 30; i++) {
		valid = is_correct(refs[i], naive[i], student[i], i + 1) && valid;
	}
	if (!valid) {
		printf("Correctness Check Failed.\n");
		exit(1);
	} else {
		printf("Correctness Check Successful\n");
		exit(0);
	}
}


bool is_correct (char *ref, char *naive, char *student, int picnum) {
	unsigned naive_total = 0;
	unsigned student_total = 0;
	unsigned nonblacks_total = 0;
	unsigned nonblackn_total = 0;
	FILE *ref_file = fopen(ref, "rb");
	if (ref_file == NULL) {
		fprintf(stderr, "Correctness Check Failed. Ref file %s not found\n", ref);
		exit(1);
	}
	FILE *naive_file = fopen(naive, "rb");
	if (naive_file == NULL) {
		fprintf(stderr, "Correctness Check Failed. Naive file %s not found\n", naive);
		fclose(ref_file);
		exit(1);
	}
	FILE *student_file = fopen(student, "rb");
	if (student_file == NULL) {
		fprintf(stderr, "Correctness Check Failed. Student file %s not found\n", student);
		fclose(ref_file);
		fclose(naive_file);
		exit(1);
	}
	png_structp read_ref_ptr;
	png_infop info_ref_ptr;
	png_infop end_ref_ptr;
	png_structp read_naive_ptr;
	png_infop info_naive_ptr;
	png_infop end_naive_ptr;
	png_structp read_student_ptr;
	png_infop info_student_ptr;
	png_infop end_student_ptr;
	setup_read(ref_file, &read_ref_ptr, &info_ref_ptr, &end_ref_ptr);
	setup_read(naive_file, &read_naive_ptr, &info_naive_ptr, &end_naive_ptr);
	setup_read(student_file, &read_student_ptr, &info_student_ptr, &end_student_ptr);
	setup_info(read_ref_ptr, info_ref_ptr);
	setup_info(read_naive_ptr, info_naive_ptr);
	setup_info(read_student_ptr, info_student_ptr);

	png_bytep ref_pointers[png_get_image_height(read_ref_ptr, info_ref_ptr)];
	allocate_read_mem(read_ref_ptr, ref_pointers, png_get_image_height(read_ref_ptr, info_ref_ptr), png_get_rowbytes(read_ref_ptr, info_ref_ptr));

	png_bytep naive_pointers[png_get_image_height(read_naive_ptr, info_naive_ptr)];
	allocate_read_mem(read_naive_ptr, naive_pointers, png_get_image_height(read_naive_ptr, info_naive_ptr), png_get_rowbytes(read_naive_ptr, info_naive_ptr));

	png_bytep student_pointers[png_get_image_height(read_student_ptr, info_student_ptr)];
	allocate_read_mem(read_student_ptr, student_pointers, png_get_image_height(read_student_ptr, info_student_ptr), png_get_rowbytes(read_student_ptr, info_student_ptr));
	execute_read(read_ref_ptr, info_ref_ptr, end_ref_ptr, ref_pointers);
	execute_read(read_naive_ptr, info_naive_ptr, end_naive_ptr, naive_pointers);
	execute_read(read_student_ptr, info_student_ptr, end_student_ptr, student_pointers);
	for (int i = 0; i < png_get_image_height(read_ref_ptr, info_ref_ptr); i++) {
		for (int j = 0; j < png_get_rowbytes(read_ref_ptr, info_ref_ptr); j++) {
			if ((!ref_pointers[i][j]) != (!naive_pointers[i][j])) {
				naive_total++;
			}
			if ((!ref_pointers[i][j]) != (!student_pointers[i][j])) {
				student_total++;
			}
			if (student_pointers[i][j] != 0) {
				nonblacks_total++;
			}
			if (naive_pointers[i][j] != 0) {
				nonblackn_total++;
			}
		}
	}


	cleanup_rows(read_ref_ptr, ref_pointers, png_get_image_height(read_ref_ptr, info_ref_ptr));
	cleanup_rows(read_naive_ptr, naive_pointers, png_get_image_height(read_naive_ptr, info_naive_ptr));
	cleanup_rows(read_student_ptr, student_pointers, png_get_image_height(read_student_ptr, info_student_ptr));

	cleanup_struct_mem(read_ref_ptr, info_ref_ptr, end_ref_ptr);
	cleanup_struct_mem(read_naive_ptr, info_naive_ptr, end_naive_ptr);
	cleanup_struct_mem(read_student_ptr, info_student_ptr, end_student_ptr);

	fclose(ref_file);
	fclose(naive_file);
	fclose(student_file);
	
	if (nonblacks_total < (nonblackn_total * 2) / 3) {
		fprintf(stderr, "Failed [%i]: %s. Image is just black! (# non-black pixels Student: %i Naive %i)\n", picnum, ref, nonblacks_total, nonblackn_total);
		return false;
	}
	if (student_total <= (naive_total * 105) / 100) {
		return true;
	}
	fprintf(stderr, "Failed [%i]: %s\n", picnum, ref);
	return false;
}

void setup_read(FILE* file, png_structp *png_read_ptr, png_infop *read_info_ptr, png_infop *read_end_ptr) {
	char header[8];
	int val = fread(header, 1, 8, file);
	if (png_sig_cmp(header, 0, val)) {
		fprintf(stderr, "File is not a png file.\n");
		exit(1);
	}
	*(png_read_ptr) = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_read_ptr == NULL) {
		fprintf(stderr, "Failed to allocate space for the png file.\n");
		exit(1);
	}
	*(read_info_ptr) = png_create_info_struct(*png_read_ptr);
	if (read_info_ptr == NULL) {
		png_destroy_read_struct(png_read_ptr, NULL, NULL);
		fprintf(stderr, "Failed to allocate space for the png file information.\n");
		exit(1);
	}
	*(read_end_ptr) = png_create_info_struct(*png_read_ptr);
	if (read_end_ptr == NULL) {
		png_destroy_read_struct(png_read_ptr, read_info_ptr, NULL);
		fprintf(stderr, "Failed to allocate space for the png file end information.\n");
		exit(1);
	}
	if (setjmp(png_jmpbuf(*png_read_ptr))) {
		png_destroy_read_struct(png_read_ptr, read_info_ptr, read_end_ptr);
		fprintf(stderr, "Error encountered while reading the png file.\n");
		exit(1);
	}
	png_init_io(*png_read_ptr, file);
	png_set_sig_bytes(*png_read_ptr, val);
}

void setup_info(png_structp png_read_ptr, png_infop read_info_ptr) {
	png_read_info(png_read_ptr, read_info_ptr);
    if (png_get_color_type(png_read_ptr, read_info_ptr) == PNG_COLOR_TYPE_RGB || png_get_color_type(png_read_ptr, read_info_ptr) == PNG_COLOR_TYPE_RGB == PNG_COLOR_TYPE_RGB_ALPHA) {
        png_set_rgb_to_gray_fixed(png_read_ptr, 1, 21268, 71514);
    }
}

void allocate_read_mem(png_structp png_read_ptr, png_bytep *row_pointers, unsigned height, unsigned width) {
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

void execute_read(png_structp png_read_ptr, png_infop read_info_ptr, png_infop read_end_ptr, png_bytep* row_pointers) {
	png_set_rows(png_read_ptr, read_info_ptr, row_pointers);
	png_read_image(png_read_ptr, row_pointers);
	png_read_end(png_read_ptr, read_end_ptr);
}

void cleanup_rows(png_structp png_read_ptr, png_bytep *readrows, unsigned height) {
	for (unsigned row = 0; row < height; row++) {
		png_free(png_read_ptr, readrows[row]);
	}
}

void cleanup_struct_mem(png_structp png_read_ptr, png_infop read_info_ptr, png_infop read_end_ptr) {
	png_destroy_read_struct(&png_read_ptr, &read_info_ptr, &read_end_ptr);
}
