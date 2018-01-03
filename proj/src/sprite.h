/**
 *  @author Joao Cardoso (jcard@fe.up.pt) ????
 *  Added by pfs@fe.up.pt
 *  Only slightly altered in order to fit our needs
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

/** @defgroup sprite Sprite
 * @{
 *
 * Sprite related functions
 */

/** A Sprite is an "object" that contains all needed information to
 * create, animate, and destroy a pixmap.  The functions assume that
 * the background is BLACK and they take into account collision with
 * other graphical objects or the screen limits. 
 */
typedef struct {
  int x;					/**< @brief current sprite x coordinate of top left corner*/
  int y;					/**< @brief current sprite y coordinate of top left corner*/
  int xspeed;				/**< @brief current speed in the x direction */
  int yspeed;  				/**< @brief current speed in the y direction */
  Bitmap* bitmap;           /**< @brief the sprite bitmap (use loadBitmap()) */
  Bitmap* empty;			/**< @brief the empty piece's bitmap */
} Sprite;

/** Creates with random speeds (not zero) and position
 * (within the screen limits), a new sprite with bitmap "bitmap", in
 * memory whose address is "base";
 * Returns NULL on invalid bitmap.
 */
Sprite *create_sprite(Bitmap* bitmap, Bitmap* empty, int x, int y, int xspeed, int yspeed);

/** The "fig" sprite is erased from memory whose address is "base"
 * and used resources released.
 */
void destroy_sprite(Sprite *sp);


/** @} end of sprite */

#endif
