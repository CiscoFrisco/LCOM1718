#ifndef _PROJ_GAME_H_
#define _PROJ_GAME_H_

#include "highscores.h"
#include "init_game.h"

/** @defgroup game game
 * @{
 *
 * Functions to support the operation of the game
 */

///Represents a movement, be it of an image, or between menu items
typedef enum  {UP, DOWN, LEFT, RIGHT, ENTER, IGNORE} Movement;

///Represents the state of the game while one if its game modes is being played
typedef enum {PLAYING, COMPLETE, EXIT_G} InGameState;


/* FUNCTIONS */

/**
 * @brief Starts a new game either of regular or time trial mode.
 *
 * Randomly selects the image to be solved and loads the necessary bitmaps. After that, shows the full image to the player during
 * 3 seconds, loads the board onto the screen and shows the puzzle being shuffled (The number of movements of the empty piece is high enough,
 * so there's no possibility the user remembers them). Then, using interrupts, lets the user solve the puzzle by moving the pieces either with
 * the keyboard or the mouse, while of course making sure the user can't make any not allowed movements.
 * Also records (and shows) the number of movements, and a timer. Lets the user 'forfeit' the game to the main menu if he wants.
 * In the end, if the user completed the puzzle, verifies if he made a good enough score and in that case asks for his nickname and stores
 * his score, then returning to the main menu.
 *
 * @param difficulty The difficulty of the game
 * @param hs The structure that holds information on highscores and may be modified if the player gets a good enough score
 * @param mode Mode of the game (regular or time trial)
 * @param bmps Structure that holds the game's bitmaps
 * @return 1 if the player finished the puzzle, 2 otherwise
 */
int regular_timetrial(int *difficulty, Highscores *hs, int mode, GameBitmaps *bmps);

/**
 * @brief Starts a new arcade game.
 *
 * A succession of regular games. The functionality is the same, the only difference is the game only ends when the user completes
 * every image available to play (or he decides to quit).
 *
 * @param difficulty The difficulty of the game
 * @param hs The structure that holds information on highscores and may be modified if the player gets a good enough score
 * @param bmps Structure that holds the game's bitmaps
 * @return 0 if the player finished all the puzzles, 1 otherwise
 */
int arcade_mode(int *difficulty, Highscores *hs, GameBitmaps *bmps);

/**
 * @brief Checks if a power up related variable has reached its time limit.
 *
 * @param powerup_set power up related variable
 * @param powerup_set_time time when the variable was activated
 * @param seconds current number of seconds
 * @param powerup_time number of seconds that the variable should be active
 */
void check_power_up_time(int *powerup_set, int *powerup_set_time, int seconds, const int powerup_time);

/**
 * @brief Prepares the start of a new multiplayer game.
 *
 * Loads the necessary images to play the game, asks the players to ready up, and when they're both ready starts a new instance of
 * a multiplayer game.
 *
 * @param difficulty The difficulty of the game
 * @param bmps Structure that holds the game's bitmaps
 * @return 1 if the player finished the puzzle, 2 if he lost or forfeited, -1 if the game didn't even begin
 */
int multi_player_mode(int *difficulty, GameBitmaps *bmps);

/**
 * @brief Starts a new multiplayer game.
 *
 * Lets 2 players play a regular game against each other on different virtual machines. The functionality is basically the same, except now
 * the input of the players affects the course of the game. For example, if someone completes the puzzle, a message is sent to the other
 * user indicating the game is over, and the function ends. Also allows the user to activate 'power ups', which show up when the other user
 * is close to finishing the game.
 *
 * @param difficulty The difficulty of the game
 * @param bmps Structure that holds the game's bitmaps
 * @return 1 if the player finished the puzzle, 2 if he lost or forfeited
 */
int multi_player_game(int *difficulty, GameBitmaps *bmps);

/**
 * @brief Waits for both players to ready up.
 *
 * Using serial port and keyboard interrupts, waits for both players to ready up, by asking them to press the SPACE key.
 * When someone presses it, a message is sent to the other user signaling he's ready. When both PC's send and receive the same message,
 * the function ends.
 *
 * @return 0 if both players accepted, 1 if the player decided to leave
 */
int players_ready();

/**
 * @brief Draws a message asking the players to ready up.
 *
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_setup_mp(GameBitmaps bmps);

/**
 * @brief Draws a message noticing the players that the game is starting.
 *
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_entering_game(GameBitmaps bmps);

/**
 * @brief Draws a message noticing the player that he won the multiplayer game.
 *
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_win(GameBitmaps bmps);

/**
 * @brief Draws a message noticing the player that he lost the multiplayer game.
 *
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_lose(GameBitmaps bmps);

/**
 * @brief Draws a message noticing the player that he won the multiplayer game by forfeit.
 *
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_win_forfeit(GameBitmaps bmps);

/**
 * @brief Draws a message noticing the player that he lost the multiplayer game by forfeit.
 *
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_lose_forfeit(GameBitmaps bmps);

/**
 * @brief Checks if an image has already been played in arcade mode.
 *
 * Goes through the puzzles_id array and checks if curr_puzz already exists there. If true, it means that image has already been played.
 *
 * @param puzzles_id An array of identifiers for the images that have already been played.
 * @param curr_puzz An identifier for the image that is being tested
 * @param size The size of the array
 * @return 1 if true, 0 if not
 */
int has_been_played(int puzzles_id[], int curr_puzz, int size);

/**
 * @brief Switches a given piece with the empty piece by moving the former's bitmap to the latter at an adequate speed.
 *
 * Depending on the board's size, sets up a sprite for the given piece's bitmap with an adequate speed and frame rate, moving it to the empty
 * piece's position.
 *
 * @param board The board that contains the pieces of the puzzle, which are modified by changing their bitmaps.
 * @param pos1 The index of the empty piece on the board's puzzle piece array
 * @param pos2 The index of the other piece on the board's puzzle piece array
 */
void animate_pieces(Board* board, unsigned int pos1, unsigned int pos2);

/**
 * @brief Checks if two movements passed as arguments are opposite to each other.
 * @param mov1 Movement 1
 * @param mov2 Movement 2
 * @return 1 if true, 0 otherwise
 */
int are_opposed_movements(Movement mov1, Movement mov2);

/**
 * @brief Shuffles an image represented in a board
 *
 * Using the same idea behind the game itself, the function generates, depending on the board's size, a 'random' number of movements
 * and loops while those movements aren't completed or the board isn't sufficiently shuffled, that is, there's still a large number
 * of images on their correct position.
 * In each loop, generates a random movement, checks if it isn't opposite to the previously generated one (we don't want a piece to go back
 * to where it was immediately after), and then checks if it's possible to make that movement. In that case, switches the bitmaps in memory
 * between the puzzle's pieces and animates them moving on the screen.
 *
 * @param board The board to be modified, holds the puzzle pieces which bitmaps are going to be switched.
 * @mode Indicates whether the function is used as a multiplayer power up or not.
 */
void shuffle(Board *board, int mode);

/**
 * @brief Checks if the user completed the puzzle
 *
 * Increments a counter by checking which pieces have their correct bitmap (ex: piece 0 should have bitmap 0).
 * In case the counter is equal to the board's number of pieces, it means the user completed the puzzle.
 *
 * @param board The structure that contains the puzzle's pieces
 * @return The number of pieces on their correct position
 */
int check_solution(Board *board);

/**
 * @brief Gets the index of the empty piece of the board's puzzle piece array.
 *
 * Goes through the board's puzzle piece array and checks which puzzle piece contains the 'empty' bitmap.
 *
 * @param board The structure that contains the puzzle's pieces
 * @return The index of the empty piece of the board.
 */
int get_empty_piece(Board* board);

/**
 * @brief Gets the index of the piece of the board's puzzle piece array that is currently clicked on by the mouse.
 *
 * Goes through the board's puzzle piece array and checks if the mouse cursor is currently over any of the board's
 * pieces by comparing their coordinates on the screen.
 *
 * @param board The structure that contains the puzzle's pieces
 * @param x the x coordinate of the mouse's cursor
 * @param y the y coordinate of the mouse's cursor
 * @return The index of the piece on the board's array.
 */
int get_piece(Board* board, int x, int y);

/**
 * @brief Switches two puzzle piece's bitmaps on memory and on the screen.
 *
 * Switches in memory two puzzle piece's bitmaps (one of them necessarily being the empty piece) and, according to the animation argument,
 * animates the switch on the screen or not. Specifically, on the shuffling phase (before the game actually starts), there is an animation
 * but during the course of the game there is not. Reason for this is, in our opinion, having no animation makes it faster an easier for
 * the player.
 *
 * @param board The structure that contains the puzzle's pieces
 * @param pos1 The index of the empty piece on the board's puzzle piece array
 * @param pos2 The index of the other piece on the board's puzzle piece array
 * @param animation Specifies whether there should be an animation or not
 */
void switch_bitmap(Board* board, unsigned int pos1, unsigned int pos2, int animation);

/**
 * @brief Checks if it's possible to switch two piece's bitmaps and in that case, does it both on memory and on the screen, with animation or not.
 *
 * Using mov (which is a movement of a piece to the empty piece) checks it it's possible to do that, verifying all possible cases
 * for different board sizes. In that case, switches the piece's bitmaps on memory and on the screen. Is also used to shuffle the board
 * before the game begins.
 *
 * @param board The structure that contains the puzzle's pieces
 * @param mov Movement to be checked
 * @param mov_counter Holds the number of movements
 * @param animation Specifies whether there should be an animation or not
 * @return 0 if possible, 1 if not
 */
int check_movement(Board* board, Movement mov, int *mov_counter, int animation);

/**
 * @brief Convertes a given makecode to a movement
 *
 * Considering that ingame, a movement happens when a piece moves to the empty piece's place, converts a given makecode to a movement.
 * Example: moving the second piece of the first row to the first, means it is a LEFT movement.
 *
 * @param makecode the makecode received from the keyboard
 * @param is_at_menu Indicates whether the game is currently at the menu or ingame
 * @return The converted makecode
 */
Movement cvt_makecode_mov(unsigned char makecode, int is_at_menu);

/**
 * @brief Checks if it's possible to switch two piece's bitmaps and in that case, does it both on memory and on the screen, with animation or not.
 *
 * Similar to the function used for the keyboard. Gets the piece that the user wants to switch and the empty piece,
 * checks if the movement is possible and in that case, switches the piece's bitmaps on memory and on the screen.
 *
 * @param board The structure that contains the puzzle's pieces
 * @param x the x coordinate of the mouse's cursor
 * @param y the y coordinate of the mouse's cursor
 * @param mov_counter Holds the number of movements
 * @param animation Specifies whether there should be an animation or not
 * @return 0 if possible, 1 if not
 */
int check_mouse_movement(Board* board, int x, int y, int *mov_counter, int animation);

/**
 * @brief Used to draw a descending timer on the screen while the full image is being shown (before the game starts)
 *
 * @param puzzle_id ID for the image that is going to be displayed
 * @param time The current time value (from 0 to 3)
 * @param bmps Structure that holds the game's bitmaps
 */
void update_time(int puzzle_id, int *time, GameBitmaps bmps);

/**
 * @brief Shows the full image of the puzzle that is going to be played during 3 seconds.
 *
 * @param puzzle_id The identifier for the image that is going to be played
 * @param bmps Structure that holds the game's bitmaps
 */
void show_full_image(int puzzle_id, GameBitmaps bmps);

/**
 * @brief Draws the background along with the full image of the puzzle that is going to be played and a message
 *
 * @param puzzle_id The identifier for the image that is going to be played
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_pre_game(int puzzle_id, GameBitmaps bmps);

/**
 * @brief Draws the elements of the game, that being the background, timer's background, exit game option and the board.
 * @param board The structure that contains the puzzle's pieces
 * @param bmps Structure that holds the game's bitmaps
 * @param mode Indicates whether the game is single player or multiplayer
 */
void draw_game(Board board, GameBitmaps bmps, unsigned int mode);

/**
 * @brief Draws a timer on the screen
 *
 * Retrieves each digit from timeInSeconds and draws the time on the screen in a formatted way, digit by digit.
 *
 * @param bmps Structure that holds the game's bitmaps, including the numbers
 * @param time_in_seconds number of seconds passed since the beginning of the game
 */
void draw_timer(GameBitmaps *bmps, unsigned int time_in_seconds);

/**
 * @brief Draws the board on the screen.
 *
 * Draws the board's background and its pieces
 *
 * @param board The structure that contains the puzzle's pieces
 */
void draw_board(Board board);

/**
 * @brief Draws a movement counter on the screen
 *
 * Retrieves each digit from mov_counter and draws the counter on the screen in a formatted way, digit by digit.
 *
 * @param bmps Structure that holds the game's bitmaps, including the numbers
 * @param mov_counter number of movements made by the player
 */
void draw_mov_counter(GameBitmaps *bmps, int mov_counter);

/**
 * @brief Converts a char into a bitmap.
 * @param character the character to be converted
 * @param letters an array of bitmaps of letters
 * @return the converted character in bitmap form
 */
Bitmap* cvt_char_bitmap(char character, Bitmap* letters[]);

/**
 * @brief Converted a makecode into a char
 * @param makecode the makecode to be converted
 * @return the converter makecode in char form
 */
char cvt_makecode_char(unsigned char makecode);

/**
 * @brief Draws a power up symbol on the screen
 *
 * @param power_up ID for the power up that is being drawn
 * @param hi Indicates whether the power up should be highlighted or not
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_power_up(char power_up, int hi, GameBitmaps bmps);

/**
 * @brief Checks if the user activated one of the power ups
 *
 * @param x the x coordinate of the mouse's cursor
 * @param y the y coordinate of the mouse's cursor
 * @return identifier for the activated power up, 0 for neither of them
 */
unsigned char check_power_up(int x, int y);

/**
 * @brief Evaluates multiplayer related events (such as activating power ups) and acts accordingly.
 * @param board Structure that holds the puzzle.
 * @param event Event to be evaluated
 * @param is_received Indicates if the event was received, or should be sent
 * @param seconds Time in seconds
 * @param bmps Structure that holds the game's bitmaps
 * @param st State of the game
 * @param var Structure that holds power up related variables
 */ 
void evaluate_event(Board *board, unsigned char event, unsigned char is_received,
		unsigned int seconds, GameBitmaps bmps, InGameState *st, PowerUpVar *var);

/**
 * @brief Draws the power up symbols on the screen.
 * @param state Indicates whether the symbols should be highlighted or not
 * @param bmps Structure that holds the game's bitmaps
 */ 
void draw_power_ups(unsigned int state, GameBitmaps bmps);

/**
 * @brief Increments or decrements a timer depending on the game mode.
 * @param mode Game mode
 * @param seconds Time in seconds
 * @param st State of the game
 */ 
void check_timer(int mode, unsigned int *seconds, InGameState *st);
/**
 * @brief Checks if the power up related variables have reached their time limit.
 *
 * @param var Structure that holds power up related variables
 * @param seconds current number of seconds
 */
void check_power_up_times(PowerUpVar *var, int seconds);

/**
 * @brief Creates an object of the stuct PowerUpVar and intializes its elements to 0
 *
 * @return Returns the object initialized
 */
PowerUpVar create_var();

/** @} end of game */

#endif /* _PROJ_GAME_H_ */

