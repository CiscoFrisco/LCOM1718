#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include <stdint.h>
#include "bitmap.h"


/** @defgroup video_gr video_gr
 * @{
 *
 * Constants and functions for outputting data to screen in graphics mode
 */

/* CONSTANTS */

#define BIT(n) 		   (0x01<<(n))     /**< @brief constant to get a bit position */

#define DEF_MODE       0x117   		   /**<@brief constant of the graphics mode used in this game */

#define LINEAR         BIT(14) 	 	   /**<@brief Linear FrameBuffer bit */

#define VG_SERVICE     0x10    		   /**<@brief BIOS video services */

#define VBE_02         0x4F02  		   /**<@brief VBE call, function 02 -- set VBE mode */

#define VBE_01         0x4F01  		   /**<@brief VBE call, function 01 -- return VBE mode information */

#define V_MODE_FUNC    0x00    		   /**<@brief BIOS video services */

#define TEXT_MODE      0x03    		   /**<@brief 80x25 text mode */

#define TRANSP_COLOR   0xF81F  		   /**<@brief Mouse's Transparent Color (Our "Green Screen") */

/* FUNCTIONS */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 */
void *vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/**
 * @brief Changes the color of a pixel.
 *
 * Receives x and y with respect to the upper left corner of the screen, calculates
 * the memory position and paints its pixel with the given color. Checks the range
 * of the parameters and return -1 in case they're invalid, else returns 0.
 *
 * @param x The x coordinate with respect to the top left corner of the screen.
 * @param y The y coordinate with respect to the top left corner of the screen.
 * @param color The new color of the pixel.
 * @param old_color A pointer that stores the old color of the pixel.
 * @return 0 if successful, -1 if not.
 */
int change_color(unsigned short x, unsigned short y, unsigned long color,
		unsigned long *old_color);

/**
 * @brief Draws a xpm in the given coordinates.
 *
 * Draws an xpm received as argument (and selected from the available ones in pixmap.h)
 * on the screen starting at the desired coordinates. Calls read_xpm() and then calls change_color()
 * to set the pixels on the screen to the color specified in the respective map position.
 * Also saves the previous colors in an array for further use.
 *
 * @param map A map from a xpm selected from the ones available on pixmap.h.
 * @param width width of the map
 * @param height height of the map
 * @param xi The x coordinate with respect to the top left corner of the screen.
 * @param yi The y coordinate with respect to the top left corner of the screen.
 * @param old_colors[] An array that stores the colors that will be overlaped.
 * @return 0 if successful, -1 if not.
 */
int draw_xpm(uint16_t* map, int width, int height, unsigned short xi,
		unsigned short yi, unsigned long old_colors[]);

/**
 * @brief Erases a xpm previously drawn in the given coordinates.
 *
 * Erases the xpm previously drawn by reading it and going to the respective starting
 * coordinates, setting the pixels to their old colors, stored in the old_colors[] array.
 *
 * @param map A map from a xpm selected from the ones available on pixmap.h.
 * @param width width of the map
 * @param height height of the map
 * @param xi The x coordinate with respect to the top left corner of the screen.
 * @param yi The y coordinate with respect to the top left corner of the screen.
 * @param old_colors[] An array that stores the colors that were overlaped by the xpm.
 * @return 0 if successful, -1 if not.
 */
int erase_xpm(uint16_t* map, int width, int height, unsigned short xi,
		unsigned short yi, unsigned long old_colors[]);

/**
 *	@brief Gets either the primary of secondary buffer based on its argument
 *	@param use_db indicates whether one wants to get the secondary of primary buffer
 *	@return a graphics buffer
 */
char* get_graphics_buffer(int use_db);

/**
 * @brief Gets horizontal resolution
 * @return Horizontal resolution of the screen
 */
unsigned get_hor_resolution();

/**
 * @brief Gets vertical resolution
 * @return Vertical resolution of the screen
 */
unsigned get_ver_resolution();

/**
 * @brief Copies secondary buffer to video_mem
 */
void double_buffer();

/**
 * @brief Copies video_mem data to the secondary buffer
 */
void cpy_buffer();

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
