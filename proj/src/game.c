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

void check_timer(int mode, unsigned int *seconds, InGameState *st)
{
	if (mode != TIMETRIAL)
		(*seconds)++;
	else
	{
		(*seconds)--;

		if (*seconds == 0)
			*st = EXIT_G;
	}
}

int regular_timetrial(int *difficulty, Highscores *hs, int mode,
		GameBitmaps *bmps)
{

	int puzz = rand() % 3;
	Board board;
	int num_pieces = (*difficulty == EASY) ? NUM_PIECES_EZ : NUM_PIECES_HD;
	Bitmap *bitmaps[num_pieces];
	PuzzlePiece pieces[num_pieces];

	init_board(&board, pieces, bitmaps, num_pieces, *bmps, SP, &puzz);

	show_full_image(puzz, *bmps);
	draw_game(board, *bmps, SP);
	shuffle(&board,0);

	int irq_timer = get_timer_hook_id();
	int irq_kbd = get_keyboard_hook_id();
	int irq_mouse = get_mouse_hook_id();

	unsigned int seconds;
	switch(mode)
	{
	case REGULAR_M:
		seconds = 0;
		break;
	case TIME_TRIAL_M:
		if(*difficulty == 0)
			seconds = TT_EZ;
		else
			seconds = TT_HD;
		break;
	default:
		break;
	}

	int ipc_status, r;
	message msg;
	int x = 0, y = 0;
	int counter = 0;
	InGameState st = PLAYING;
	int width, height;
	uint16_t *map = read_xpm(pic_mouse, &width, &height);
	unsigned long old_colors[width * height];

	unsigned int type = 0;
	int byte_cnt = 0;
	int mov_counter = 0;
	Movement mov;
	int prev_ldown = 1;

	draw_xpm(map, width, height, x, y, old_colors);

	while (st == PLAYING)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_timer)
				{
					if (counter % 60 == 0)
					{
						check_timer(mode, &seconds, &st);

						draw_timer(bmps, seconds);
					}

					counter++;
				}
				if (msg.NOTIFY_ARG & irq_kbd)
				{
					if (kbd_c_wrapper(&type) == 0)
					{
						mov = cvt_makecode_mov(g_kbd_data, 0);

						erase_xpm(map, width, height, x, y, old_colors);

						if (mov != IGNORE)
						{
							if (check_movement(&board, mov, &mov_counter, 0) == 0)
								draw_mov_counter(bmps, mov_counter);

							if (check_solution(&board) == board.num_pieces)
								st = COMPLETE;
						}

						draw_xpm(map, width, height, x, y, old_colors);
					}
				}

				if (msg.NOTIFY_ARG & irq_mouse)
				{
					if (mouse_wrapper(&byte_cnt) == 0)
					{
						erase_xpm(map, width, height, x, y, old_colors);

						x += get_mouse_X();
						y -= get_mouse_Y();
						check_mouse_boundaries(&x, &y);

						if (checkLDOWN() == 0 && prev_ldown != 0)
						{
							if (check_mouse_movement(&board, x, y, &mov_counter, 0) == 0)
								draw_mov_counter(bmps, mov_counter);

							if (check_solution(&board) == board.num_pieces)
								st = COMPLETE;

							event evt = check_exit(x, y, SP);

							if (evt == TO_MENU)
								st = EXIT_G;
						}

						prev_ldown = checkLDOWN();

						draw_xpm(map, width, height, x, y, old_colors);
					}
				}

				break;
			default:
				break;
			}
		}
		else
		{
		}
	}

	if (st == COMPLETE)
		check_highscore(mode, hs, seconds, *difficulty, mov_counter, *bmps);

	return st;
}

int arcade_mode(int *difficulty, Highscores *hs, GameBitmaps *bmps)
{
	int puzz_counter = 0;
	int puzzle_id[3] = {-1, -1, -1};

	int irq_timer = get_timer_hook_id();
	int irq_kbd = get_keyboard_hook_id();
	int irq_mouse = get_mouse_hook_id();

	int ipc_status, r;
	message msg;
	int counter = 0;
	int x = 0, y = 0;
	InGameState st = PLAYING;
	int width, height;
	uint16_t *map = read_xpm(pic_mouse, &width, &height);
	unsigned long old_colors[width * height];
	unsigned int seconds = 0;
	unsigned int type = 0;
	int byte_cnt = 0;
	int mov_counter = 0;
	Movement mov;
	int prev_ldown = 1;
	clear_kbd_data();

	while (puzz_counter < 3)
	{
		Board board;
		int num_pieces = (*difficulty == EASY) ? NUM_PIECES_EZ : NUM_PIECES_HD;
		Bitmap *bitmaps[num_pieces];
		PuzzlePiece pieces[num_pieces];
		int puzz;

		do
		{
			puzz = rand() % 3;
		} while (has_been_played(puzzle_id, puzz, 3));

		puzzle_id[puzz_counter] = puzz;

		init_board(&board, pieces, bitmaps, num_pieces, *bmps, SP, &puzz);

		show_full_image(puzz, *bmps);

		draw_game(board, *bmps, SP);
		shuffle(&board,0);

		draw_xpm(map, width, height, x, y, old_colors);

		while (st == PLAYING && g_kbd_data != CVT_BREAK(ESC_KEY))
		{
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
			{
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status))
			{
				switch (_ENDPOINT_P(msg.m_source))
				{
				case HARDWARE:
					if (msg.NOTIFY_ARG & irq_timer)
					{
						if (counter % 60 == 0)
						{
							check_timer(ARCADE, &seconds, &st);
							draw_timer(bmps, seconds);
						}

						counter++;
					}
					if (msg.NOTIFY_ARG & irq_kbd)
					{
						if (kbd_c_wrapper(&type) == 0)
						{
							mov = cvt_makecode_mov(g_kbd_data, 0);

							erase_xpm(map, width, height, x, y, old_colors);

							if (mov != IGNORE)
							{
								if (check_movement(&board, mov, &mov_counter, 0) == 0)
									draw_mov_counter(bmps, mov_counter);

								if (check_solution(&board) == board.num_pieces)
									st = COMPLETE;
							}

							draw_xpm(map, width, height, x, y, old_colors);
						}
					}

					if (msg.NOTIFY_ARG & irq_mouse)
					{
						if (mouse_wrapper(&byte_cnt) == 0)
						{
							erase_xpm(map, width, height, x, y, old_colors);

							x += get_mouse_X();
							y -= get_mouse_Y();
							check_mouse_boundaries(&x, &y);

							if (checkLDOWN() == 0 && prev_ldown != 0)
							{
								if (check_mouse_movement(&board, x, y, &mov_counter, 0) == 0)
									draw_mov_counter(bmps, mov_counter);

								if (check_solution(&board) == board.num_pieces)
									st = COMPLETE;

								event evt = check_exit(x, y, SP);

								if (evt == TO_MENU)
									return 1;
							}

							prev_ldown = checkLDOWN();

							draw_xpm(map, width, height, x, y, old_colors);
						}
					}

					break;
				default:
					break;
				}
			}
			else
			{
			}
		}

		puzz_counter++;
		st = PLAYING;
	}

	check_highscore(ARCADE, hs,seconds, *difficulty, mov_counter, *bmps);

	erase_xpm(map, width, height, x, y, old_colors);

	return 0;
}

int multi_player_mode(int *difficulty, GameBitmaps *bmps)
{
	int result;

	draw_setup_mp(*bmps);

	if (players_ready())
		return -1;

	draw_entering_game(*bmps);

	sleep(3);

	result = multi_player_game(difficulty, bmps);

	clearRBR();

	return result;
}

void draw_power_ups(unsigned int state, GameBitmaps bmps)
{
	draw_power_up(SHUFFLE, state, bmps);
	draw_power_up(FREEZE, state, bmps);
}

void evaluate_event(Board *board, unsigned char event, unsigned char is_received,
		unsigned int seconds, GameBitmaps bmps, InGameState *st, PowerUpVar *var)
{
	switch (event)
	{
	case PLAYER_WON:
		if (is_received)
		{
			draw_lose(bmps);
			*st = EXIT_G;
		}
		else
		{
			send_data(event);
			draw_win(bmps);
			*st = COMPLETE;
		}
		sleep(3);
		break;
	case FORFEIT:
		if (is_received)
			draw_win_forfeit(bmps);
		else
		{
			send_data(event);
			draw_lose_forfeit(bmps);
		}

		*st = EXIT_G;
		sleep(3);
		break;
	case FREEZE:
		if (is_received)
		{
			var->freeze = ACTIVE;
			var->freeze_time = seconds;
			var->finish_set = ACTIVE;
			var->finish_break = seconds;
		}
		else
		{
			send_data(event);
			var->pup_set = ACTIVE;
			var->pup_break = seconds;
			draw_power_ups(DISABLED, bmps);
		}
		break;
	case SHUFFLE:
		if (is_received)
		{
			shuffle(board,1);
			var->finish_set = ACTIVE;
			var->finish_break = seconds;
		}
		else
		{
			send_data(event);
			var->pup_set = ACTIVE;
			var->pup_break = seconds;
			draw_power_ups(DISABLED, bmps);
		}
		break;
	case FINISHING:
		if (is_received)
		{
			draw_power_ups(ACTIVE, bmps);
			var->pup_set = 2;
		}
		else
		{
			send_data(event);
		}
		break;
	default:
		break;
	}
}

PowerUpVar create_var()
{
	PowerUpVar var;

	var.freeze_time = 0;
	var.freeze = 0;
	var.pup_break = 0;
	var.pup_set = 0;
	var.finish_break = 0;
	var.finish_set = 0;

	return var;
}

int multi_player_game(int *difficulty, GameBitmaps *bmps)
{
	//Variable to choose the same puzzle for both players
	static int puzz = -1;
	puzz++;
	if (puzz == NUM_IMAGES)
		puzz = 0;

	Board board;
	int num_pieces = (*difficulty == EASY) ? NUM_PIECES_EZ : NUM_PIECES_HD;
	Bitmap *bitmaps[num_pieces];
	PuzzlePiece pieces[num_pieces];
	InGameState st = PLAYING;

	/* PREPARE THE GAME */

	init_board(&board, pieces, bitmaps, num_pieces, *bmps, MP, &puzz);
	show_full_image(puzz, *bmps);
	draw_game(board, *bmps, MP);
	shuffle(&board,0);

	PowerUpVar var = create_var();
	unsigned int seconds = 0;

	//On rare occasions, one of the players may forfeit before the other game has finished shuffling
	evaluate_event(&board, g_uart_rbr_data, 1, seconds, *bmps, &st, &var);

	int irq_timer = get_timer_hook_id();
	int irq_kbd = get_keyboard_hook_id();
	int irq_mouse = get_mouse_hook_id();
	int irq_ser_port = get_uart_hook_id();

	int ipc_status, r;
	message msg;
	int x = 0, y = 0;
	int counter = 0;

	int width, height;
	uint16_t *map = read_xpm(pic_mouse, &width, &height);
	unsigned long old_colors[width * height];

	unsigned int type = 0;
	int byte_cnt = 0;
	int mov_counter = 0;
	Movement mov;
	int prev_ldown = 1;
	int correct_pieces = 0;

	draw_xpm(map, width, height, x, y, old_colors);

	draw_power_ups(DISABLED, *bmps);

	while (st == PLAYING)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_timer)
				{
					if (counter % 60 == 0)
					{
						check_timer(MP, &seconds, &st);

						check_power_up_times(&var, seconds);
					}

					draw_timer(bmps, seconds);

					counter++;
				}
				if (msg.NOTIFY_ARG & irq_kbd)
				{
					if (kbd_c_wrapper(&type) == 0 && var.freeze == DISABLED)
					{
						mov = cvt_makecode_mov(g_kbd_data, 0);

						erase_xpm(map, width, height, x, y, old_colors);

						if (mov != IGNORE)
						{
							if (check_movement(&board, mov, &mov_counter, 0) == 0)
								draw_mov_counter(bmps, mov_counter);

							correct_pieces = check_solution(&board);

							if (correct_pieces == board.num_pieces)
								evaluate_event(&board, PLAYER_WON, 0, seconds, *bmps, &st, &var);
							else if (correct_pieces >= (2 * board.num_pieces) / 3 &&
									var.finish_set == 0)
								evaluate_event(&board, FINISHING, 0, seconds, *bmps, &st, &var);
						}

						draw_xpm(map, width, height, x, y, old_colors);
					}
				}
				if (msg.NOTIFY_ARG & irq_mouse)
				{
					if (mouse_wrapper(&byte_cnt) == 0 && var.freeze == DISABLED)
					{
						erase_xpm(map, width, height, x, y, old_colors);

						x += get_mouse_X();
						y -= get_mouse_Y();
						check_mouse_boundaries(&x, &y);

						if (checkLDOWN() == 0 && prev_ldown != 0)
						{
							if (check_mouse_movement(&board, x, y, &mov_counter, 0) == 0)
								draw_mov_counter(bmps, mov_counter);

							correct_pieces = check_solution(&board);

							if (correct_pieces == board.num_pieces)
								evaluate_event(&board, PLAYER_WON, 0, seconds, *bmps, &st, &var);
							else if (correct_pieces >= (2 * board.num_pieces) / 3 &&
									var.finish_set == 0)
								evaluate_event(&board, FINISHING, 0, seconds, *bmps, &st, &var);

							unsigned char check_PUP = check_power_up(x, y);

							if (check_PUP != 0 && var.pup_set == 2)
								evaluate_event(&board, check_PUP, 0, seconds, *bmps, &st, &var);

							event evt = check_exit(x, y, MP);

							if (evt == TO_MENU)
								evaluate_event(&board, FORFEIT, 0, seconds, *bmps, &st, &var);
						}

						prev_ldown = checkLDOWN();

						draw_xpm(map, width, height, x, y, old_colors);
					}
				}
				if (msg.NOTIFY_ARG & irq_ser_port)
				{
					uart_assembly_handler();

					erase_xpm(map, width, height, x, y, old_colors);

					evaluate_event(&board, g_uart_rbr_data, 1, seconds, *bmps, &st, &var);

					draw_xpm(map, width, height, x, y, old_colors);

					clearRBR();
					clear_uart_data();
				}

				break;
			default:
				break;
			}
		}
		else
		{
		}
	}

	clearRBR();
	clear_uart_data();

	return st;
}

int players_ready(GameBitmaps bmps)
{
	unsigned int type = 0;
	int ipc_status, r;
	message msg;

	//booleans to check whether or not the players are ready;
	int local_ready = 1;
	int ser_ready = 1;

	int irq_timer = get_timer_hook_id();
	int irq_kbd = get_keyboard_hook_id();
	int irq_mouse = get_mouse_hook_id();
	int irq_ser_port = get_uart_hook_id();

	if (g_uart_rbr_data == LEAVE)
		clear_uart_data();

	while ((local_ready == 1 || ser_ready == 1) && g_kbd_data != ESC_KEY &&
			g_uart_rbr_data != LEAVE)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_timer)
				{
					clearRBR();
				}
				if (msg.NOTIFY_ARG & irq_kbd)
				{
					if (kbd_c_wrapper(&type) == 0)
					{
						if (g_kbd_data == SPACE_MAKECODE)
						{
							local_ready = 0;
							send_data(READY); //local player is ready;
						}
						else if (g_kbd_data == ESC_KEY)
							send_data(LEAVE); //player decided to leave
					}
				}

				if (msg.NOTIFY_ARG & irq_mouse)
				{
					mouse_handler();
				}

				if (msg.NOTIFY_ARG & irq_ser_port)
				{
					uart_assembly_handler();

					if (g_uart_rbr_data == READY)
						ser_ready = 0;
				}
				break;
			default:
				break;
			}
		}
		else
		{
		}
	}

	clearRBR();

	//If both players are ready
	if (g_kbd_data != ESC_KEY && g_uart_rbr_data != LEAVE)
	{
		clear_uart_data();
		return 0;
	}

	clear_uart_data();
	return 1;
}

void draw_power_up(char power_up, int hi, GameBitmaps bmps)
{
	if (power_up == SHUFFLE && hi)
		draw_bitmap(bmps.shuffle_hi, PUP1X, PUPY, NO_DB);
	else if (power_up == SHUFFLE && !hi)
		draw_bitmap(bmps.shuffle, PUP1X, PUPY, NO_DB);

	else if (power_up == FREEZE && hi)
		draw_bitmap(bmps.freeze_hi, PUP2X, PUPY, NO_DB);
	else
		draw_bitmap(bmps.freeze, PUP2X, PUPY, NO_DB);
}

unsigned char check_power_up(int x, int y)
{
	int xi1, yi, xf1, yf;
	int xi2, xf2;

	xi1 = PUP1X;
	yi = PUPY;
	xf1 = xi1 + PUP1SIZE;
	yf = yi + PUP1SIZE;
	xi2 = PUP2X;
	xf2 = xi2 + PUP2SIZE;

	if (x >= xi1 && x <= xf1 && y >= yi && y <= yf)
		return SHUFFLE;
	else if (x >= xi2 && x <= xf2 && y >= yi && y <= yf)
		return FREEZE;

	return 0;
}

void check_power_up_times(PowerUpVar *var, int seconds)
{
	check_power_up_time(&var->freeze, &var->freeze_time, seconds, FREEZE_TIME);

	check_power_up_time(&var->pup_set, &var->pup_break, seconds, PUP_BREAK);

	check_power_up_time(&var->finish_set, &var->finish_break, seconds, PUP_BREAK);
}

void check_power_up_time(int *powerup_set, int *powerup_set_time, int seconds, const int powerup_time)
{
	if (*powerup_set == ACTIVE && (seconds - *powerup_set_time) == powerup_time)
	{
		*powerup_set = DISABLED;
		*powerup_set_time = 0;
	}
}

void draw_setup_mp(GameBitmaps bmps)
{
	draw_bitmap(bmps.background_main, 0, 0, USE_DB);
	draw_bitmap(bmps.mp_message1, 100, 250, USE_DB);
	draw_bitmap(bmps.leave, 100, 400, USE_DB);

	double_buffer();
}

void draw_entering_game(GameBitmaps bmps)
{
	draw_bitmap(bmps.background_main, 0, 0, USE_DB);
	draw_bitmap(bmps.mp_message2, 100, 350, USE_DB);

	double_buffer();
}

void draw_win_forfeit(GameBitmaps bmps)
{
	draw_bitmap(bmps.background_main, 0, 0, USE_DB);
	draw_bitmap(bmps.won_forfeit, 100, 350, USE_DB);

	double_buffer();
}

void draw_lose_forfeit(GameBitmaps bmps)
{
	draw_bitmap(bmps.background_main, 0, 0, USE_DB);
	draw_bitmap(bmps.lost_forfeit, 100, 350, USE_DB);

	double_buffer();
}

void draw_lose(GameBitmaps bmps)
{
	draw_bitmap(bmps.background_main, 0, 0, USE_DB);
	draw_bitmap(bmps.lost, 100, 350, USE_DB);

	double_buffer();
}

void draw_win(GameBitmaps bmps)
{
	draw_bitmap(bmps.background_main, 0, 0, USE_DB);
	draw_bitmap(bmps.won, 100, 350, USE_DB);

	double_buffer();
}

int has_been_played(int puzzles_id[], int current_puzz, int size)
{
	int i = 0;

	for (; i < size; i++)
		if (puzzles_id[i] == current_puzz)
			return 1;

	return 0;
}

int are_opposed_movements(Movement mov1, Movement mov2)
{
	if ((mov1 == UP && mov2 == DOWN) || (mov1 == DOWN && mov2 == UP))
		return 1;
	else if ((mov1 == LEFT && mov2 == RIGHT) || (mov1 == RIGHT && mov2 == LEFT))
		return 1;
	else
		return 0;
}

void shuffle(Board *board, int mode)
{
	int num_operations;

	switch(mode)
	{
	case 0:
		num_operations = (board->num_pieces == NUM_PIECES_HD) ?
				rand() % (75 - 49) + 50 : rand() % (50 - 24) + 25;
		break;
	case 1:
		num_operations = (board->num_pieces == NUM_PIECES_HD) ?
				rand() % (30 - 19) + 20 : rand() % (20 - 9) + 10;
		break;
	default:
		break;
	}

	int counter = 0;
	int mov_counter = 0;
	int prevMov;

	cpy_buffer();

	//The function has to complete a certain number of switches, and make sure the board is sufficiently shuffled
	while (counter < num_operations || check_solution(board) >= (board->num_pieces) / 3)
	{
		int mov = rand() % 4;

		if (are_opposed_movements(mov, prevMov) == 0 || mov_counter == 0)
		{
			if (check_movement(board, mov, &mov_counter, 1) == 0)
			{
				prevMov = mov;
				counter++;
			}
		}
	}
}

int check_movement(Board *board, Movement mov, int *mov_counter, int animation)
{
	int pos = get_empty_piece(board);
	int side_num_pieces = sqrt(board->num_pieces);

	//If the empty piece is on the first column
	if (pos % side_num_pieces == 0)
	{
		//If it's the first of the column
		if (pos == 0)
		{
			if (mov == LEFT)
				switch_bitmap(board, pos, pos + 1, animation);
			else if (mov == UP)
				switch_bitmap(board, pos, pos + side_num_pieces, animation);
			else
				return 1;
		} //If its the last
		else if (pos == side_num_pieces * (side_num_pieces - 1))
		{
			if (mov == LEFT)
				switch_bitmap(board, pos, pos + 1, animation);
			else if (mov == DOWN)
				switch_bitmap(board, pos, pos - side_num_pieces, animation);
			else
				return 1;
		}
		//If it's between
		else
		{
			if (mov == LEFT)
				switch_bitmap(board, pos, pos + 1, animation);
			else if (mov == DOWN)
				switch_bitmap(board, pos, pos - side_num_pieces, animation);
			else if (mov == UP)
				switch_bitmap(board, pos, pos + side_num_pieces, animation);
			else
				return 1;
		}
	}
	//If the empty piece is on the last column
	else if (pos % side_num_pieces == (side_num_pieces - 1))
	{
		//If it's the first of the column
		if (pos == side_num_pieces - 1)
		{
			if (mov == RIGHT)
				switch_bitmap(board, pos, pos - 1, animation);
			else if (mov == UP)
				switch_bitmap(board, pos, pos + side_num_pieces, animation);
			else
				return 1;
		} //If its the last
		else if (pos == side_num_pieces * side_num_pieces - 1)
		{
			if (mov == RIGHT)
				switch_bitmap(board, pos, pos - 1, animation);
			else if (mov == DOWN)
				switch_bitmap(board, pos, pos - side_num_pieces, animation);
			else
				return 1;
		}
		//If its between
		else
		{
			if (mov == RIGHT)
				switch_bitmap(board, pos, pos - 1, animation);
			else if (mov == DOWN)
				switch_bitmap(board, pos, pos - side_num_pieces, animation);
			else if (mov == UP)
				switch_bitmap(board, pos, pos + side_num_pieces, animation);
			else
				return 1;
		}
	} //If its on the first row, in the middle elements
	else if (pos < side_num_pieces)
	{
		if (mov == RIGHT)
			switch_bitmap(board, pos, pos - 1, animation);
		else if (mov == LEFT)
			switch_bitmap(board, pos, pos + 1, animation);
		else if (mov == UP)
			switch_bitmap(board, pos, pos + side_num_pieces, animation);
		else
			return 1;
	}
	//If its on the last row, in the middle elements
	else if (pos > side_num_pieces * (side_num_pieces - 1))
	{
		if (mov == RIGHT)
			switch_bitmap(board, pos, pos - 1, animation);
		else if (mov == LEFT)
			switch_bitmap(board, pos, pos + 1, animation);
		else if (mov == DOWN)
			switch_bitmap(board, pos, pos - side_num_pieces, animation);
		else
			return 1;
	}
	//If it's in the middle
	else
	{
		if (mov == RIGHT)
			switch_bitmap(board, pos, pos - 1, animation);
		else if (mov == LEFT)
			switch_bitmap(board, pos, pos + 1, animation);
		else if (mov == DOWN)
			switch_bitmap(board, pos, pos - side_num_pieces, animation);
		else if (mov == UP)
			switch_bitmap(board, pos, pos + side_num_pieces, animation);
		else
			return 1;
	}

	(*mov_counter)++;
	return 0;
}

int check_solution(Board *board)
{
	int counter = 0;

	int i = 0;
	for (; i < board->num_pieces; i++)
		if (board->pieces[i].bmp.position == i)
			counter++;

	return counter;
}

int get_empty_piece(Board *board)
{
	int i = 0;
	for (; i < board->num_pieces; i++)
		if (board->pieces[i].bmp.position == board->num_pieces - 1)
			return i;

	return 1;
}

int get_piece(Board *board, int x, int y)
{
	int i = 0;
	for (; i < board->num_pieces; i++)
	{
		int xi = board->pieces[i].x_coord;
		int yi = board->pieces[i].y_coord;
		int xf = xi + board->pieces[i].width;
		int yf = yi + board->pieces[i].width;

		if (x >= xi && x <= xf && y >= yi && y <= yf)
			return i;
	}

	return -1;
}

void switch_bitmap(Board *board, unsigned int pos1, unsigned int pos2, int animation)
{
	PuzzleBitmap temp1 = board->pieces[pos1].bmp; //empty piece
	PuzzleBitmap temp2 = board->pieces[pos2].bmp; //the other piece

	board->pieces[pos2].bmp = temp1;
	board->pieces[pos1].bmp = temp2;

	if (animation)
		animate_pieces(board, pos1, pos2);
	else
	{
		draw_bitmap(board->pieces[pos1].bmp.bitmap, board->pieces[pos1].x_coord,
				board->pieces[pos1].y_coord, NO_DB);

		draw_bitmap(board->pieces[pos2].bmp.bitmap, board->pieces[pos2].x_coord,
				board->pieces[pos2].y_coord, NO_DB);
	}
}

void animate_pieces(Board *board, unsigned int pos1, unsigned int pos2)
{
	int xspeed = 0, yspeed = 0;
	int dif = pos2 - pos1;
	int side_pieces = (int)sqrt(board->num_pieces);
	int speed = (side_pieces == 4) ? 33 : 44;

	if (dif == side_pieces)
		yspeed = -speed;
	else if (dif == -side_pieces)
		yspeed = speed;
	else if (dif == 1)
		xspeed = -speed;
	else if (dif == -1)
		xspeed = speed;

	Sprite *sp = create_sprite(board->pieces[pos1].bmp.bitmap,
			board->pieces[pos2].bmp.bitmap, board->pieces[pos2].x_coord,
			board->pieces[pos2].y_coord, xspeed, yspeed);
	int xf = board->pieces[pos1].x_coord;
	int yf = board->pieces[pos1].y_coord;
	int frames = 4;
	int counter = 0;
	int ipc_status, r;
	message msg;

	int irq_timer = get_timer_hook_id();
	int irq_kbd = get_keyboard_hook_id();
	int irq_mouse = get_mouse_hook_id();
	int irq_serial = get_uart_hook_id();

	while (sp->x != xf || sp->y != yf)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:

				if (msg.NOTIFY_ARG & irq_timer)
				{
					counter++;

					//Draw the xpm in a new position when the selected number of interrutps has been reached
					if ((counter % frames) == 0)
					{
						draw_bitmap(sp->empty, sp->x, sp->y, USE_DB);

						//Vertical movement
						if (sp->xspeed == 0)
							sp->y += sp->yspeed;
						//Horizontal movement
						else
							sp->x += sp->xspeed;

						draw_bitmap(sp->bitmap, sp->x, sp->y, USE_DB);
					}

					double_buffer();
				}
				if (msg.NOTIFY_ARG & irq_kbd)
				{
					kbd_c_handler();
				}
				if (msg.NOTIFY_ARG & irq_mouse)
				{
					mouse_handler();
				}
				if (msg.NOTIFY_ARG & irq_serial)
				{
					uart_assembly_handler();
				}
				break;
			default:
				break;
			}
		}
		else
		{
		}
	}
}

Movement cvt_makecode_mov(unsigned char makecode, int is_at_menu)
{
	if (makecode == A_MAKECODE && is_at_menu == 0)
		return LEFT;
	else if (makecode == D_MAKECODE && is_at_menu == 0)
		return RIGHT;
	else if (makecode == S_MAKECODE)
		return DOWN;
	else if (makecode == W_MAKECODE)
		return UP;
	else if (makecode == ENTER_MAKECODE && is_at_menu == 1)
		return ENTER;
	else
		return IGNORE;
}

int check_mouse_movement(Board *board, int x, int y, int *mov_counter, int animation)
{
	int pos_empty = get_empty_piece(board);
	int pos_piece = get_piece(board, x, y);
	int side_num_pieces = sqrt(board->num_pieces);

	if (pos_empty == pos_piece || pos_piece == -1)
		return -1;

	//If the empty piece is on the first column
	if (pos_empty % side_num_pieces == 0)
	{
		//If it's the first of the column
		if (pos_empty == 0)
		{
			if (!(pos_piece == 1 || pos_piece == side_num_pieces))
				return 1;
		} //If its the last
		else if (pos_empty == side_num_pieces * (side_num_pieces - 1))
		{
			if (!(pos_piece == pos_empty + 1 || pos_piece == pos_empty - side_num_pieces))
				return 1;
		}
		//If it's between
		else
		{
			if (!(pos_piece == pos_empty + 1 || pos_piece == pos_empty - side_num_pieces ||
					pos_piece == pos_empty + side_num_pieces))
				return 1;
		}
	}
	//If the empty piece is on the last column
	else if (pos_empty % side_num_pieces == (side_num_pieces - 1))
	{
		//If it's the first of the column
		if (pos_empty == side_num_pieces - 1)
		{
			if (!(pos_piece == pos_empty - 1 || pos_piece == pos_empty + side_num_pieces))
				return 1;
		} //If its the last
		else if (pos_empty == side_num_pieces * side_num_pieces - 1)
		{
			if (!(pos_piece == pos_empty - side_num_pieces || pos_piece == pos_empty - 1))
				return 1;
		}
		//If its between
		else
		{
			if (!(pos_piece == pos_empty - 1 ||
					pos_piece == pos_empty - side_num_pieces ||
					pos_piece == pos_empty + side_num_pieces))
				return 1;
		}
	} //If its on the first row, in the middle elements
	else if (pos_empty < side_num_pieces)
	{
		if (!(pos_piece == pos_empty - 1 || pos_piece == pos_empty + 1 ||
				pos_piece == pos_empty + side_num_pieces))
			return 1;
	}
	//If its on the last row, in the middle elements
	else if (pos_empty > side_num_pieces * (side_num_pieces - 1))
	{
		if (!(pos_piece == pos_empty - 1 || pos_piece == pos_empty + 1 ||
				pos_piece == pos_empty - side_num_pieces))
			return 1;
	}
	//If it's in the middle
	else
	{
		if (!(pos_piece == pos_empty - 1 || pos_piece == pos_empty + 1 ||
				pos_piece == pos_empty + side_num_pieces ||
				pos_piece == pos_empty - side_num_pieces))
			return 1;
	}

	switch_bitmap(board, pos_empty, pos_piece, animation);
	(*mov_counter)++;
	return 0;
}

void update_time(int puzzle_id, int *time, GameBitmaps bmps)
{
	draw_pre_game(puzzle_id, bmps);

	switch (*time)
	{
	case 3:
		draw_bitmap(bmps.number3, TIMER1_X, TIMER1_Y, USE_DB);
		break;
	case 2:
		draw_bitmap(bmps.number2, TIMER1_X, TIMER1_Y, USE_DB);
		break;
	case 1:
		draw_bitmap(bmps.number1, TIMER1_X, TIMER1_Y, USE_DB);
		break;
	default:
		break;
	}

	double_buffer();
}

void draw_timer(GameBitmaps *bmps, unsigned int timeInSeconds)
{
	unsigned int minutes = timeInSeconds / 60;
	unsigned int seconds = timeInSeconds % 60;
	unsigned int min1 = 0, min2 = 0, sec1 = 0, sec2 = 0;

	//Retrieve the digits
	min1 = minutes / 10;
	min2 = minutes % 10;
	sec1 = seconds / 10;
	sec2 = seconds % 10;

	Bitmap *min1bmp = bmps->numbers[min1];
	Bitmap *min2bmp = bmps->numbers[min2];
	Bitmap *twoPointsbmp = bmps->numbers[10];
	Bitmap *sec1bmp = bmps->numbers[sec1];
	Bitmap *sec2bmp = bmps->numbers[sec2];

	draw_bitmap(bmps->timer_bg, 130, 220, NO_DB);
	draw_bitmap(min1bmp, GAME_TIMER_MIN1_X, GAME_TIMER_Y, NO_DB);
	draw_bitmap(min2bmp, GAME_TIMER_MIN2_X, GAME_TIMER_Y, NO_DB);
	draw_bitmap(twoPointsbmp, GAME_TIMER_SEP_X, GAME_TIMER_Y, NO_DB);
	draw_bitmap(sec1bmp, GAME_TIMER_SEC1_X, GAME_TIMER_Y, NO_DB);
	draw_bitmap(sec2bmp, GAME_TIMER_SEC2_X, GAME_TIMER_Y, NO_DB);
}

void draw_mov_counter(GameBitmaps *bmps, int mov_counter)
{
	unsigned int n1 = 0, n2 = 0, n3 = 0;

	n1 = mov_counter / 100;
	n2 = (mov_counter % 100) / 10;
	n3 = (mov_counter % 100) % 10;

	Bitmap *n1bmp = bmps->numbers[n1];
	Bitmap *n2bmp = bmps->numbers[n2];
	Bitmap *n3bmp = bmps->numbers[n3];

	draw_bitmap(bmps->timer_bg, 130, 410, NO_DB);
	draw_bitmap(n1bmp, MOV_COUNTER_N1_X, MOV_COUNTER_Y, NO_DB);
	draw_bitmap(n2bmp, MOV_COUNTER_N2_X, MOV_COUNTER_Y, NO_DB);
	draw_bitmap(n3bmp, MOV_COUNTER_N3_X, MOV_COUNTER_Y, NO_DB);
}

void draw_pre_game(int puzzle_id, GameBitmaps bmps)
{
	draw_bitmap(bmps.background, 0, 0, USE_DB);
	draw_bitmap(bmps.message1, 100, 20, USE_DB);

	switch (puzzle_id)
	{
	case PANDA:
		draw_bitmap(bmps.panda, IMG_X, IMG_Y, USE_DB);
		break;
	case MOTORBIKE:
		draw_bitmap(bmps.motorbike, IMG_X, IMG_Y, USE_DB);
		break;
	case LANDSCAPE:
		draw_bitmap(bmps.landscape, IMG_X, IMG_Y, USE_DB);
		break;
	default:
		break;
	}
}

void show_full_image(int puzzle_id, GameBitmaps bmps)
{
	draw_pre_game(puzzle_id, bmps);
	double_buffer();

	//Stores bit mask in irq_timer and subscribes interrupts
	int irq_timer = get_timer_hook_id();
	int irq_kbd = get_keyboard_hook_id();
	int irq_mouse = get_mouse_hook_id();

	int counter = 0;		  //Counter counts the number of timer interrupts
	int time = FULL_IMG_TIME; //time measures the passage of time
	int ipc_status, r;
	message msg;

	while (time >= 0)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_timer)
				{
					if (counter % 60 == 0)
					{
						update_time(puzzle_id, &time, bmps);
						time--; //Decrements time every second
					}
					counter++;
				}
				if (msg.NOTIFY_ARG & irq_kbd)
				{
					kbd_c_handler();
				}
				if (msg.NOTIFY_ARG & irq_mouse)
				{
					mouse_handler();
				}
				break;
			default:
				break;
			}
		}
		else
		{
		}
	}
}

void draw_game(Board board, GameBitmaps bmps, unsigned int mode)
{
	switch (mode)
	{
	case SP:
		draw_bitmap(bmps.background_sp, 0, 0, NO_DB);
		break;
	case MP:
		draw_bitmap(bmps.background_mp, 0, 0, NO_DB);
		break;
	default:
		return;
	}

	draw_board(board);
}

void draw_board(Board board)
{
	draw_bitmap(board.background, board.x_init, board.y_init, NO_DB);

	int i = 0;
	for (; i < board.num_pieces; i++)
		draw_bitmap(board.pieces[i].bmp.bitmap, board.pieces[i].x_coord,
				board.pieces[i].y_coord, NO_DB);
}

char cvt_makecode_char(unsigned char makecode)
{
	if (makecode == 0x10)
		return (char)113;
	else if (makecode == 0x11)
		return (char)119;
	else if (makecode == 0x12)
		return (char)101;
	else if (makecode == 0x13)
		return (char)114;
	else if (makecode == 0x14)
		return (char)116;
	else if (makecode == 0x15)
		return (char)121;
	else if (makecode == 0x16)
		return (char)117;
	else if (makecode == 0x17)
		return (char)105;
	else if (makecode == 0x18)
		return (char)111;
	else if (makecode == 0x19)
		return (char)112;
	else if (makecode == 0x1E)
		return (char)97;
	else if (makecode == 0x1F)
		return (char)115;
	else if (makecode == 0x20)
		return (char)100;
	else if (makecode == 0x21)
		return (char)102;
	else if (makecode == 0x22)
		return (char)103;
	else if (makecode == 0x23)
		return (char)104;
	else if (makecode == 0x24)
		return (char)106;
	else if (makecode == 0x25)
		return (char)107;
	else if (makecode == 0x26)
		return (char)108;
	else if (makecode == 0x2C)
		return (char)122;
	else if (makecode == 0x2D)
		return (char)120;
	else if (makecode == 0x2E)
		return (char)99;
	else if (makecode == 0x2F)
		return (char)118;
	else if (makecode == 0x30)
		return (char)98;
	else if (makecode == 0x31)
		return (char)110;
	else if (makecode == 0x32)
		return (char)109;
	else if (makecode == 0x0E)
		return (char)8;
	else
		return (char)0;
}

Bitmap *cvt_char_bitmap(char character, Bitmap *letters[])
{
	unsigned int i = (unsigned int)character - 97;

	return letters[i];
}
