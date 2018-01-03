#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "video_gr.h"
#include "game_macros.h"
#include "bitmap.h"
#include "timer.h"
#include "game.h"
#include "pixmap.h"
#include "i8042.h"
#include "sprite.h"
#include "timer.h"
#include "menu.h"
#include "mouse.h"
#include "read_xpm.h"
#include "keyboard.h"
#include "rtc.h"
#include "rtc_macros.h"
#include "highscores.h"
#include "uart.h"

extern unsigned char g_uart_rbr_data;

int init_game(){

	static GameState st = MAIN_M;

	//Load bitmaps
	GameBitmaps bmps;
	load_bmps(&bmps);

	//Initialize menus
	Bitmap* path[22];
	MenuItem main_menu_items[3];
	MenuItem choose_mode_items[6];
	MenuItem choose_bs_items[3];
	MenuItem hs_items[4];
	Menu main_menu, choose_mode, highscore, choose_bs;
	Menu menus[4] = {main_menu, choose_mode, highscore, choose_bs};
	load_menu_options(path);
	init_menus(path, menus, main_menu_items, choose_mode_items,
			choose_bs_items, hs_items);

	//Read highscores from their files
	Highscores hs = create_highscores();
	read_highscores(&hs);

	//Draw main menu
	draw_menu(menus[MAIN_M], bmps);


	/* SETUP GAME */

	//Stores bit mask in irq_set and subscribes interrupts
	int irq_timer = timer_subscribe_int();
	int irq_kbd = kbd_subscribe_int();
	int irq_mouse = mouse_subscribe_int();
	setup_LCR();
	setup_IER();
	subscribe_uart();

	set_stream_mode(0);

	int ipc_status, r;
	message msg;
	int x = 0,y = 0;
	int width, height;
	uint16_t* map = read_xpm(pic_mouse, &width, &height);
	unsigned long old_colors[width * height];
	unsigned int type=0;
	int byte_cnt=0;
	int print_cnt=0;
	Movement mov;
	int difficulty = 0;
	MenuHelper prev_item;
	int prev_ldown = -1;

	//Draw mouse
	draw_xpm(map, width, height,x,y,old_colors);

	while(st != EXIT_GAME)
	{
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if(msg.NOTIFY_ARG & irq_timer)
				{
					clearRBR();
				}
				if (msg.NOTIFY_ARG & irq_kbd)
				{
					if(kbd_c_wrapper(&type) == 0)
					{
						mov = cvt_makecode_mov(g_kbd_data, 1);

						erase_xpm(map, width, height, x, y, old_colors);

						if(mov != IGNORE)
						{
							event evt = set_event_kbd(mov, st, menus, &prev_item,
									hs, bmps);

							check_event(evt,&st, menus, &difficulty, &hs,
									&bmps);
						}

						draw_xpm(map, width, height, x, y, old_colors);
					}
				}

				if (msg.NOTIFY_ARG & irq_mouse)
				{
					if(mouse_wrapper(&byte_cnt) == 0)
					{
						erase_xpm(map, width, height, x, y, old_colors);

						x += get_mouse_X();
						y -= get_mouse_Y();
						check_mouse_boundaries(&x, &y);

						event evt = set_event_mouse(x, y, st, menus, &prev_item,
								&prev_ldown, hs,bmps);
						check_event(evt, &st, menus, &difficulty, &hs, &bmps);


						draw_xpm(map, width, height, x, y, old_colors);

						print_cnt++;
					}
				}
				break;
			default:
				break;
			}
		}
		else {}
	}

	//Disables data reporting
	if(disable_data_reporting(1)!=0)
		return -1;

	//Unsubscribes interrupts
	timer_unsubscribe_int();
	kbd_unsubscribe_int();
	mouse_unsubscribe_int();
	unsubscribe_uart();

	write_highscores(hs);
	delete_bmps(&bmps);

	return 0;
}

void load_bmps(GameBitmaps *bmps)
{
	//Load game images
	bmps->background = load_bitmap("home/lcom/slideit_files/bitmaps/background.bmp");
	bmps->background_main = load_bitmap("home/lcom/slideit_files/bitmaps/background_main.bmp");
	bmps->background_sp = load_bitmap("home/lcom/slideit_files/bitmaps/background_sp.bmp");
	bmps->background_mp = load_bitmap("home/lcom/slideit_files/bitmaps/background_mp.bmp");
	bmps->congratulations = load_bitmap("home/lcom/slideit_files/bitmaps/congratulations.bmp");
	bmps->mp_message1 = load_bitmap("/home/lcom/slideit_files/bitmaps/pre_game/mp_message1.bmp");
	bmps->mp_message2 = load_bitmap("/home/lcom/slideit_files/bitmaps/pre_game/mp_message2.bmp");
	bmps->lost = load_bitmap("/home/lcom/slideit_files/bitmaps/game/m_player/lost.bmp");
	bmps->won = load_bitmap("/home/lcom/slideit_files/bitmaps/game/m_player/won.bmp");
	bmps->lost_forfeit = load_bitmap("/home/lcom/slideit_files/bitmaps/game/m_player/forfeit_loss.bmp");
	bmps->won_forfeit = load_bitmap("/home/lcom/slideit_files/bitmaps/game/m_player/forfeit_win.bmp");
	bmps->leave = load_bitmap("/home/lcom/slideit_files/bitmaps/pre_game/leave.bmp");
	bmps->board_bkg = load_bitmap("home/lcom/slideit_files/bitmaps/game/board_bkg.bmp");
	bmps->exit_game = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/exit.bmp");
	bmps->timer_bg = load_bitmap("home/lcom/slideit_files/bitmaps/timer_bg.bmp");

	//Load pre-game bitmaps
	bmps->number3 = load_bitmap("home/lcom/slideit_files/bitmaps/pre_game/3.bmp");
	bmps->number2 = load_bitmap("home/lcom/slideit_files/bitmaps/pre_game/2.bmp");
	bmps->number1 = load_bitmap("home/lcom/slideit_files/bitmaps/pre_game/1.bmp");
	bmps->message1 = load_bitmap("home/lcom/slideit_files/bitmaps/pre_game/message.bmp");


	//Load numbers and letters
	bmps->numbers = malloc(sizeof(Bitmap*)*NUM_NUMBERS);
	bmps->letters = malloc(sizeof(Bitmap*)*NUM_LETTERS);
	init_letters(bmps->letters);
	init_numbers(bmps->numbers);

	//Load powerup symbols
	bmps->shuffle = load_bitmap("home/lcom/slideit_files/bitmaps/shuffle.bmp");
	bmps->shuffle_hi = load_bitmap("home/lcom/slideit_files/bitmaps/shuffle_hi.bmp");
	bmps->freeze = load_bitmap("home/lcom/slideit_files/bitmaps/freeze.bmp");
	bmps->freeze_hi = load_bitmap("home/lcom/slideit_files/bitmaps/freeze_hi.bmp");
	bmps->pups = load_bitmap("home/lcom/slideit_files/bitmaps/pups.bmp");

	bmps->hs_title = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/Highscores.bmp");
	bmps->bs_title = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/board_size.bmp");


	//Load images for puzzles
	bmps->panda = load_bitmap("home/lcom/slideit_files/bitmaps/pre_game/panda.bmp");
	bmps->panda_ez = malloc(sizeof(Bitmap*)*NUM_PIECES_EZ);
	bmps->panda_hd = malloc(sizeof(Bitmap*)*NUM_PIECES_HD);
	load_panda_ez(bmps->panda_ez);
	load_panda_hd(bmps->panda_hd);


	bmps->motorbike = load_bitmap("home/lcom/slideit_files/bitmaps/pre_game/m_bike.bmp");
	bmps->motorbike_ez = malloc(sizeof(Bitmap)*NUM_PIECES_EZ);
	bmps->motorbike_hd = malloc(sizeof(Bitmap)*NUM_PIECES_HD);
	load_mbike_ez(bmps->motorbike_ez);
	load_mbike_hd(bmps->motorbike_hd);


	bmps->landscape = load_bitmap("home/lcom/slideit_files/bitmaps/pre_game/landscape.bmp");
	bmps->landscape_ez = malloc(sizeof(Bitmap*)*NUM_PIECES_EZ);
	bmps->landscape_hd = malloc(sizeof(Bitmap*)*NUM_PIECES_HD);
	load_landscape_ez(bmps->landscape_ez);
	load_landscape_hd(bmps->landscape_hd);
}

void delete_bmps(GameBitmaps *bmps)
{
	delete_bitmap(bmps->number1);
	delete_bitmap(bmps->number2);
	delete_bitmap(bmps->number3);
	delete_bitmap(bmps->message1);
	delete_bitmap(bmps->background);
	delete_bitmap(bmps->background_sp);
	delete_bitmap(bmps->background_mp);
	delete_bitmap(bmps->congratulations);
	delete_bitmap(bmps->exit_game);
	delete_bitmap(bmps->timer_bg);
	delete_bitmap(bmps->mp_message1);
	delete_bitmap(bmps->mp_message2);
	delete_bitmap(bmps->lost);
	delete_bitmap(bmps->won);
	delete_bitmap(bmps->lost_forfeit);
	delete_bitmap(bmps->won_forfeit);
	delete_bitmap(bmps->shuffle);
	delete_bitmap(bmps->shuffle_hi);
	delete_bitmap(bmps->freeze);
	delete_bitmap(bmps->freeze_hi);
	delete_bitmap(bmps->hs_title);
	delete_bitmap(bmps->bs_title);
	delete_bitmap(bmps->panda);
	delete_bitmap(bmps->motorbike);
	delete_bitmap(bmps->landscape);
	delete_bitmap(bmps->leave);
	delete_bitmap(bmps->board_bkg);
	delete_bitmap(bmps->pups);

	int i=0;
	for(; i < NUM_LETTERS; ++i)
		delete_bitmap(bmps->letters[i]);

	free(bmps->letters);

	i=0;
	for(; i < NUM_NUMBERS; ++i)
		delete_bitmap(bmps->numbers[i]);

	free(bmps->numbers);

	i=0;
	for(; i < NUM_PIECES_EZ; ++i)
		delete_bitmap(bmps->panda_ez[i]);

	free(bmps->panda_ez);

	i=0;
	for(;i < NUM_PIECES_HD; ++i)
		delete_bitmap(bmps->panda_hd[i]);

	free(bmps->panda_hd);

	i=0;
	for(;i < NUM_PIECES_EZ; ++i)
		delete_bitmap(bmps->motorbike_ez[i]);

	free(bmps->motorbike_ez);

	i=0;
	for(;i < NUM_PIECES_HD; ++i)
		delete_bitmap(bmps->motorbike_hd[i]);

	free(bmps->motorbike_hd);

	i=0;
	for(;i < NUM_PIECES_EZ; ++i)
		delete_bitmap(bmps->landscape_ez[i]);

	free(bmps->landscape_ez);

	i=0;
	for(;i < NUM_PIECES_HD; ++i)
		delete_bitmap(bmps->landscape_hd[i]);

	free(bmps->landscape_hd);
}

void init_numbers(Bitmap* numbers[])
{
	numbers[0] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/0.bmp");
	numbers[1] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/1.bmp");
	numbers[2] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/2.bmp");
	numbers[3] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/3.bmp");
	numbers[4] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/4.bmp");
	numbers[5] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/5.bmp");
	numbers[6] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/6.bmp");
	numbers[7] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/7.bmp");
	numbers[8] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/8.bmp");
	numbers[9] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/9.bmp");
	numbers[10] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/dois_pontos.bmp");
	numbers[11] = load_bitmap("home/lcom/slideit_files/bitmaps/numbers/barra.bmp");
}

void init_letters(Bitmap* letters[])
{
	letters[0] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/a.bmp");
	letters[1] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/b.bmp");
	letters[2] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/c.bmp");
	letters[3] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/d.bmp");
	letters[4] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/e.bmp");
	letters[5] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/f.bmp");
	letters[6] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/g.bmp");
	letters[7] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/h.bmp");
	letters[8] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/i.bmp");
	letters[9] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/j.bmp");
	letters[10] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/k.bmp");
	letters[11] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/l.bmp");
	letters[12] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/m.bmp");
	letters[13] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/n.bmp");
	letters[14] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/o.bmp");
	letters[15] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/p.bmp");
	letters[16] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/q.bmp");
	letters[17] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/r.bmp");
	letters[18] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/s.bmp");
	letters[19] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/t.bmp");
	letters[20] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/u.bmp");
	letters[21] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/v.bmp");
	letters[22] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/w.bmp");
	letters[23] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/x.bmp");
	letters[24] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/y.bmp");
	letters[25] = load_bitmap("home/lcom/slideit_files/bitmaps/letters/z.bmp");

}

void load_panda_ez(Bitmap* bitmaps[])
{

	bitmaps[0] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda3x3/panda1.bmp");
	bitmaps[1] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda3x3/panda2.bmp");
	bitmaps[2] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda3x3/panda3.bmp");
	bitmaps[3] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda3x3/panda4.bmp");
	bitmaps[4] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda3x3/panda5.bmp");
	bitmaps[5] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda3x3/panda6.bmp");
	bitmaps[6] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda3x3/panda7.bmp");
	bitmaps[7] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda3x3/panda8.bmp");
	bitmaps[8] = load_bitmap("home/lcom/slideit_files/bitmaps/game/empty_piece33.bmp");
}

void load_panda_hd(Bitmap* bitmaps[])
{
	bitmaps[0] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda1.bmp");
	bitmaps[1] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda2.bmp");
	bitmaps[2] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda3.bmp");
	bitmaps[3] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda4.bmp");
	bitmaps[4] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda5.bmp");
	bitmaps[5] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda6.bmp");
	bitmaps[6] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda7.bmp");
	bitmaps[7] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda8.bmp");
	bitmaps[8] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda9.bmp");
	bitmaps[9] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda10.bmp");
	bitmaps[10] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda11.bmp");
	bitmaps[11] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda12.bmp");
	bitmaps[12] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda13.bmp");
	bitmaps[13] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda14.bmp");
	bitmaps[14] = load_bitmap("home/lcom/slideit_files/bitmaps/game/panda/panda15.bmp");
	bitmaps[15] = load_bitmap("home/lcom/slideit_files/bitmaps/game/empty_piece.bmp");
}

void load_mbike_ez(Bitmap* bitmaps[])
{
	bitmaps[0] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike3x3/m_bike1.bmp");
	bitmaps[1] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike3x3/m_bike2.bmp");
	bitmaps[2] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike3x3/m_bike3.bmp");
	bitmaps[3] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike3x3/m_bike4.bmp");
	bitmaps[4] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike3x3/m_bike5.bmp");
	bitmaps[5] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike3x3/m_bike6.bmp");
	bitmaps[6] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike3x3/m_bike7.bmp");
	bitmaps[7] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike3x3/m_bike8.bmp");
	bitmaps[8] = load_bitmap("home/lcom/slideit_files/bitmaps/game/empty_piece33.bmp");
}

void load_mbike_hd(Bitmap* bitmaps[])
{
	bitmaps[0] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike1.bmp");
	bitmaps[1] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike2.bmp");
	bitmaps[2] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike3.bmp");
	bitmaps[3] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike4.bmp");
	bitmaps[4] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike5.bmp");
	bitmaps[5] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike6.bmp");
	bitmaps[6] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike7.bmp");
	bitmaps[7] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike8.bmp");
	bitmaps[8] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike9.bmp");
	bitmaps[9] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike10.bmp");
	bitmaps[10] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike11.bmp");
	bitmaps[11] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike12.bmp");
	bitmaps[12] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike13.bmp");
	bitmaps[13] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike14.bmp");
	bitmaps[14] = load_bitmap("home/lcom/slideit_files/bitmaps/game/m_bike/m_bike15.bmp");
	bitmaps[15] = load_bitmap("home/lcom/slideit_files/bitmaps/game/empty_piece.bmp");
}

void load_landscape_ez(Bitmap* bitmaps[])
{
	bitmaps[0] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land3x3/land1.bmp");
	bitmaps[1] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land3x3/land2.bmp");
	bitmaps[2] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land3x3/land3.bmp");
	bitmaps[3] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land3x3/land4.bmp");
	bitmaps[4] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land3x3/land5.bmp");
	bitmaps[5] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land3x3/land6.bmp");
	bitmaps[6] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land3x3/land7.bmp");
	bitmaps[7] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land3x3/land8.bmp");
	bitmaps[8] = load_bitmap("home/lcom/slideit_files/bitmaps/game/empty_piece33.bmp");
}

void load_landscape_hd(Bitmap* bitmaps[])
{
	bitmaps[0] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land1.bmp");
	bitmaps[1] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land2.bmp");
	bitmaps[2] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land3.bmp");
	bitmaps[3] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land4.bmp");
	bitmaps[4] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land5.bmp");
	bitmaps[5] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land6.bmp");
	bitmaps[6] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land7.bmp");
	bitmaps[7] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land8.bmp");
	bitmaps[8] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land9.bmp");
	bitmaps[9] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land10.bmp");
	bitmaps[10] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land11.bmp");
	bitmaps[11] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land12.bmp");
	bitmaps[12] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land13.bmp");
	bitmaps[13] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land14.bmp");
	bitmaps[14] = load_bitmap("home/lcom/slideit_files/bitmaps/game/land/land15.bmp");
	bitmaps[15] = load_bitmap("home/lcom/slideit_files/bitmaps/game/empty_piece.bmp");
}

void init_puzzle_bitmaps(Bitmap* bitmaps[], GameBitmaps bmps, int num_pieces,
		int *current_puzzle)
{
	switch(*current_puzzle)
	{
	case PANDA:
		if(num_pieces == NUM_PIECES_EZ)
			memcpy(bitmaps,bmps.panda_ez,sizeof(Bitmap*)*num_pieces);
		else
			memcpy(bitmaps,bmps.panda_hd,sizeof(Bitmap*)*num_pieces);
		break;
	case MOTORBIKE:
		if(num_pieces == NUM_PIECES_EZ)
			memcpy(bitmaps,bmps.motorbike_ez,sizeof(Bitmap*)*num_pieces);
		else
			memcpy(bitmaps,bmps.motorbike_hd,sizeof(Bitmap*)*num_pieces);
		break;
	case LANDSCAPE:
		if(num_pieces == NUM_PIECES_EZ)
			memcpy(bitmaps,bmps.landscape_ez,sizeof(Bitmap*)*num_pieces);
		else
			memcpy(bitmaps,bmps.landscape_hd,sizeof(Bitmap*)*num_pieces);
		break;
	default:
		break;
	}
}

void init_puzzle_pieces(PuzzlePiece pieces[], Bitmap* bitmaps[],
		int num_pieces, unsigned int mode, GameBitmaps bmps, int *current_puzzle)
{

	init_puzzle_bitmaps(bitmaps,bmps,num_pieces,current_puzzle);

	int x = (mode == SP) ? BOARD_INIT_X_SP : BOARD_INIT_X_MP;
	int y = (mode == SP) ? BOARD_INIT_Y_SP : BOARD_INIT_Y_MP;
	int i = 0;

	for(; i < num_pieces; i++)
	{
		PuzzlePiece piece;
		PuzzleBitmap pb;
		piece.x_coord = x;
		piece.y_coord = y;
		pb.bitmap = bitmaps[i];
		piece.width = pb.bitmap->bitmapInfoHeader.width;
		pb.position = i;
		piece.bmp = pb;

		pieces[i] = piece;

		//If the next piece goes on the first column, adjust coordinates accordingly
		if((i + 1) % (int) sqrt(num_pieces)  == 0)
		{

			x = (mode == SP) ? BOARD_INIT_X_SP : BOARD_INIT_X_MP;

			if(num_pieces == NUM_PIECES_HD)
				y += 132;
			else
				y += 176;
		}
		else
		{
			if(num_pieces == NUM_PIECES_HD)
				x += 132;
			else
				x +=176;
		}
	}
}

void init_board(Board* board, PuzzlePiece pieces[], Bitmap* bitmaps[],int num_pieces,
		GameBitmaps bmps, unsigned int mode, int *current_puzzle)
{

	init_puzzle_pieces(pieces,bitmaps,num_pieces,mode,bmps,current_puzzle);

	board->x_init = (mode == SP) ? BOARD_INIT_X_SP : BOARD_INIT_X_MP;
	board->y_init = (mode == SP) ? BOARD_INIT_Y_SP : BOARD_INIT_Y_MP;
	board->size = BOARD_SIZE;

	if(num_pieces == NUM_PIECES_EZ)
		board->num_pieces = NUM_PIECES_EZ;
	else
		board->num_pieces = NUM_PIECES_HD;


	board->background  = bmps.board_bkg;

	board->pieces = malloc(board->num_pieces * sizeof(PuzzlePiece));
	memcpy(board->pieces, pieces, board->num_pieces * sizeof(PuzzlePiece));
}

