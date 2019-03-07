void canny_edge_detection(char *, char *);

void gaussian_filter(png_bytep *, png_bytep *, const unsigned, const unsigned, const float);

void convolution(png_bytep *, png_bytep *, float *, const unsigned, const unsigned, const int, const bool);

void intensity_gradients(png_bytep *, png_bytep *, png_bytep *, float *, float *, const unsigned, const unsigned);

void non_maximum_suppression(png_bytep *, float *, float *, const unsigned, const unsigned);

void hysteresis(png_bytep *, png_bytep *, const unsigned, const unsigned, const unsigned, const unsigned);

void allocate_read_mem(png_structp, png_bytep *, unsigned, unsigned);

void allocate_write_mem(png_structp, png_bytep *, png_bytep *, png_bytep *, png_bytep *, png_bytep *, unsigned, unsigned);

void cleanup_rows(png_structp, png_bytep *, png_bytep *, png_bytep *, png_bytep *, png_bytep *, png_structp, png_bytep *, unsigned);

void handle_batch(char **s, char **, unsigned);