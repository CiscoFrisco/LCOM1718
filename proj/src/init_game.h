#ifndef _PROJ_INIT_GAME_H_
#define _PROJ_INIT_GAME_H_

#include "bitmap.h"

/** @defgroup init_game init_game
 * @{
 *
 * Functions, constants and structures to support the initialization of the game
 */

/* CONSTANTS */

#define MAX_INPUT_SIZE 10				/**< @brief Maximum size of a name string */
#define USE_DB         1				/**< @brief Indicates to use double buffering */
#define NUM_LETTERS    26				/**< @brief Number of letters supported by the game */
#define NUM_NUMBERS    11				/**< @brief Number of numbers supported by the game */
#define READY          'r'				/**< @brief Character that signals that a player is ready */
#define PLAYER_WON	   'w'				/**< @brief Character that signals that a player has won */
#define FORFEIT        'f'				/**< @brief Character that signals that a player has forfeited */
#define FREEZE         'c'				/**< @brief Character that signals that a player has activated the FREEZE power up */
#define SHUFFLE        's'				/**< @brief Character that signals that a player has activated the SHUFFLE power up */
#define FINISHING      'e'				/**< @brief Character that signals that a player is close to finishing the game */
#define LEAVE          'l'				/**< @brief Character that signals that a player has left before the start of the game */
#define REGULAR        0				/**< @brief Identifier for the regular game mode */
#define TIMETRIAL      1				/**< @brief Identifier for the time trial game mode */
#define ARCADE         2				/**< @brief Identifier for the arcade game mode */
#define BACKSPACE      8				/**< @brief ASCII value of the backspace key */
#define A              97              	/**< @brief ASCII value of the a key */
#define Z              122             	/**< @brief ASCII value of the z key */
#define PUP1X          650				/**< @brief x coordinate of the shuffle power up symbol */
#define PUPY           610				/**< @brief y coordinate of the power up symbols */
#define PUP1SIZE       128				/**< @brief size of the shuffle power up symbol */
#define PUP2X          800				/**< @brief x coordinate of the freeze power up symbol */
#define PUP2SIZE       128				/**< @brief size of the freeze power up symbol */
#define SP			   0				/**< @brief represents Single Player mode */
#define MP			   2				/**< @brief represents MultiPlayer mode */
#define PANDA		   0				/**< @brief identifier for panda image */
#define MOTORBIKE	   1				/**< @brief identifier for motor bike image */
#define LANDSCAPE	   2				/**< @brief identifier for landscape image */

/* USEFUL STRUCTURES */

/** @name GameBitmaps */
/**@{
 *
 * Struct to store bitmaps needed throughout the course of a game
 */

typedef struct
{
	Bitmap* 	number1;
	Bitmap* 	number2;
	Bitmap* 	number3;
	Bitmap* 	message1;
	Bitmap* 	background;
	Bitmap* 	background_main;
	Bitmap* 	background_sp;
	Bitmap* 	background_mp;
	Bitmap* 	congratulations;
	Bitmap* 	exit_game;
	Bitmap* 	timer_bg;
	Bitmap** 	letters;
	Bitmap** 	numbers;
	Bitmap* 	hs_title;
	Bitmap* 	bs_title;
	Bitmap* 	board_bkg;
	Bitmap* 	timer;
	Bitmap* 	moves;
	Bitmap* 	slideit;

	//For multiplayer mode
	Bitmap* 	mp_message1;
	Bitmap* 	mp_message2;
	Bitmap* 	lost;
	Bitmap* 	won;
	Bitmap* 	lost_forfeit;
	Bitmap* 	won_forfeit;
	Bitmap* 	leave;
	Bitmap* 	pups;

	//Power Up symbols
	Bitmap* 	shuffle;
	Bitmap* 	shuffle_hi;
	Bitmap* 	freeze;
	Bitmap* 	freeze_hi;

	//Puzzle Bitmaps
	Bitmap* 	panda;
	Bitmap** 	panda_ez;
	Bitmap** 	panda_hd;

	Bitmap* 	motorbike;
	Bitmap** 	motorbike_ez;
	Bitmap** 	motorbike_hd;

	Bitmap* 	landscape;
	Bitmap** 	landscape_ez;
	Bitmap** 	landscape_hd;

} GameBitmaps;

/** @} end of GameBitmaps*/

/** @name PuzzlePiece */
/**@{
 *
 * Struct to represent a puzzle piece of the board
 */

typedef struct
{
	PuzzleBitmap 	bmp; 						/**< @brief bitmap with the puzzle piece's image */
	int 			x_coord; 					/**< @brief x_coordinate of top left corner */
	int 			y_coord; 					/**< @brief y_coordinate of top left corner */
	int 			width;						/**< @brief width of the puzzle piece */
} PuzzlePiece;

/** @} end of PuzzlePiece*/


/** @name Board */
/**@{
 *
 * Struct to represent a board that holds several images, each representing a piece of the original image
 */
typedef struct
{
	int 			x_init;   					/**< @brief board initial x coordinate (top left corner) */
	int 			y_init;  					/**< @brief board initial y coordinate (top left corner) */
	int 			size;  						/**< @brief board lateral size */
	Bitmap* 		background;   				/**< @brief board background */
	int 			num_pieces;  				/**< @brief number of pieces in the board */
	PuzzlePiece* 	pieces;  					/**< @brief array with the board's puzzle pieces */
} Board;

/** @} end of Board*/


/** @name Board */
/**@{
 *
 * Struct to hold variables related to power up control
 */
typedef struct
{
	int 			freeze_time;				/**< @brief Time when the freeze power up was activated */
	int 			freeze;						/**< @brief Indicates whether the freeze power up is active */
	int 			pup_break;					/**< @brief Time when a power up was activated by the user */
	int 			pup_set;					/**< @brief Indicates whether a power up was activated by the user */
	int 			finish_break;				/**< @brief Time when a finish_set was activated */
	int 			finish_set;					/**< @brief Indicates whether the FINISHING message should be sent or not */
} PowerUpVar;

/** @} end of PowerUpVar*/

/* FUNCTIONS */


/**
 * @brief Loads the bitmaps necessary throughout the course of the game
 *
 * Using Henrique Ferrolho's bitmap functions, loads from their respective folders all the images necessary to play a game
 *
 * @param bmps Structure to be loaded with the game's bitmaps
 */
void load_bmps(GameBitmaps *bmps);

/**
 * @brief Initializes the main part of the game.
 *
 * Loads the necessary bitmaps, initializes the menus, reads the highscores files and presents the main menu.
 * From there, supported by a state machine and functions to detect and analyze user input, allows menu navigation and
 * games, until the user decides to quit.
 *
 * @return 0 if successful, -1 if not
 */
int init_game();

/**
 * @brief Deletes the bitmaps when the game ends
 *
 * Using Henrique Ferrolho's bitmap functions, deletes all the images  at the end of the game
 *
 * @param bmps Structure to be deleted from the game
 */
void delete_bmps(GameBitmaps *bmps);

/**
 * @brief According to the current_puzzle argument, loads onto bitmaps the puzzle bitmaps of the image to be played.
 *
 * @param bitmaps An array of bitmaps that will hold the puzzle bitmaps
 * @param bmps Structure that holds the game's bitmaps
 * @param num_pieces The number of pieces in the board
 * @param current_puzzle The current puzzle to be played
 */
void init_puzzle_bitmaps(Bitmap* bitmaps[], GameBitmaps bmps, int num_pieces, int *current_puzzle);

/**
 * @brief Initializes an array with the puzzle pieces of the board.
 *
 * @param pieces An array of puzzle pieces to be initialized
 * @param bitmaps An array of bitmaps that holds the puzzle bitmaps
 * @param num_pieces The number of pieces in the board
 * @param mode Indicates whether the mode is single player or multiplayer
 * @param bmps Structure that holds the game's bitmaps
 * @param current_puzzle The current puzzle to be played
 */
void init_puzzle_pieces(PuzzlePiece pieces[], Bitmap* bitmaps[],
		int num_pieces, unsigned int mode, GameBitmaps bmps, int *current_puzzle);

/**
 * @brief Initializes a board according to its number of pieces, with its respective pieces.
 *
 * @param board The board to be initialized
 * @param pieces An array with the board's puzzle pieces
 * @param bitmaps An array of bitmaps that holds the puzzle bitmaps
 * @param num_pieces The number of pieces in the board
 * @param bmps Structure that holds the game's bitmaps
 * @param mode Indicates whether the mode is single player or multiplayer
 * @param current_puzzle The current puzzle to be played
 */
void init_board(Board* board, PuzzlePiece pieces[], Bitmap* bitmaps[],int num_pieces,
		GameBitmaps bmps, unsigned int mode, int *current_puzzle);

/**
 * @brief Initialized an array with bitmaps of numbers
 *
 * @param numbers An array to be initialized with bitmaps of numbers
 */
void init_numbers(Bitmap* numbers[]);

/**
 * @brief Initialized an array with bitmaps of letters
 *
 * @param letters An array to be initialized with bitmaps of letters
 */
void init_letters(Bitmap* letters[]);

/**
 * @brief Loads onto bitmaps the panda's images for 3x3 mode.
 * @param bitmaps An array of bitmaps to be loaded with the images
 */
void load_panda_ez(Bitmap* bitmaps[]);

/**
 * @brief Loads onto bitmaps the panda's images for 4x4 mode.
 * @param bitmaps An array of bitmaps to be loaded with the images
 */
void load_panda_hd(Bitmap* bitmaps[]);

/**
 * @brief Loads onto bitmaps the motorbike's images for 3x3 mode.
 * @param bitmaps An array of bitmaps to be loaded with the images
 */
void load_mbike_ez(Bitmap* bitmaps[]);

/**
 * @brief Loads onto bitmaps the motorbike's images for 4x4 mode.
 * @param bitmaps An array of bitmaps to be loaded with the images
 */
void load_mbike_hd(Bitmap* bitmaps[]);

/**
 * @brief Loads onto bitmaps the landscapes's images for 3x3 mode.
 * @param bitmaps An array of bitmaps to be loaded with the images
 */
void load_landscape_ez(Bitmap* bitmaps[]);

/**
 * @brief Loads onto bitmaps the landscapes's images for 4x4 mode.
 * @param bitmaps An array of bitmaps to be loaded with the images
 */
void load_landscape_hd(Bitmap* bitmaps[]);

/** @} end of game */

#endif /* _PROJ_INIT_GAME_H_ */

