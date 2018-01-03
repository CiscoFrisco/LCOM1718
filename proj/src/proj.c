#include <minix/syslib.h>
#include <stdlib.h>
#include <time.h>
#include "init_game.h"
#include "video_gr.h"

int main(int argc, char **argv)
{
	srand(time(NULL));

	sef_startup();
	sys_enable_iop(SELF);

	vg_init(DEF_MODE);

	init_game(argv[1]);

	vg_exit();

	return 0;

}
