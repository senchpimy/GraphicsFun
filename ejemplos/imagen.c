#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <jpeglib.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_jpeg(const char *filename, unsigned char **image_data, int *width,
               int *height) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  FILE *infile;
  JSAMPARRAY buffer;
  int row_stride;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "Unable to open image file.\n");
    exit(1);
  }

  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  *width = cinfo.output_width;
  *height = cinfo.output_height;

  row_stride = cinfo.output_width * cinfo.output_components;
  *image_data = (unsigned char *)malloc(row_stride * cinfo.output_height);

  buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE,
                                      row_stride, 1);

  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);
    memcpy(*image_data + (cinfo.output_scanline - 1) * row_stride, buffer[0],
           row_stride);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);
}

void read_png(const char *filename, unsigned char **image_data, int *width,
              int *height) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file.\n");
    exit(1);
  }

  png_byte header[8];
  fread(header, 1, 8, fp);

  if (png_sig_cmp(header, 0, 8)) {
    fprintf(stderr, "File is not recognized as a PNG file.\n");
    fclose(fp);
    exit(1);
  }

  png_structp png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    fprintf(stderr, "Unable to create PNG read structure.\n");
    fclose(fp);
    exit(1);
  }

  png_infop info = png_create_info_struct(png);
  if (!info) {
    fprintf(stderr, "Unable to create PNG info structure.\n");
    png_destroy_read_struct(&png, (png_infopp)NULL, (png_infopp)NULL);
    fclose(fp);
    exit(1);
  }

  png_bytep row_pointers;

  png_set_sig_bytes(png, 8);
  png_init_io(png, fp);
  png_read_info(png, info);

  *width = png_get_image_width(png, info);
  *height = png_get_image_height(png, info);
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth = png_get_bit_depth(png, info);

  if (bit_depth == 16) {
    png_set_strip_16(png);
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png);
  }

  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(png);
  }

  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
  }

  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  }

  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png);
  }

  png_read_update_info(png, info);

  row_pointers = (png_bytep)malloc(sizeof(png_bytep) * *height);
  for (int y = 0; y < *height; y++) {
    row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png, info));
  }

  png_read_image(png, &row_pointers);

  fclose(fp);

  *image_data =
      (unsigned char *)malloc(sizeof(unsigned char) * 3 * (*width) * (*height));

  for (int y = 0; y < *height; y++) {
    png_bytep row = row_pointers[y];
    for (int x = 0; x < *width; x++) {
      png_bytep px = &(row[x * 4]);
      (*image_data)[(y * (*width) + x) * 3 + 0] = px[0];
      (*image_data)[(y * (*width) + x) * 3 + 1] = px[1];
      (*image_data)[(y * (*width) + x) * 3 + 2] = px[2];
    }
  }

  for (int y = 0; y < *height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);
  png_destroy_read_struct(&png, &info, NULL);
}

int main() {
  Display *display;
  Window window;
  XEvent event;
  XImage *image;
  Visual *visual;
  int depth;
  int screen;
  Colormap colormap;
  XSizeHints hints;

  display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "Unable to open display.\n");
    return 1;
  }

  screen = DefaultScreen(display);
  visual = DefaultVisual(display, screen);
  depth = DefaultDepth(display, screen);
  colormap = DefaultColormap(display, screen);

  window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, 640,
                               480, 1, BlackPixel(display, screen),
                               WhitePixel(display, screen));

  hints.x = 100;
  hints.y = 100;
  hints.width = 640;
  hints.height = 480;
  hints.flags = PPosition | PSize;

  XSetNormalHints(display, window, &hints);

  XSelectInput(display, window, ExposureMask | KeyPressMask);

  XMapWindow(display, window);

  // Uncomment one of the following lines depending on the image format
  // For JPEG
  // const char *filename = "example_image.jpg";

  // For PNG
  // const char *filename = "example_image.png";

  int width, height;
  unsigned char *image_data;

  // Uncomment one of the following lines depending on the image format
  // For JPEG
  // read_jpeg(filename, &image_data, &width, &height);

  // For PNG
  // read_png(filename, &image_data, &width, &height);

  image = XCreateImage(display, visual, depth, ZPixmap, 0, (char *)image_data,
                       width, height, 8, 0);

  while (1) {
    XNextEvent(display, &event);

    if (event.type == Expose) {
      XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0,
                width, height);
    }

    if (event.type == KeyPress) {
      break;
    }
  }

  XDestroyImage(image);
  free(image_data);
  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
}
