#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "video_gr.h"
#include "vg_macros.h"
#include "vbe.h"
#include "math.h"
#include "video_test.h"
#include "read_xpm.h"
#include "sprite.h"
#include <stdlib.h>

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	  0xE0000000
#define H_RES             1024
#define V_RES		      768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void *vg_init(unsigned short mode){

	vbe_mode_info_t info;

	vbe_get_mode_info(mode,&info);

	struct reg86u reg;
	int r;
	struct mem_range mr;
	unsigned int vram_base = info.PhysBasePtr;  /* VRAM's physical addresss */
	bits_per_pixel = info.BitsPerPixel;
	h_res=info.XResolution;
	v_res=info.YResolution;
	unsigned int vram_size = h_res * v_res * (bits_per_pixel / 8);  /* VRAM's size, but you can use the frame-buffer size, instead */

	reg.u.w.ax = VBE_02 ; // VBE call, function 02 -- set VBE mode
	reg.u.w.bx = LINEAR |mode; // set bit 14: linear framebuffer
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

	if(video_mem == MAP_FAILED)
		panic("couldn't map video memory");

	return video_mem;
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = VG_SERVICE; /* BIOS video services */

	reg86.u.b.ah = V_MODE_FUNC;    /* Set Video Mode function */
	reg86.u.b.al = TEXT_MODE;    /* 80x25 text mode*/

	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

//Falta verificar caso em que canto superior esquerdo fica em cima e quadrado e muito grande
int vg_square(unsigned short x,unsigned short y, unsigned short size, unsigned long color)
{
	int center_x = h_res / 2;
	int center_y = v_res / 2;
	unsigned short h_size=size;
	unsigned short v_size=size;

	//calculate the upper left corner coordinates of the square
	int square_corner_x = center_x + x - (size/2);
	int square_corner_y = center_y + y - (size/2);

	//if square upper left corner is beyond limits
	if(square_corner_x > (int) h_res || square_corner_y > (int) v_res)
		return -1;

	//Check if the square will go off limits and adjust size in the respective axis
	if(square_corner_x + size>h_res && square_corner_x>0)
		h_size=h_res-square_corner_x;
	if(square_corner_y + size>v_res && square_corner_y>0)
		v_size=v_res-square_corner_y;

	//Check if the upper left corner coordinates are negative. In that case, set them to 0 and adjust size in that axis
	if(square_corner_y<0){
		square_corner_y=0;
		v_size=V_RES;
	}
	if(square_corner_x<0){
		square_corner_x=0;
		h_size=H_RES;
	}

	//print the square
	int i;
	for(i = 0; i < v_size && square_corner_y < V_RES; i++)
	{
		int j;
		for (j = 0; j < h_size; j++)
		{
			//Set the pixel and adjust square_corner_x
			change_color(square_corner_x, square_corner_y,color);
			square_corner_x+=bits_per_pixel/8;
		}
		square_corner_y+=bits_per_pixel/8;
		square_corner_x-=h_size*bits_per_pixel/8;
	}

	return 0;
}

int calcSlopeB(unsigned short xi, unsigned short yi,unsigned short xf, unsigned short yf, double *slope, unsigned short *b)
{
	//calculate the slope
	*slope = (double) (yf-yi)/(xf-xi);

	//calculate the intercept
	*b = yi - (*slope)*xi;

	return 0;
}

int calcFunc(unsigned short x, double slope, unsigned short b)
{
	//calculate the y coordinate
	return (int)(slope*x+b);
}


int vg_line(unsigned short xi, unsigned short yi,unsigned short xf, unsigned short yf, unsigned long color)
{
	unsigned short x,y;
	unsigned short xInicial, yInicial, xFinal, yFinal;
	double slope;
	unsigned short b;

	//If the initial coordinates are greater than the final, just switch them
	if(xi>xf){
		xInicial = xf;
		yInicial = yf;
		xFinal = xi;
		yFinal = yi;
	}else{
		xInicial = xi;
		yInicial = yi;
		xFinal = xf;
		yFinal = yf;
	}

	calcSlopeB(xInicial,yInicial,xFinal,yFinal,&slope,&b);

	//while xf hasn't been reached, paint the line's pixels
	if(xInicial != xFinal)
	{
		for(x=xInicial;x<xFinal;x++){
			change_color(x,calcFunc(x,slope,b),color);
		}
	}

	//Special case -> it's a vertical line
	else
		for(y = yInicial; y < yFinal; y++)
			change_color(xInicial,y,color);

	return 0;
}

int change_color(unsigned short x, unsigned short y, unsigned long color)
{
	//Stores temporarily the base memory position
	char* tmp = video_mem;

	//Check ranges
	if(color > 0x3F || color < 0x00 || x>=h_res || x<0 || y>=v_res || y<0)
		return -1;

	//Calculate memory position
	tmp += (y*h_res + x)*(bits_per_pixel/8);

	//Sets the pixel color
	if(*tmp != color)
		*tmp = color;

	return 0;
}

int draw_xpm(char *xpm[], unsigned short xi, unsigned short yi){

	int width, height;
	//transform into a bitmap
	char* map = read_xpm(xpm,&width,&height);

	unsigned short x=xi, y=yi;

	//print the bitmap
	int i,j;
	for(i=0;i<height && y<V_RES;i++){
		for(j=0;j<width;j++){
			change_color(x,y,*(map + i*width + j));
			x++;
		}
		y++;
		x-=width;
	}


	return 0;
}


Sprite *create_sprite(char *pic[], int x, int y, int xspeed, int yspeed) {
	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
	if( sp == NULL )
		return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if( sp->map == NULL ) {
		free(sp);
		return NULL;
	}
	else
	{
		sp->x =x;
		sp->y =y;
		sp->xspeed = xspeed;
		sp->yspeed = yspeed;
	}

	return sp;
}

void destroy_sprite(Sprite *sp) {
	if( sp == NULL )
		return;
	if( sp ->map )
		free(sp->map);
	free(sp);
	sp = NULL; // XXX: pointer is passed by value
	// should do this @ the caller
}

void clear_screen()
{

	int x = 0, y = 0;
	int color = 0; //set all pixels to black
	int i,j;
	for(i=0;i<V_RES;i++){
		for(j=0;j<H_RES;j++){
			change_color(x,y,color);
			x++;
		}
		y++;
		x-=H_RES;
	}
}


