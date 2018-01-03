/**
 *  @author Joao Cardoso (jcard@fe.up.pt) ????
 *  Added by pfs@fe.up.pt
 *
 *  Slightly altered with the help of professor Pedro Silva in order
 *  to support xpm in 16-bit modes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HRES        1024
#define VRES        768
#define NUM_COLORS 	256
#define MAX_COLOR 	65535

uint16_t *read_xpm(char *map[], int *wd, int *ht)
{
	int width, height, colors;
	uint16_t sym[NUM_COLORS];
	int  col;
	int i, j;
	uint16_t *pix, *pixtmp;
	char *line;
	unsigned char symbol;

	/* read width, height, colors */
	if (sscanf(map[0],"%d %d %d", &width, &height, &colors) != 3)
	{
		printf("read_xpm: incorrect width, height, colors\n");
		return NULL;
	}

	if (width > HRES || height > VRES || colors > NUM_COLORS)
	{
		printf("read_xpm: incorrect width, height, colors\n");
		return NULL;
	}

	*wd = width;
	*ht = height;

	memset(sym, 0, sizeof(sym));

	/* read symbols <-> colors */
	for (i = 0; i < colors; i++)
	{
		if (sscanf(map[i+1], "%c %d", &symbol, &col) != 2)
		{
			printf("read_xpm: incorrect symbol, color at line %d\n", i+1);
			return NULL;
		}

		if (col > MAX_COLOR)
		{
			printf("read_xpm: incorrect color at line %d\n", i+1);
			return NULL;
		}

		sym[symbol] = col;
	}

	/* allocate pixmap memory */
	pix = pixtmp = malloc(width * height * 2);

	/* parse each pixmap symbol line */
	for (i = 0; i < height; i++)
	{
		line = map[colors + 1 + i];

		for (j = 0; j < width; j++)
			*pixtmp++ = sym[(unsigned) line[j]];
	}

	return pix;
}
