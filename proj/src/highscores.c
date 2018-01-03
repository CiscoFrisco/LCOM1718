#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "highscores.h"
#include "game.h"
#include "rtc.h"
#include "mouse.h"
#include "keyboard.h"
#include "i8042.h"
#include "video_gr.h"


extern unsigned char g_kbd_data;


Time create_time(unsigned int sec, unsigned int min, unsigned int hour,
		unsigned int day, unsigned int month, unsigned int year)
{
	Time time;

	time.day = day;
	time.month = month;
	time.year = year;
	time.seconds = sec;
	time.minutes = min;
	time.hours = hour;

	return time;
}

Highscore create_highscore(Time time, unsigned int score, char* name,
		unsigned int type)
{
	Highscore highscore;

	highscore.time = time;
	highscore.score = score;
	strcpy(highscore.name, name);
	highscore.name_size = strlen(name);
	highscore.type = type;

	return highscore;
}

Highscores create_highscores()
{
	Highscores highscores;

	highscores.regular = malloc(HS_MAX_SIZE * sizeof(Highscore));
	highscores.timetrial = malloc(HS_MAX_SIZE * sizeof(Highscore));
	highscores.arcade = malloc(HS_MAX_SIZE * sizeof(Highscore));
	highscores.regular_size = 0;
	highscores.timetrial_size = 0;
	highscores.arcade_size = 0;

	return highscores;
}

void get_file_path(char* path, unsigned int type)
{
	switch(type)
	{
	case REGULAR:
		strcpy(path, "/home/lcom/slideit_files/text_files/hs_reg.txt");
		break;
	case TIMETRIAL:
		strcpy(path, "/home/lcom/slideit_files/text_files/hs_tt.txt");
		break;
	case ARCADE:
		strcpy(path, "/home/lcom/slideit_files/text_files/hs_arc.txt");
		break;
	default:
		break;
	}
}

int read_highscore_file(Highscore hs[], unsigned int *size, unsigned int type)
{
	FILE* hsPtr;
	char name[NAME_MAX_LENGTH];
	unsigned int seconds, minutes, hours, day, month, year;
	char sep;
	unsigned int counter = 0;
	unsigned int score;
	char* path = malloc(FILE_PATH_MAX_SIZE);

	get_file_path(path, type);

	if((hsPtr = fopen(path, "r")) == NULL)
	{
		printf("File could not be opened\n");
		return 1;
	}
	else
	{
		while(fscanf(hsPtr, "%s %d%c%d%c%d %d%c%d%c%d %d", name, &hours, &sep,
				&minutes, &sep, &seconds, &day, &sep, &month, &sep, &year,
				&score) != EOF && counter < HS_MAX_SIZE)
		{
			Highscore inf = create_highscore(create_time(seconds, minutes,
					hours, day, month, year), score, name, type);

			hs[counter] = inf;
			counter++;
		}

		*size = counter;
	}

	fclose(hsPtr);
	return 0;
}

int write_highscore_file(Highscore hs[], unsigned int arrSize,
		unsigned int type)
{
	FILE* hsPtr;
	char* path = malloc(FILE_PATH_MAX_SIZE);

	get_file_path(path, type);

	if((hsPtr = fopen(path, "w")) == NULL)
	{
		printf("File could not be opened\n");
		return 1;
	}
	else
	{
		unsigned int counter = 0;
		while(counter < arrSize)
		{
			Highscore h = hs[counter];

			fprintf(hsPtr, "%s %d%c%d%c%d %d%c%d%c%d %d\n", h.name, h.time.hours,
					FILE_SEPARATOR, h.time.minutes, FILE_SEPARATOR,
					h.time.seconds, h.time.day, FILE_SEPARATOR, h.time.month,
					FILE_SEPARATOR, h.time.year, h.score);

			counter++;
		}
	}

	return 0;
}

void sort_highscores(Highscore hs[], unsigned int size)
{
	unsigned int p = 1;
	for (; p < size; p++)
	{
		Highscore tmp = hs[p];

		int j;
		for (j = p; j > 0 && tmp.score > hs[j - 1].score; j--)
			hs[j] = hs[j - 1];

		hs[j] = tmp;
	}
}

void insert_highscore(Highscore inf, Highscore hs[], unsigned int *size)
{
	//Full size hasn't been reached, so the new highscore can safely be put directly in the array
	if(*size < HS_MAX_SIZE)
	{
		hs[*size] = inf;
		(*size)++;
		sort_highscores(hs, *size);
	}
	//The array is full, so the lowest element has to be deleted
	else
	{
		Highscore temp[*size + 1];
		memcpy(temp, hs, (*size) * sizeof(Highscore));
		temp[*size] = inf;							 	 //Add the new highscore to the end of the temp array
		sort_highscores(temp, *size + 1); 				 //Sort it
		memcpy(hs,temp,(*size) * sizeof(Highscore)); 	 //Store the 5 highest scores on the array
	}
}

void insert_in_highscores(Highscore inf, Highscores *hs, unsigned int mode)
{
	switch(mode)
	{
	case REGULAR:
		insert_highscore(inf, hs->regular, &hs->regular_size);
		break;
	case TIMETRIAL:
		insert_highscore(inf, hs->timetrial, &hs->timetrial_size);
	break;
	case ARCADE:
		insert_highscore(inf, hs->arcade, &hs->arcade_size);
		break;
	default:
		break;
	}
}

unsigned int calc_score(unsigned int type, unsigned int time,
		unsigned int difficulty, unsigned int nMovements)
{
	unsigned int diffPoints = (difficulty == EASY) ? 500 : 1000;
	unsigned int timePoints = (type == REGULAR) ? 1000 / time : time;
	unsigned int movementPoints = 1000 / nMovements;

	return diffPoints + timePoints + movementPoints;
}

int check_score(Highscore hs[], unsigned int size, unsigned int score)
{
	//If the size hasn't reached it's maximum, then a new highscore can be safely added
	if(size < HS_MAX_SIZE)
		return 1;

	unsigned int i = 0;

	//If not, then we have to check if there are lower scores on the array
	for(;i < size; i++)
		if(score > hs[i].score)
			return 1;

	return 0;

}

Bitmap* cvt_number_bitmap(int num, Bitmap* numbers[])
{
	return numbers[num];
}

void get_highscores(unsigned int type, Highscores hs, Highscore* inf,
		unsigned int *size)
{
	switch(type)
	{
	case REGULAR:
		*size = hs.regular_size;
		memcpy(inf, hs.regular, (*size)*(sizeof(Highscore)));
		break;
	case TIMETRIAL:
		*size = hs.timetrial_size;
		memcpy(inf, hs.timetrial, (*size)*(sizeof(Highscore)));
		break;
	case ARCADE:
		*size = hs.arcade_size;
		memcpy(inf, hs.arcade, (*size)*(sizeof(Highscore)));
		break;
	default:
		break;
	}
}

void draw_highscores(Highscores hs, unsigned int type, Bitmap* letters[],
		Bitmap* numbers[])
{

	int i = 0;
	unsigned int size;
	Highscore* inf = malloc(HS_MAX_SIZE*sizeof(Highscore));
	int x = 50, x1 = 50;
	int y = 300;

	get_highscores(type, hs, inf, &size);

	for(; i < size; i++)
	{
		int j = 0;
		//Print the name
		for(; j < inf[i].name_size; j++)
		{
			Bitmap* bmp = cvt_char_bitmap(inf[i].name[j], letters);
			draw_bitmap(bmp, x1, y, NO_DB);
			x1+=bmp->bitmapInfoHeader.width + 2;
		}


		x1 = 250;
		//Print the score
		draw_number(inf[i].score, &x1, &y, numbers);

		x1 = 400;
		//Print the date that the game was played
		draw_time(inf[i].time, x1 + 20, y, numbers);

		y += 50;
		x1 = x;
	}
}

void draw_number(int number,int *x, int *y,Bitmap* numbers[])
{
	int num_digits = (number > 0) ? floor(log10(number)) + 1 : 1;

	//If the number has only one digit, then print a 0 behind
	if(num_digits == 1)
	{
		draw_bitmap(numbers[0], *x, *y, NO_DB);
		(*x) += 30;
	}

	for(; num_digits>0;num_digits--)
	{
		int digit = number / ((int) pow(10, num_digits - 1));
		number = number % (int) pow(10, num_digits - 1);

		draw_bitmap(cvt_number_bitmap(digit, numbers), *x, *y, NO_DB);

		(*x) += 30;
	}
}

void draw_time(Time time, int x, int y, Bitmap* numbers[])
{
	draw_number(time.hours, &x, &y, numbers);
	draw_bitmap(numbers[TWO_POINTS], x, y, NO_DB);
	x += 30;

	draw_number(time.minutes, &x, &y, numbers);
	draw_bitmap(numbers[TWO_POINTS], x, y, NO_DB);
	x += 30;

	draw_number(time.seconds, &x, &y, numbers);

	x = 750;
	draw_number(time.day, &x, &y, numbers);
	draw_bitmap(numbers[SLASH], x, y, NO_DB);
	x += 30;

	draw_number(time.month, &x, &y, numbers);
	draw_bitmap(numbers[SLASH], x, y, NO_DB);
	x += 30;

	draw_number(time.year, &x, &y, numbers);
}

int write_highscores(Highscores hi)
{
	if(write_highscore_file(hi.regular, hi.regular_size, REGULAR) != 0)
		return 1;

	if(write_highscore_file(hi.timetrial, hi.timetrial_size, TIMETRIAL) != 0)
		return 1;

	if(write_highscore_file(hi.arcade, hi.arcade_size, ARCADE) != 0)
		return 1;

	return 0;
}

int read_highscores(Highscores *hi)
{
	if(read_highscore_file(hi->regular, &hi->regular_size, REGULAR) != 0)
		return 1;

	if(read_highscore_file(hi->timetrial, &hi->timetrial_size, TIMETRIAL) != 0)
		return 1;

	if(read_highscore_file(hi->arcade, &hi->arcade_size, ARCADE) != 0)
		return 1;

	return 0;
}

int check_highscore(unsigned int mode, Highscores *hs, unsigned int seconds, unsigned int difficulty, unsigned int mov_counter,
		GameBitmaps bmps)
{
	unsigned int size;
	Highscore* inf = malloc(HS_MAX_SIZE * sizeof(Highscore));
	int score = calc_score(mode, seconds, difficulty, mov_counter);
	get_highscores(mode, *hs, inf, &size);

	if(check_score(inf, size, score) == 1)
	{
		draw_bitmap(bmps.background, 0, 0, NO_DB);
		draw_bitmap(bmps.congratulations, 100, 50, NO_DB);


		int irq_kbd = get_keyboard_hook_id();
		int irq_mouse = get_mouse_hook_id();

		int ipc_status, r;
		message msg;
		unsigned int type = 0;
		int in_count = 0;
		char name[MAX_INPUT_SIZE];

		while(g_kbd_data != CVT_BREAK(ENTER_MAKECODE) || in_count >= MAX_INPUT_SIZE)
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
					if (msg.NOTIFY_ARG & irq_kbd)
					{
						if(kbd_c_wrapper(&type)==0)
						{
							char c = cvt_makecode_char(g_kbd_data);

							//Check if the char is valid
							if((c >= (char) A && c<= (char) Z) || c == (char) BACKSPACE)
							{
								//Add the char to the name string, or delete the last one
								if(c != (char) BACKSPACE)
								{
									if(in_count < MAX_INPUT_SIZE)
									{
										name[in_count] = c;
										in_count++;
									}

								}
								else if(in_count > 0)
								{
									in_count--;
									terminate_string(name, in_count, MAX_INPUT_SIZE);
								}

								print_name(name, in_count, bmps);
							}
						}
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
			else {}
		}

		terminate_string(name, in_count, MAX_INPUT_SIZE);
		clear_kbd_data();

		//Create a new Highscore object and add it to its correspondent array
		Highscore new_hs = create_highscore(read_time(), score, name, mode);
		insert_in_highscores(new_hs, hs, mode);

		return 0;
	}
	else
		return 1;
}




void print_name(char* name, unsigned int size, GameBitmaps bmps)
{
	unsigned int i = 0;
	unsigned int x = 300, y = 300;

	draw_bitmap(bmps.background, 0, 0, USE_DB);
	draw_bitmap(bmps.congratulations, 100, 50, USE_DB);

	//Print each letter
	for(; i < size; i++)
	{
		Bitmap* letter = cvt_char_bitmap(name[i], bmps.letters);
		draw_bitmap(letter, x, y, USE_DB);
		x+=letter->bitmapInfoHeader.width + 2;
	}

	double_buffer();
}

int terminate_string(char* string, unsigned int index, unsigned int size)
{
	if(index < size)
	{
		string[index] = '\0';
		return 0;
	}

	return -1;
}
