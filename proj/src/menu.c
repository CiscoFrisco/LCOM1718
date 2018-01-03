#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "menu.h"
#include "video_gr.h"
#include "mouse.h"
#include "timer.h"
#include "keyboard.h"
#include "game_macros.h"
#include "game.h"

void load_menu_options(Bitmap* path[])
{
	path[0] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/play.bmp");
	path[1] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/play_hi.bmp");
	path[2] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/highscores.bmp");
	path[3] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/highscores_hi.bmp");
	path[4] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/regular.bmp");
	path[5] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/regular_hi.bmp");
	path[6] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/timetrial.bmp");
	path[7] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/timetrial_hi.bmp");
	path[8] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/arcade.bmp");
	path[9] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/arcade_hi.bmp");
	path[10] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/mp.bmp");
	path[11] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/mp_hi.bmp");
	path[12] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/options.bmp");
	path[13] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/options_hi.bmp");
	path[14] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/3x3.bmp");
	path[15] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/3x3_hi.bmp");
	path[16] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/4x4.bmp");
	path[17] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/4x4_hi.bmp");
	path[18] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/back.bmp");
	path[19] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/back_hi.bmp");
	path[20] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/exit.bmp");
	path[21] = load_bitmap("home/lcom/slideit_files/bitmaps/menu_options/exit_hi.bmp");
}

void init_main_menu_items(Bitmap* path[], MenuItem items[])
{
	unsigned int j = 0, i = 0;
	int y = 284;

	//Adding items to navigate to Choose Game and Highscores menus
	for(; j < 2; j++, i += 2)
	{
		items[j].bitmap = path[i];
		items[j].bitmapHi = path[i + 1];
		items[j].x_coord = MENU_OPTION_XI;
		items[j].y_coord = y;
		items[j].width = MENU_OPTION_WIDTH;
		items[j].height = MENU_OPTION_HEIGHT;

		y += 150;
	}

	//Adding an exit game option
	items[j].bitmap = path[20];
	items[j].bitmapHi = path[21];
	items[j].x_coord = MENU_OPTION_XI;
	items[j].y_coord = y;
	items[j].width = MENU_OPTION_WIDTH;
	items[j].height = MENU_OPTION_HEIGHT;
}

void draw_menu (Menu menu, GameBitmaps bmps)
{
	switch(menu.id)
	{
	case MAIN_M:
		draw_bitmap(bmps.background_main, 0, 0, USE_DB);
		break;
	case CHOOSE_GAME:
		draw_bitmap(bmps.background_main, 0, 0, USE_DB);
		break;
	case HIGHSCORES:
		draw_bitmap(bmps.background_main, 0, 0, USE_DB);
		draw_bitmap(bmps.hs_title, 400, 0, USE_DB);
		break;
	case CHOOSE_BOARD_SIZE:
		draw_bitmap(bmps.background_main, 0, 0, USE_DB);
		draw_bitmap(bmps.bs_title, MENU_OPTION_XI, 300, USE_DB);
		break;
	default:
		break;
	}

	unsigned int i = 0;

	//Draw items according to their highlighted status
	for(; i < menu.num_items; i++)
	{
		if(i != menu.highlighted)
		{
			draw_bitmap(menu.items[i].bitmap, menu.items[i].x_coord,
					menu.items[i].y_coord, USE_DB);
		}
		else
		{
			draw_bitmap(menu.items[i].bitmapHi, menu.items[i].x_coord,
					menu.items[i].y_coord, USE_DB);
		}
	}

	double_buffer();
}

void init_choose_mode_menu_items(Bitmap* path[], MenuItem items[])
{
	unsigned int j = 0, i = 4;
	int y = 284;

	//Adding items to navigate to game modes
	for(; j < 5; i += 2, j++)
	{
		items[j].bitmap = path[i];
		items[j].bitmapHi = path[i + 1];
		items[j].x_coord = MENU_OPTION_XI;
		items[j].y_coord = y;
		items[j].width = MENU_OPTION_WIDTH;
		items[j].height = MENU_OPTION_HEIGHT;

		y += 75;
	}

	//Adding a 'back' option
	items[j].bitmap = path[18];
	items[j].bitmapHi = path[19];
	items[j].x_coord = MENU_OPTION_XI;
	items[j].y_coord = y;
	items[j].width = MENU_OPTION_WIDTH;
	items[j].height = MENU_OPTION_HEIGHT;
}

void init_menu(Menu *menu, MenuItem items[], unsigned int num_items,
		unsigned int id)
{
	menu->highlighted = DEFAULT_HIGHLIGHTED;
	menu->num_items = num_items;
	menu->id = id;
	menu->items = malloc(sizeof(MenuItem)*menu->num_items);
	memcpy(menu->items, items, menu->num_items * sizeof(MenuItem));
}

void init_choose_bs_menu_items(Bitmap* path[], MenuItem items[])
{
	unsigned int j = 0, i = 14;
	int y = 384;

	//Adding items to choose board size
	for(; j < 2; i += 2, j++)
	{
		items[j].bitmap = path[i];
		items[j].bitmapHi = path[i +1];
		items[j].x_coord = MENU_OPTION_XI;
		items[j].y_coord = y;
		items[j].width = MENU_OPTION_WIDTH;
		items[j].height = MENU_OPTION_HEIGHT;

		y += 100;
	}

	//Adding a 'back' option
	items[j].bitmap = path[18];
	items[j].bitmapHi = path[19];
	items[j].x_coord = MENU_OPTION_XI;
	items[j].y_coord = y;
	items[j].width = MENU_OPTION_WIDTH;
	items[j].height = MENU_OPTION_HEIGHT;
}

void init_hs_menu_items(Bitmap* path[], MenuItem items[])
{
	unsigned int j=0, i = 4;
	int x = 40;

	//Adding items to choose which highscores to reveal
	for(; j < 3;i += 2, j++)
	{
		items[j].bitmap = path[i];
		items[j].bitmapHi = path[i +1];
		items[j].x_coord = x;
		items[j].y_coord = 200;
		items[j].width = MENU_OPTION_WIDTH;
		items[j].height = MENU_OPTION_HEIGHT;

		x+= 300;
	}

	//Adding a 'back' option
	items[j].bitmap = path[18];
	items[j].bitmapHi = path[19];
	items[j].x_coord = 340;
	items[j].y_coord = 600;
	items[j].width = MENU_OPTION_WIDTH;
	items[j].height = MENU_OPTION_HEIGHT;

}

void init_menus(Bitmap* path[], Menu menus[], MenuItem main_menu_items[],
		MenuItem choose_mode_items[], MenuItem hs_items[],
		MenuItem choose_bs_items[])
{
	init_main_menu_items(path, main_menu_items);
	init_menu(&menus[MAIN_M], main_menu_items, 3, MAIN_M);

	init_choose_mode_menu_items(path, choose_mode_items);
	init_menu(&menus[CHOOSE_GAME], choose_mode_items, 6, CHOOSE_GAME);

	init_hs_menu_items(path, hs_items);
	init_menu(&menus[HIGHSCORES],hs_items,4, HIGHSCORES);

	init_choose_bs_menu_items(path, choose_bs_items);
	init_menu(&menus[CHOOSE_BOARD_SIZE], choose_bs_items, 3, CHOOSE_BOARD_SIZE);
}

void check_event(event evt, GameState *st, Menu menus[], int *difficulty,
		Highscores *hs, GameBitmaps *bmps)
{
	if(evt == FAIL)
		return;

	switch(*st)
	{
	case MAIN_M:
		if(evt == TO_GAME)
		{
			*st = CHOOSE_GAME;
			draw_menu(menus[CHOOSE_GAME], *bmps);
		}
		else if(evt == TO_HIGH)
		{
			*st = HIGHSCORES;
			draw_menu(menus[HIGHSCORES], *bmps);
		}
		else
			*st =  EXIT_GAME;
		break;
	case CHOOSE_GAME:
		if(evt == TO_BS)
		{
			*st = CHOOSE_BOARD_SIZE;
			draw_menu(menus[CHOOSE_BOARD_SIZE], *bmps);
		}
		else if(evt == TO_REGULAR)
		{
			*st = IN_GAME;
			regular_timetrial(difficulty, hs, REGULAR, bmps);
			*st = MAIN_M;
			draw_menu(menus[MAIN_M], *bmps);
		}
		else if(evt == TO_ARCADE)
		{
			*st = IN_GAME;
			arcade_mode(difficulty, hs, bmps);
			*st = MAIN_M;
			draw_menu(menus[MAIN_M], *bmps);
		}
		else if(evt == TO_T_TRIAL)
		{
			*st = IN_GAME;
			regular_timetrial(difficulty, hs, TIMETRIAL, bmps);

			*st = MAIN_M;
			draw_menu(menus[MAIN_M], *bmps);
		}
		else if(evt == TO_M_PLAYER)
		{
			*st = IN_GAME;
			multi_player_mode(difficulty, bmps);
			*st = MAIN_M;
			draw_menu(menus[MAIN_M], *bmps);
		}
		else
		{
			*st = MAIN_M;
			draw_menu(menus[MAIN_M], *bmps);
		}
		break;
	case HIGHSCORES:
		if(evt == BACK)
		{
			*st = MAIN_M;
			draw_menu(menus[MAIN_M], *bmps);
		}
		break;
	case CHOOSE_BOARD_SIZE:
		if(evt == EZ)
			*difficulty = EASY;
		else if(evt == HD)
			*difficulty = HARD;
		else
		{
			*st = CHOOSE_GAME;
			draw_menu(menus[CHOOSE_GAME], *bmps);
		}
		break;
	default:
		break;
	}
}

event set_event_kbd(Movement mov, GameState st, Menu menus[],
		MenuHelper *prev_item, Highscores hs, GameBitmaps bmps)
{
	unsigned int current_option = menus[st].highlighted;

	if(current_option == FIRST_OPTION && mov == UP)
		return FAIL;
	else if(current_option == menus[st].num_items - 1 && mov == DOWN)
		return FAIL;
	else if (menus[st].num_items == 1 && mov != ENTER)
		return FAIL;
	else
	{
		if(mov == UP)
		{
			menus[st].highlighted = current_option - 1;
			prev_item->itemIndex = current_option - 1;
			prev_item->menuIndex = st;
			draw_menu(menus[st], bmps);

			if(st == HIGHSCORES && (menus[st].highlighted >= 0 &&
					menus[st].highlighted <= 2))
				draw_highscores(hs, menus[st].highlighted, bmps.letters, bmps.numbers);

			return FAIL;
		}
		else if(mov == DOWN)
		{
			menus[st].highlighted = current_option + 1;
			prev_item->itemIndex = current_option + 1;
			prev_item->menuIndex = st;
			draw_menu(menus[st], bmps);

			if(st == HIGHSCORES && (menus[st].highlighted >= 0 &&
					menus[st].highlighted <= 2))
				draw_highscores(hs, menus[st].highlighted, bmps.letters, bmps.numbers);

			return FAIL;
		}
		else if(mov == ENTER)
		{
			switch(st)
			{
			case MAIN_M:
				if(current_option == 0)
					return TO_GAME;
				else if(current_option == 1)
					return TO_HIGH;
				else
					return QUIT;
				break;
			case CHOOSE_GAME:
				if(current_option == 0)
					return TO_REGULAR;
				else if (current_option == 1)
					return TO_T_TRIAL;
				else if (current_option == 2)
					return TO_ARCADE;
				else if(current_option == 3)
					return TO_M_PLAYER;
				else if(current_option == 4)
					return TO_BS;
				else
					return BACK;
				break;
			case HIGHSCORES:
				return BACK;
				break;
			case CHOOSE_BOARD_SIZE:
				if(current_option == 0)
					return EZ;
				else if (current_option == 1)
					return HD;
				else
					return BACK;
				break;
			default:
				return FAIL;
				break;
			}
		}
		else
			return FAIL;
	}
}

MenuHelper get_menu_item(int x, int y, Menu menu)
{
	MenuHelper h;
	h.menuIndex = menu.id;

	int i = 0;
	for(;i < menu.num_items; i++)
	{
		int xi = menu.items[i].x_coord;
		int yi = menu.items[i].y_coord;
		int xf = xi + menu.items[i].width;
		int yf = yi + menu.items[i].height;

		if(x >= xi && x <= xf && y >= yi && y <= yf)
		{
			h.itemIndex = i;
			return h;
		}
	}

	h.itemIndex = -1;
	return h;
}

unsigned int cmp_menu_helper(MenuHelper m1, MenuHelper m2)
{
	if(m1.itemIndex == m2.itemIndex && m1.menuIndex == m2.menuIndex)
		return 1;

	return 0;
}

event set_event_mouse(int x, int y, GameState st, Menu menus[],
		MenuHelper *prev_item, int *prev_ldown, Highscores hs,
		GameBitmaps bmps)
{
	MenuHelper hovered_item = get_menu_item(x, y, menus[st]);

	if(hovered_item.itemIndex == -1)
	{
		*prev_item = hovered_item;
		*prev_ldown = -1;
		return FAIL;
	}
	else if(checkLDOWN()!=0 && prev_item->itemIndex == -1)
	{
		menus[st].highlighted = hovered_item.itemIndex;
		draw_menu(menus[st], bmps);

		//In Highscores menu, draw highscores when an option is hovered
		if(st == HIGHSCORES && (hovered_item.itemIndex >= 0 &&
				hovered_item.itemIndex <= 2))
			draw_highscores(hs, menus[st].highlighted, bmps.letters, bmps.numbers);

		*prev_item = hovered_item;
		*prev_ldown = -1;
		return FAIL;
	}
	else if(checkLDOWN()!=0 && prev_item->itemIndex != -1 &&
			!cmp_menu_helper(*prev_item, hovered_item))
	{
		menus[st].highlighted = hovered_item.itemIndex;
		draw_menu(menus[st], bmps);

		//In Highscores menu, draw highscores when an option is hovered
		if(st == HIGHSCORES && (hovered_item.itemIndex >= 0 &&
				hovered_item.itemIndex <= 2))
			draw_highscores(hs, menus[st].highlighted, bmps.letters, bmps.numbers);

		*prev_item = hovered_item;
		*prev_ldown = -1;
		return FAIL;
	}
	else if(checkLDOWN() == 0 && *prev_ldown != 0)
	{
		menus[st].highlighted = hovered_item.itemIndex;
		*prev_item = hovered_item;
		*prev_ldown = 0;

		switch(st)
		{
		case MAIN_M:
			if(hovered_item.itemIndex == 0)
				return TO_GAME;
			else if(hovered_item.itemIndex == 1)
				return TO_HIGH;
			else if(hovered_item.itemIndex == 2)
				return QUIT;
			else
				return FAIL;
			break;
		case CHOOSE_GAME:
			if(hovered_item.itemIndex == 0)
				return TO_REGULAR;
			else if (hovered_item.itemIndex == 1)
				return TO_T_TRIAL;
			else if (hovered_item.itemIndex == 2)
				return TO_ARCADE;
			else if(hovered_item.itemIndex == 3)
				return TO_M_PLAYER;
			else if(hovered_item.itemIndex == 4)
				return TO_BS;
			else if(hovered_item.itemIndex == 5)
				return BACK;
			else
				return FAIL;
			break;
		case HIGHSCORES:
			if(hovered_item.itemIndex == 3)
				return BACK;
			else
				return FAIL;
			break;
		case CHOOSE_BOARD_SIZE:
			if(hovered_item.itemIndex == 0)
				return EZ;
			else if (hovered_item.itemIndex == 1)
				return HD;
			else if(hovered_item.itemIndex == 2)
				return BACK;
			else
				return FAIL;
			break;
		default:
			return FAIL;
			break;
		}

	}
	else
		return FAIL;
}

event check_exit(int x, int y, int mode)
{
	int yi;
	int xi = EXIT_GAME_X;
	int xf = xi + MENU_OPTION_WIDTH;

	if(mode == SP)
		yi = EXIT_GAME_Y_SP;
	else
		yi = EXIT_GAME_Y_MP;

	int yf = yi + MENU_OPTION_HEIGHT;

	if(x >= xi && x <= xf && y >= yi && y <= yf)
		return TO_MENU;

	return FAIL;
}
