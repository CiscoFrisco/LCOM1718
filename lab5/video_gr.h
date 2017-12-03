#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/**
 * Helper function for video_test_square(). Draws a square in the screen with the specified
 * size and color. Receives x and y with respect to the center of the screen. In case the square cannot be drawn inside the screen, return -1, else
 * return 0. Also accounts for the cases where only part of the square can be drawn.
 */
int vg_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color);

/*
 * Helper function for video_test_line(). Draws a line in the screen between the specified
 * points. Calculates its linear function, and uses it in order to paint the pixels.
 * Checks the coordinates and returns -1 if they're invalid, otherwise returns 0.
 */
int vg_line(unsigned short xi, unsigned short yi,unsigned short xf, unsigned short yf, unsigned long color);

/*
 * Receives x and y with respect to the upper left corner of the screen, calculates
 * the memory position and paints its pixel with the given color. Checks the range
 * of the parameters and return -1 in case they're invalid, else returns 0.
 */
int change_color(unsigned short x, unsigned short y, unsigned long color);

/*
 * Helper function that given an X coordinate, a slope and the intercept, calculates
 * y value.
 */
int calcFunc(unsigned short x, double slope, unsigned short b);

/*
 * Helper function that given two points, calculates slope and intercept.
 */
int calcSlopeB(unsigned short xi, unsigned short yi,unsigned short xf, unsigned short yf, double *slope, unsigned short *b);

/*
 * Draws an xpm received as argument (and selected from the available ones in pixmap.h)
 * on the screen starting at the desired coordinates. Calls read_xpm() and then, similar
 * to draw_square, calls change_color() to set the pixels on the screen to the color
 * specified in the respective map position.
 */
int draw_xpm(char *xpm[], unsigned short xi, unsigned short yi);

/*
 * Helper function that turns the previously altered pixels back to black.
 */
void clear_screen();

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
