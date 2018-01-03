#ifndef _PROJ_MENU_H_
#define _PROJ_MENU_H_

#include "bitmap.h"
#include "game.h"
#include "highscores.h"


/** @defgroup menu menu
 * @{
 *
 * Constants and Funtions relevant for the implementation of the game menus.
 */

/* CONSTANTS */

#define USE_DB 				1     	/**< @brief Double Buffer */
#define MENU_OPTION_WIDTH  	360    	/**< @brief Width of the menu's buttons */
#define MENU_OPTION_HEIGHT 	58    	/**< @brief Height of the menu's buttons */
#define MENU_OPTION_XI     	332     /**< @brief X coordinate of the menu's buttons */
#define DEFAULT_HIGHLIGHTED 0      	/**< @brief Buttons can be highlighted or not. These constant sets the default button not to be highlighted */
#define EASY                0      	/**< @brief Difficulty of the game -- 3x3 in Easy mode */
#define HARD                1      	/**< @brief Difficulty of the game -- 4x4 in Hard mode */
#define FIRST_OPTION        0       /**< @brief Constant that identifies whether or not a menu option is the first option in the respective menu (to restrict the movements) */
#define REGULAR             0       /**< @brief Regular mode constant */
#define TIMETRIAL           1       /**< @brief TimeTrial mode constant */
#define SINGLE_PLAYER       1      	/**< @brief Single Player modes constant */
#define MULTIPLAYER         0      	/**< @brief Multiplayer modes constant */

/* USEFUL STRUCTURES */

/// Menu events for the menu state machine
typedef enum {QUIT, BACK, TO_MENU, TO_GAME, TO_HIGH, TO_BS, TO_REGULAR, TO_ARCADE, TO_T_TRIAL, TO_M_PLAYER, EZ, HD, FAIL} event;

///@ Menu states for the menu state machine
typedef enum {MAIN_M, CHOOSE_GAME, HIGHSCORES, CHOOSE_BOARD_SIZE, IN_GAME, EXIT_GAME} GameState;

/** @name MenuItem */
/**@{
 *
 * Struct to represent a Menu Item (Menu Button)
 */

typedef struct
{
	Bitmap* 	bitmap;					/**< @brief bitmap with the item's message */
	Bitmap* 	bitmapHi;				/**< @brief highlighted version of the bitmap */
	int 		x_coord;				/**< @brief x coordinate of the top left corner of the item */
	int 		y_coord;				/**< @brief y coordinate of the top left corner of the item */
	int 		width;					/**< @brief width of the item */
	int 		height;					/**< @brief height of the item */
} MenuItem;

/** @} end of MenuItem*/

/** @name Menu */
/**@{
 *
 * Struct to hold information about a Menu.
 */
typedef struct
{
	unsigned int 	id;					/**< @brief menu's identifier */
			 int 	highlighted;		/**< @brief keeps the index for the current highlighted item */
	unsigned int 	num_items;			/**< @brief number of items on the menu */
	MenuItem* 		items;				/**< @brief array of the menu's items */
} Menu;

/** @} end of Menu*/

/** @name MenuHelper */
/**@{
 *
 * Struct to help navigation on menus
 */
typedef struct
{
 	int itemIndex;						/**< @brief index of the MenuItem */
 	int menuIndex;						/**< @brief item's menu identifier */
} MenuHelper;

/** @} end of MenuHelper*/



/* FUNCTIONS */

/**
 * @brief Initializes all the menus.
 *
 * @param path[] Array of bitmaps with the corresponding bitmaps of the items
 * @param menus Array with all the menus
 * @param main_menu_items Items to be part of the main menu
 * @param choose_mode_items Items to be part of the choose mode menu
 * @param hs_items Items to be part of the highscores menu
 * @param choose_bs_items Items to be part of the choose board size menu
 */
void init_menus(Bitmap* path[], Menu menus[], MenuItem main_menu_items[], MenuItem choose_mode_items[], MenuItem hs_items[], MenuItem choose_bs_items[]);

/**
 * @brief Loads the bitmaps needed to initialize the menu's items
 *
 * Loads the needed bitmaps and puts them in the argument of this function
 *
 * @param path[] Destination of the bitmaps to be loaded
 */
void load_menu_options(Bitmap* path[]);

/**
 * @brief To check if the player has left the current game.
 *
 * According to the mouse's coordinates,verifies if they are within the EXIT GAME button and returns an event (only when during a game)
 * to inform if the player has clicked it
 *
 * @param x Mouse current x coordinate
 * @param y Mouse current y coordinate
 * @param mode To distinguish between single player modes or multiplayer mode
 * @return Returns TO_MENU event if it is within the EXIT GAME button, otherwise returns FAIL event.
 */
event check_exit(int x, int y, int mode);


/**
 * @brief Initializes an object of the struct Menu
 *
 * Receives a menu and its attributes and initializes the menu with the corresponding atributes
 *
 * @param menu Menu to be initialized
 * @param items[] Items to be part of the current menu
 * @param num_items Number of items of the menu
 * @param id Menu's ID
 */
void init_menu(Menu *menu, MenuItem items[], unsigned int num_items, unsigned int id);


/**
 * @brief Initializes the Menu items (buttons) of the Main Menu
 *
 * Receives the paths with the corresponding bitmaps and creates multiple objects of the strut MenuItem with the respective attributes
 *
 * @param path[] Array of bitmaps with the corresponding bitmaps of the items
 * @param items[] Array of MenuItems of the current menu
 */
void init_main_menu_items(Bitmap* path[], MenuItem items[]);

/**
 * @brief Draws the menu specified as an argument
 *
 * @param menu Menu to be drawn
 * @param bmps Structure that holds the game's bitmaps
 */
void draw_menu (Menu menu, GameBitmaps bmps);

/**
 * @brief Initializes the Menu items (buttons) of the Choose GameMode Menu
 *
 * Receives the paths with the corresponding bitmaps and creates multiple objects of the strut MenuItem with the respective attributes
 *
 * @param path[] Array of bitmaps with the corresponding bitmaps of the items
 * @param items[] Array of MenuItems of the current menu
 */
void init_choose_mode_menu_items(Bitmap* path[], MenuItem items[]);

/**
 * @brief Initializes the Menu items (buttons) of the Choose Board Size Menu
 *
 * Receives the paths with the corresponding bitmaps and creates multiple objects of the strut MenuItem with the respective attributes
 *
 * @param path[] Array of bitmaps with the corresponding bitmaps of the items
 * @param items[] Array of MenuItems of the current menu
 */
void init_choose_bs_menu_items(Bitmap* path[], MenuItem items[]);

/**
 * @brief Initializes the Menu items (buttons) of the Highscores Menu
 *
 * Receives the paths with the corresponding bitmaps and creates multiple objects of the strut MenuItem with the respective attributes
 *
 * @param path[] Array of bitmaps with the corresponding bitmaps of the items
 * @param items[] Array of MenuItems of the current menu
 */
void init_hs_menu_items(Bitmap* path[], MenuItem items[]);

/**
 * @brief Receives an event and acts accordingly
 *
 * According to the event, updates the game state and calls the corresponding functions
 *
 * @param evt Event received either by the mouse or keyboard
 * @param *st Current game state
 * @param menus[] An array of menus. Contains every menu of the Game.
 * @param difficulty Current game difficulty
 * @param *hs Highscores of every game mode
 * @param bmps Structure that holds the game's bitmaps
 */
void check_event(event evt, GameState *st, Menu menus[], int *difficulty, Highscores *hs, GameBitmaps *bmps);

/**
 * @brief Gets an event from the keyboard to be used on the check_event function.
 *
 * Depending on the keyboard's key pressed it generates the correspondent event.
 *
 *
 * @param mov The corresponding movement depending on the keyboard's key pressed (UP, DOWN)
 * @param st Current game state
 * @param menus[] An array of menus. Contains every menu of the Game.
 * @param *prev_item Information about the previous item of the mouse
 * @param hs Struct with the highscores information of every game mode.
 * @param bmps Structure that holds the game's bitmaps
 *
 * @return Returns an event depending on the keyboards movement (key pressed).
 */
event set_event_kbd(Movement mov, GameState st, Menu menus[], MenuHelper *prev_item, Highscores hs, GameBitmaps bmps);

/**
 * @brief Gets an event from the mouse to be used on the check_event function.
 *
 * Depending on the mouse's atributes (coordinates and buttons) it generates the correspondent event.
 *
 * @param x Mouse current x coordinate
 * @param y Mouse current y coordinate
 * @param st State of the program
 * @param menus[] An array of menus. Contains every menu of the Game.
 * @param *prev_item Information about the previous item of the mouse
 * @param *prev_ldown Information about the left mouse button in the previous call. 0 if it was pressed, otherwise it means it was released.
 * @param hs Struct with the highscores information of every game mode.
 * @param bmps Structure that holds the game's bitmaps
 *
 * @return Returns an event depending on the mouse's position and button pressing variables.
 */
event set_event_mouse(int x, int y, GameState st, Menu menus[], MenuHelper *prev_item, int *prev_ldown, Highscores hs, GameBitmaps bmps);

/**
 * @brief Gets the current item pointed by the mouse.
 *
 * Checks the current coordinates of the mouse and compares to the coordinates of the menu's buttons.
 * Retrieves the button order in the current menu.
 *
 * @param x Mouse current x coordinate
 * @param y Mouse current y coordinate
 * @param menu Current menu
 *
 * @return Returns the current item that the mouse is pointing to. If the mouse is not pointing to any item returns item with index -1.
 */
MenuHelper get_menu_item(int x, int y, Menu menu);

/**
 * @brief Compares two MenuHelper objects
 * @param m1 MenuHelper object
 * @param m2 MenuHelper object
 * @return 1 if true, 0 if false
 */ 
unsigned int cmp_menu_helper(MenuHelper m1, MenuHelper m2);

/** @} end of menu */

#endif /* _PROJ_MENU_H_ */
