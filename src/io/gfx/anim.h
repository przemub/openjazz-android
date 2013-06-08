
/**
 *
 * @file anim.h
 *
 * Part of the OpenJazz project
 *
 * @section History
 * 26th July 2009: Created anim.h from parts of sprite.h
 *
 * @section Licence
 * Copyright (c) 2005-2010 Alister Thomson
 *
 * OpenJazz is distributed under the terms of
 * the GNU General Public License, version 2.0
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef _ANIM_H
#define _ANIM_H


#include "OpenJazz.h"

#include <SDL/SDL.h>


// Classes

class Sprite;

/// Animation
class Anim {

	private:
		Sprite**       sprites;   ///< Sprite images
		signed char*   xOffsets;  ///< Horizontal offsets for each frame
		signed char*   yOffsets;  ///< Vertical offsets for each frame
		bool           ignoreDefaultYOffset;
		signed char    shootX;     ///< Bullet generation x-coordinate
		signed char    shootY;     ///< Bullet generation y-coordinate
		signed char    accessoryX; ///< Accessory animation x-coordinate
		signed char    accessoryY; ///< Accessory animation y-coordinate
		signed char    yOffset;    ///< Vertical offset
		unsigned char  frames;    ///< Number of frames
		unsigned char  frame;     ///< Current frame
		unsigned char  accessory; ///< Number of an animation that is an accessory to this animation
		                          ///< Most of the time accessories are used with guardians.

	public:
		Anim                        ();
		~Anim                       ();

		void  setData               (int length, signed char sX, signed char sY, signed char aX, signed char aY, unsigned char a, signed char y);
		void  setFrame              (int nextFrame, bool looping);
		void  setFrameData          (Sprite *frameSprite, signed char x, signed char y);
		int   getWidth              ();
		int   getHeight             ();
		int   getLength             ();
		fixed getShootX             ();
		fixed getShootY             ();
		fixed getAccessoryX         ();
		fixed getAccessoryY         ();
		fixed getAccessoryShootX    ();
		fixed getAccessoryShootY    ();
		fixed getOffset             ();
		Anim* getAccessory          ();
		void  draw                  (fixed x, fixed y);
		void  drawScaled            (fixed x, fixed y, fixed scale);
		void  disableDefaultOffset  ();
		void  setPalette            (SDL_Color *palette, int start, int amount);
		void  flashPalette          (int index);
		void  restorePalette        ();

};

#endif

