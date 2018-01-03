#include <minix/drivers.h>
#include <minix/syslib.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include "video_gr.h"
#include "vbe.h"
#include "math.h"
#include "read_xpm.h"
#include "bitmap.h"
#include "sprite.h"

/* Private global variables */
static char *video_mem;				/**< @brief  Process (virtual) address to which VRAM is mapped */
static char *secondary_buffer; 		/**< @brief  for double buffering implementation */

static unsigned h_res;				/**< @brief  Horizontal screen resolution in pixels */
static unsigned v_res;				/**< @brief  Vertical screen resolution in pixels */
static unsigned bits_per_pixel; 	/**< @brief Number of VRAM bits per pixel */


void *vg_init(unsigned short mode)
{
	vbe_mode_info_t info;

	vbe_get_mode_info(mode, &info);

	struct reg86u reg;
	int r;
	struct mem_range mr;
	unsigned int vram_base = info.PhysBasePtr;  /* VRAM's physical addresss */
	bits_per_pixel = info.BitsPerPixel;
	h_res = info.XResolution;
	v_res = info.YResolution;
	unsigned int vram_size = h_res * v_res * (bits_per_pixel / 8);  /* VRAM's size, but you can use the frame-buffer size, instead */

	reg.u.w.ax = VBE_02 ; // VBE call, function 02 -- set VBE mode
	reg.u.w.bx = LINEAR | mode; // set bit 14: linear framebuffer
	reg.u.b.intno = VG_SERVICE ;

	if( sys_int86(&reg) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
	secondary_buffer = malloc(h_res * v_res * (bits_per_pixel / 8));

	if(video_mem == MAP_FAILED)
		panic("couldn't map video memory");

	return video_mem;
}

unsigned get_hor_resolution()
{
	return h_res;
}

unsigned get_ver_resolution()
{
	return v_res;
}

char* get_graphics_buffer(int use_db)
{
	if(use_db)
		return secondary_buffer;

	return video_mem;
}

void double_buffer(){
	memcpy(video_mem, secondary_buffer, h_res * v_res * (bits_per_pixel / 8));
}

void cpy_buffer(){
	memcpy(secondary_buffer, video_mem, h_res * v_res * (bits_per_pixel / 8));
}

int vg_exit()
{
	struct reg86u reg86;

	reg86.u.b.intno = VG_SERVICE; 	/* BIOS video services */

	reg86.u.b.ah = V_MODE_FUNC;    	/* Set Video Mode function */
	reg86.u.b.al = TEXT_MODE;    	/* 80x25 text mode*/

	free(secondary_buffer);

	if( sys_int86(&reg86) != OK )
	{
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	}
	else
		return 0;
}

int change_color(unsigned short x, unsigned short y, unsigned long color,
		unsigned long *old_color)
{
	//Stores temporarily the base memory position
	uint16_t* tmp = &((uint16_t*) video_mem)[y*h_res + x];

	//Check ranges
	if(x >= h_res || x < 0 || y >= v_res || y < 0)
		return -1;

	//Stores the pixel's current value
	*old_color = (unsigned long) *tmp;

	//Sets the pixel color, if it's not meant to be transparent
	if(*tmp != color && color!=TRANSP_COLOR)
		*tmp = color;

	return 0;
}


int draw_xpm(uint16_t* map, int width, int height, unsigned short xi,
		unsigned short yi, unsigned long old_colors[]){

	unsigned long old_color;
	unsigned short x = xi, y = yi;

	//print the pixmap
	int i, j;
	for(i = 0 ; i < height && y < v_res; i++)
	{
		for(j = 0; j < width; j++)
		{
			change_color(x, y, map[i*width + j], &old_color); //Set the pixel's color
			old_colors[i*width + j] = old_color; //Store the previous color in an array
			x++;
		}

		y++;
		x-=width;
	}

	return 0;
}

int erase_xpm(uint16_t* map, int width, int height, unsigned short xi,
		unsigned short yi, unsigned long old_colors[]){

	unsigned short x = xi, y = yi;
	unsigned long old_color;

	int i, j;

	for(i = 0;i < height && y < v_res; i++){
		for(j=0;j<width;j++){
			//Replace the pixel's value with its previous one
			change_color(x, y, old_colors[i*width + j], &old_color);
			x++;
		}

		y++;
		x-=width;
	}

	return 0;
}

Sprite *create_sprite(Bitmap* bitmap, Bitmap* empty, int x, int y, int xspeed,
		int yspeed)
{
	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc (sizeof(Sprite));

	if(sp == NULL)
		return NULL;

	// read the sprite bitmap
	sp->bitmap = bitmap;
	sp->empty = empty;

	if(sp->bitmap == NULL || sp->empty == NULL)
	{
		free(sp);
		return NULL;
	}
	else
	{ 	//If everything is okay, initialize the sprite
		sp->x = x;
		sp->y = y;
		sp->xspeed = xspeed;
		sp->yspeed = yspeed;
	}

	return sp;
}

void destroy_sprite(Sprite *sp)
{
	if(sp == NULL)
		return;

	if(sp ->bitmap != NULL)
		delete_bitmap(sp->bitmap);

	if(sp->empty != NULL)
		delete_bitmap(sp->empty);

	free(sp);
}

