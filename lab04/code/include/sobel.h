struct img_1D_t *edge_detection_1D(const struct img_1D_t *input_img);

void rgb_to_grayscale_1D(const struct img_1D_t *img, struct img_1D_t *result);

void gaussian_filter_1D(const struct img_1D_t *img, struct img_1D_t *res_img, const uint16_t *kernel);

void sobel_filter_1D(const struct img_1D_t *img, struct img_1D_t *res_img, 
                  const int16_t *v_kernel, const int16_t *h_kernel);


struct img_chained_t *edge_detection_chained(const struct img_chained_t *input_img);

void rgb_to_grayscale_chained(const struct img_chained_t *img, struct img_chained_t *result);

void gaussian_filter_chained(const struct img_chained_t *img, struct img_chained_t *res_img, const uint16_t *kernel);

void sobel_filter_chained(const struct img_chained_t *img, struct img_chained_t *res_img, 
                  const int16_t *v_kernel, const int16_t *h_kernel);