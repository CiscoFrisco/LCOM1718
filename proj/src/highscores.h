#ifndef _PROJ_HIGHSCORES_H
#define _PROJ_HIGHSCORES_H

#include "bitmap.h"
#include "init_game.h"

/** @defgroup highscores highscores
 * @{
 *
 * Functions, constants and structures related to Highscores
 */

/* CONSTANTS */

#define HS_MAX_SIZE 				5 				/**< @brief Maximum size of an array of highscores */
#define NAME_MAX_LENGTH 			10 				/**< @brief Maximum length of a player name's string */
#define FILE_SEPARATOR 				'-'				/**< @brief Separator between date elements on the files  */
#define FILE_PATH_MAX_SIZE  		45      		/**< @brief Maximum size of a file path string */
#define EASY 						0				/**< @brief Id for easy difficulty */
#define REGULAR 					0				/**< @brief Regular gamemode id */
#define TIMETRIAL 					1				/**< @brief Timetrial gamemode id */
#define ARCADE 						2				/**< @brief Arcade gamemode id */
#define TWO_POINTS 					10				/**< @brief Index on the numbers array of two points bitmap */
#define SLASH               		11      		/**< @brief Index on the numbers array of slash bitmap */
#define NO_DB						0       		/**< @brief Double buffer constant */

/* USEFUL STRUCTURES */

/** @name Time */
/**@{
 *
 * Struct to represent time
 */

typedef struct
{
	unsigned int 	day;
	unsigned int 	month;
	unsigned int 	year;
	unsigned int 	seconds;
	unsigned int 	minutes;
	unsigned int 	hours;
} Time;

/** @} end of Time*/

/** @name Highscore */
/**@{
 *
 * Struct to hold information about a Highscore.
 */

typedef struct
{
	Time 			time; 							/**< @brief Time of the game */
	unsigned int 	score; 							/**< @brief Score */
	char 			name[NAME_MAX_LENGTH]; 			/**< @brief Name of the player */
	unsigned int 	name_size; 						/**< @brief Length of the player's name */
	unsigned int 	type;							/**< @brief Type of the played game */
} Highscore;

/** @} end of Highscore*/


/** @name Highscores */
/**@{
 *
 * Struct to store highscores of different game modes
 */

typedef struct{
	unsigned int 	regular_size; 					/**< @brief Size of the regular array */
	unsigned int 	timetrial_size; 				/**< @brief Size of the timetrial array */
	unsigned int 	arcade_size; 					/**< @brief Size of the arcade array */
	Highscore* 		regular; 						/**< @brief Array of highscores from regular games */
	Highscore* 		timetrial; 						/**< @brief Array of highscores from timetrial games */
	Highscore* 		arcade; 						/**< @brief Array of highscores from arcade games */
} Highscores;

/** @} end of Highscores*/

/* FUNCTIONS */

/**
 * @brief Gets an array of Highscores from a specific game mode
 *
 * According to the type argument, modifies inf and size arguments to hold
 * information about highscores from that type.
 *
 * @param type type of game which highscores one wishes to retrieve
 * @param hs Holds array with highscores info
 * @param inf array that will be initialized with information contained on hs
 * @param size size of the array that's going to be retrieved
 */
void get_highscores(unsigned int type, Highscores hs, Highscore* inf,
		unsigned int *size);

/**
 * @brief Creates a struct that will hold information about highscores on all game modes
 *
 * @return A structure to hold info on highscores
 */
Highscores create_highscores();

/**
 * @brief Creates a struct that holds information of the score a player achieve on some game
 *
 * @param t time when the game took place
 * @param score score the player achieved
 * @param name the name of the player
 * @param type the type of the game
 * @return A structure to hold info on highscores
 */
Highscore create_highscore(Time t, unsigned int score, char* name,
		unsigned int type);

/**
 * @brief Creates a struct that holds information of a determined time
 *
 * @param sec seconds
 * @param min minutes
 * @param hour hours
 * @param day day
 * @param month month
 * @param year year
 * @return A structure to hold info on time
 */
Time create_time(unsigned int sec, unsigned int min, unsigned int hour,
		unsigned int day, unsigned int month, unsigned int year);

/**
 * @brief Calculates the score a player achieved on a game
 *
 * Using a simple formula, calculates the score of a player using the game mode, difficulty, number of movements, and time needed to complete the game.
 *
 * @param type the game mode
 * @param time amount of seconds needed to finish the game
 * @param difficulty difficulty of the game (depends on board-size)
 * @param nMovements number of movements made by the player
 * @return the score of the player
 */
unsigned int calc_score(unsigned int type, unsigned int time,
		unsigned int difficulty, unsigned int nMovements);

/**
 * @brief Inserts a new Highscore object into its respective array
 *
 * Inserts an Highscore object passed as an argument into it's respective array, by checking the size of that array and, in case
 * it's already full, removing the element with lowest score. Also sorts the array.
 *
 * @param inf The object to be inserted in its respective array
 * @param hs The array of Highscore objects that will be modified
 * @param size The size of the hs array
 */
void insert_highscore(Highscore inf, Highscore hs[], unsigned int *size);

/**
 * @brief Inserts a new Highscore object into its respective array
 * 
 * Evaluates the mode argument and calls insert_highscore() in order to
 * add a new element to its respective array
 * 
 * @param inf The object to be inserted in its respective array
 * @param hs The Highscores object that will be modified
 * @param mode Indicates the array that will be modified
 */ 
void insert_in_highscores(Highscore inf, Highscores *hs, unsigned int mode);

/**
 * @brief Sorts an array of highscores by descending order
 *
 * Sorts and array of highscores, using the Insersion Sort algorithm, by highest to lowest score.
 *
 * @param hs The array of Highscore objects to sort
 * @param size The size of the hs array
 */
void sort_highscores(Highscore hs[], unsigned int size);

/**
 *  @brief Writes the highscores relative to a certain gamemode to its respective file
 *
 *	Loads the path of the file to be modified and writes the highscores of a certain game mode
 *	in a formatted way.
 *
 *  @param hs The array which information will be written to a file
 *  @param arrSize The size of the hs array
 *  @param type The type of game
 *  @return 0 if successful, 1 if not
 */
int write_highscore_file(Highscore hs[], unsigned int arrSize, unsigned int type);

/**
 * @brief Writes all of the Highscores info on their respective files.
 *
 * @param hi A structure that holds all the info related to Highscores
 * @return 0 if successful, 1 if not
 */
int write_highscores(Highscores hi);

/**
 * @brief Reads all of the Highscores info from their respective files.
 *
 * @param hi A structure that will hold all the info related to Highscores
 * @return 0 if successful, 1 if not
 */
int read_highscores(Highscores *hi);

/**
 *  @brief Reads the highscores relative to a certain game mode from its respective file
 *
 *	Loads the path of the file to be read and reads the highscores of a certain game mode
 *	to the hs array.
 *
 *  @param hs The array to be initialized with previously store Highscores information
 *  @param size The size of the hs array
 *  @param type The type of game
 *  @return 0 if successful, 1 if not
 */
int read_highscore_file(Highscore hs[], unsigned int *size, unsigned int type);

/**
 *  @brief Checks if it's possible to insert a new Highscore object into its respective array
 *
 *	Checks the size of the hs array to see if it's possible to insert a new object.
 *	In case it's full, sees if there's any lower rated scores.
 *
 *  @param hs the array which will be checked
 *  @param size the size of the hs array
 *  @param score the score that will be checked
 *
 *  @return 1 if possible, 0 if not
 */
int check_score(Highscore hs[], unsigned int size, unsigned int score);

/**
 * @brief Draws highscores of a given type on the screen
 *
 * Prints the name, score and date of all games stored as highscores relative
 * to a given game mode.
 *
 * @param hs the structure that holds information on all types of highscores
 * @param type the type of the game
 * @param letters an array of bitmaps of letters
 * @param numbers an array of bitmaps of numbers
 */
void draw_highscores(Highscores hs, unsigned int type, Bitmap* letters[],
		Bitmap* numbers[]);

/**
 * @brief Draws a time/date on the screen
 *
 * Draws a full date on the screen, in a formated way.
 *
 * @param time the time/date to be drawn
 * @param x the initial x coordinate
 * @param y the initial y coordinate
 * @param numbers an array of bitmaps of numbers
 */
void draw_time(Time time, int x, int y, Bitmap* numbers[]);

/**
 * @brief Draws a number on the screen
 *
 * Checks the number of digits in the number an draws them one by one.
 *
 * @param number the number to be drawn
 * @param x the initial x coordinate
 * @param y the initial y coordinate
 * @param numbers an array of bitmaps of numbers
 */
void draw_number(int number, int *x, int *y, Bitmap* numbers[]);

/**
 * @brief Converts a number into it's corresponding bitmap
 *
 * @param num the number to be converted
 * @param numbers an array of bitmaps of numbers
 * @return a bitmap equivalent to the number
 */
Bitmap* cvt_number_bitmap(int num, Bitmap* numbers[]);

/**
 * @brief Gets the file path relative to the game type.
 *
 * @param path the path of the highscores file
 * @param type the type of game of the highscores file
 */
void get_file_path(char *path, unsigned int type);

/**
 * @brief Receives input from the user and adds a new highscore object to its respective array
 *
 * Checks if the user made a good enough score to make it to the highscores. In that case, shows a message asking the player to
 * write his nickname (giving him the option to correct it in case he made a mistake), making sure he doesn't input any invalid characters.
 * When he's done (after he presses ENTER), then a new highscore object is added to it's respective array, and respective position.
 *
 * @param mode The game mode
 * @param hs The object that holds information on all highscores
 * @param seconds The time in seconds it took the player to finish the game
 * @param difficulty The difficulty of the game
 * @param mov_counter The number of movements made by the player
 * @param bmps Structure that holds the game's bitmaps
 * @return 0 if successful, 1 if not
 */
int check_highscore(unsigned int mode, Highscores *hs, unsigned int seconds, unsigned int difficulty, unsigned int mov_counter,
		GameBitmaps bmps);

/**
 * @brief Prints the name of the player on the screen
 *
 * Converts the characters in the name string into bitmaps and prints them, one by one, in a formated way. Used to interface with the
 * player when he is asked to insert his name for highscores recording.
 *
 * @param name string that holds the characters inserted by the player
 * @param size the size of the string
 * @param bmps Structure that holds the game's bitmaps
 */
void print_name(char* name, unsigned int size, GameBitmaps bmps);

/**
 * @brief Adds the '\0' to a string at the given index.
 *
 * @param string string to be terminated
 * @param index index to put the '\0' char
 * @param size maximum size of the string
 *
 * @return 0 if successful, -1 if not
 */
int terminate_string(char* string, unsigned int index, unsigned int size);

/** @} end of highscores */


#endif /* _PROJ_HIGHSCORES_H */
