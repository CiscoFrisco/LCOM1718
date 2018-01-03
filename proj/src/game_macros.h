#ifndef _LCOM_GAME_MACROS_H_
#define _LCOM_GAME_MACROS_H_

/** @defgroup game_macros game_macros
 * @{
 *
 * Constants to help in the development and readability of our code in game.c.
 */

#define FULL_IMG_TIME       3	 /**< @brief Number of seconds that the full image is shown to the player before the game */

#define IMG_X           	100  /**< @brief X coordinate (top left corner) of Full Image Preview in the Pre-Game Window */

#define IMG_Y           	128  /**< @brief Y coordinate (top left corner) of Full Image Preview in the Pre-Game Window */

#define TIMER1_X        	800  /**< @brief Timer Background X coordinate (top left corner) */

#define TIMER1_Y        	320  /**< @brief Timer Background Y coordinate (top left corner) */

#define PIECE_WIDTH_HD     	128  /**< @brief Size of a Puzzle Piece Side in 4x4 Board (in pixels) */

#define PIECE_WIDTH_EZ   	172  /**< @brief Size of a Puzzle Piece Side in 3x3 Board (in pixels) */

#define NUM_PIECES_HD      	16   /**< @brief Number of Pieces in a 4x4 Board */

#define NUM_PIECES_EZ		9    /**< @brief Number of Pieces in a 3x3 Board  */

#define BOARD_INIT_X_SP   	444  /**< @brief Board X coordinate (top left corner) in single player modes */

#define BOARD_INIT_Y_SP    	164  /**< @brief Board Y coordinate (top left corner) in single player modes */

#define BOARD_INIT_X_MP    	452  /**< @brief Board X coordinate (top left corner) in multiplayer mode */

#define BOARD_INIT_Y_MP    	45   /**< @brief Board Y coordinate (top left corner) in multiplayer mode */

#define BOARD_SIZE          524  /**< @brief Board Size in pixels */

#define GAME_TIMER_MIN1_X   160  /**< @brief Timer X coordinate of the tens digit  of the minutes (top left corner)*/

#define GAME_TIMER_MIN2_X   190  /**< @brief Timer X coordinate of the ones digit of the minutes (top left corner) */

#define GAME_TIMER_SEP_X    220  /**< @brief Timer X coordinate of the separation symbol ':' (top left corner) */

#define GAME_TIMER_SEC1_X   250  /**< @brief Timer X coordinate of the tens digit of the seconds (top left corner) */

#define GAME_TIMER_SEC2_X   280  /**< @brief Timer X coordinate of the ones digit of the seconds (top left corner) */

#define GAME_TIMER_Y		235  /**< @brief Game Timer Y coordinate (top left corner)*/

#define MOV_COUNTER_Y       420  /**< @brief Movement Counter Y coordinate (top left corner)*/

#define MOV_COUNTER_N1_X    175  /**< @brief Movement Counter X coordinate of the hundreds digit (top left corner) */

#define MOV_COUNTER_N2_X    215  /**< @brief Movement Counter X coordinate of the tens digit (top left corner)*/

#define MOV_COUNTER_N3_X    255  /**< @brief Movement Counter X coordinate of the ones digit (top left corner) */

#define TT_EZ               120  /**< @brief Time to finish puzzle (in seconds) in Time trial Mode 3x3 */

#define TT_HD               240  /**< @brief Time to finish puzzle (in seconds) in Time trial Mode 4x4 */

#define EXIT_GAME_X         37   /**< @brief Exit Game button X coordinate position (top left corner) */

#define EXIT_GAME_Y_SP      45   /**< @brief Exit Game button Y coordinate position (top left corner) in single player modes */

#define EXIT_GAME_Y_MP      500  /**< @brief Exit Game button Y coordinate position (top left corner) in multiplayer mode*/

#define USE_DB              1    /**< @brief Double buffer constant */

#define NO_DB               0	 /**< @brief Double buffer constant */

#define REGULAR_M           0    /**< @brief Regular Game Mode */

#define TIME_TRIAL_M        1    /**< @brief TimeTrial Game Mode */

#define EASY                0	 /**< @brief Easy difficulty */

#define NUM_IMAGES          3    /**< @brief Number of images that can be played */

#define ACTIVE              1    /**< @brief Signals that a power up is active */

#define DISABLED            0    /**< @brief Signals that a power up is disabled */

#define FREEZE_TIME         10   /**< @brief Number of seconds that a freeze power up may be active */

#define PUP_BREAK           60   /**< @brief Number of seconds until a player can activate another power up */


/**@}  end of game_macros */

#endif /* _LCOM_GAME_MACROS_H_ */
