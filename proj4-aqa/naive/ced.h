#define MAX_BRIGHTNESS 255
#define M_PI 3.14159265358979323846264338327

void setup_read(FILE *, FILE *, png_structp *, png_infop *, png_infop *);

void setup_info(png_structp, png_infop);

void execute_read(png_structp, png_infop, png_infop, png_bytep*);

void setup_write(FILE *, FILE *, png_structp, png_infop, png_infop, png_structp *, png_infop *);

void execute_write(png_structp, png_infop, png_bytep *);

void cleanup_struct_mem(png_structp, png_infop, png_infop, png_structp, png_infop);