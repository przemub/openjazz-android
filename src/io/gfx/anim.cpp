
/**
 *
 * @file anim.cpp
 *
 * Part of the OpenJazz project
 *
 * @section History
 * 26th July 2009: Created anim.cpp from parts of sprite.cpp
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


#include "anim.h"
#include "sprite.h"

#include "jj1level/jj1level.h"


/**
 * Create empty animation.
 */
Anim::Anim () {

	sprites = new Sprite *[19];
	xOffsets = new signed char[19];
	yOffsets = new signed char[19];

	frame = 0;
	yOffset = 0;
	ignoreDefaultYOffset = false;

	return;

}


/**
 * Delete animation.
 */
Anim::~Anim () {

	delete[] sprites;
	delete[] xOffsets;
	delete[] yOffsets;

	return;

}


/**
 * Set overall animation data.
 *
 * @param length Number of frames
 * @param sX Bullet generation x-coordinate
 * @param sY Bullet generation y-coordinate
 * @param aX Accessory animation x-coordinate
 * @param aY Accessory animation y-coordinate
 * @param a Accessory animation index
 * @param y Vertical offset
 */
void Anim::setData (int length, signed char sX, signed char sY, signed char aX, signed char aY, unsigned char a, signed char y) {

	if (length > 19) {

		delete[] sprites;
		delete[] xOffsets;
		delete[] yOffsets;

		sprites = new Sprite *[length];
		xOffsets = new signed char[length];
		yOffsets = new signed char[length];

	}

	frames = length;
	shootX = sX;
	shootY = sY;
	accessoryX = aX;
	accessoryY = aY;
	accessory = a;
	yOffset = y;

	return;

}


/**
 * Set current frame.
 *
 * @param nextFrame The frame to use
 * @param looping Whether the animation should stop at the end or loop
 */
void Anim::setFrame (int nextFrame, bool looping) {

	if (looping) frame = nextFrame % frames;
	else frame = (nextFrame >= frames)? frames - 1: nextFrame;

	return;

}


/**
 * Set the data for the current frame.
 *
 * @param sprite Sprite to use
 * @param x Horizontal offset
 * @param y Vertical offset
 */
void Anim::setFrameData (Sprite *sprite, signed char x, signed char y) {

	sprites[frame] = sprite;
	xOffsets[frame] = x;
	yOffsets[frame] = y;

	return;

}


/**
 * Determine the width of the current frame.
 *
 * @return The width of the current frame
 */
int Anim::getWidth () {

	return sprites[frame]->getWidth();

}


/**
 * Determine the height of the current frame.
 *
 * @return The height of the current frame
 */
int Anim::getHeight () {

	return sprites[frame]->getHeight();

}


/**
 * Determine the length of the animation.
 *
 * @return The length of the animation
 */
int Anim::getLength () {

	return frames;

}


/**
 * Determine the bullet generation x-coordinate of the current frame.
 *
 * @return The bullet generation x-coordinate
 */
fixed Anim::getShootX () {

	return ITOF(shootX + (xOffsets[frame] << 2));

}


/**
 * Determine the bullet generation y-coordinate of the current frame.
 *
 * @return The bullet generation y-coordinate
 */
fixed Anim::getShootY () {

	return ITOF(shootY + yOffsets[frame] - yOffset);

}


/**
 * Determine the accessory animation x-coordinate.
 *
 * @return The accessory animation x-coordinate
 */
fixed Anim::getAccessoryX () {

	return ITOF(accessoryX << 2);

}


/**
 * Determine the accessory animation y-coordinate.
 *
 * @return The accessory animation y-coordinate
 */
fixed Anim::getAccessoryY () {

	return ITOF(accessoryY - yOffset);

}


/**
 * Determine the accessory bullet generation x-coordinate of the current frame.
 *
 * @return The accessory bullet generation x-coordinate
 */
fixed Anim::getAccessoryShootX () {

	return ITOF(shootX + (accessoryX << 2) + xOffsets[frame]);

}


/**
 * Determine the accessory bullet generation y-coordinate of the current frame.
 *
 * @return The accessory bullet generation y-coordinate
 */
fixed Anim::getAccessoryShootY () {

	return ITOF(shootY + accessoryY + yOffsets[frame] - yOffset);

}


/**
 * Determine the vertical offset.
 *
 * @return The vertical offset
 */
fixed Anim::getOffset () {

	if (!ignoreDefaultYOffset && yOffset == 0)
		return ITOF(yOffset) + TTOF(1);

	return ITOF(yOffset);

}


/**
 * Determine the accessory animation.
 *
 * @return The accessory animation
 */
Anim* Anim::getAccessory() {

	return level->getAnim(accessory);

}


/**
 * Draw current frame.
 *
 * @param x X-coordinate at which to draw
 * @param y Y-coordinate at which to draw
 */
void Anim::draw (fixed x, fixed y) {

	// In case yOffset is zero, and the ignore default offset flag is set,
	// draw the animation without any offset.

	if (ignoreDefaultYOffset && yOffset == 0)
		sprites[frame]->draw(FTOI(x) + (xOffsets[frame] << 2) + 1,
				FTOI(y) + yOffsets[frame] + 1);

	// In case yOffset is zero, most animations need a default offset
	// of 1 tile (32 pixels).

	else if (yOffset == 0)
		sprites[frame]->draw(FTOI(x) + (xOffsets[frame] << 2) + 1,
				FTOI(y) + yOffsets[frame] - TTOI(1) + 2);

	// In all other cases drawing with the Y offset will do.

	else
		sprites[frame]->draw(FTOI(x) + (xOffsets[frame] << 2) + 1,
				FTOI(y) + yOffsets[frame] - yOffset + 1);

	return;

}


/**
 * Draw current frame scaled.
 *
 * @param x X-coordinate at which to draw
 * @param y Y-coordinate at which to draw
 * @param scale Scaling factor
 */
void Anim::drawScaled (fixed x, fixed y, fixed scale) {

	// Used to draw bonus level player, so no offset
	sprites[frame]->drawScaled(FTOI(x), FTOI(y), scale);

	return;

}


/**
 * Disable default vertical offset.
 */
void Anim::disableDefaultOffset() {

	ignoreDefaultYOffset = true;

	return;

}


/**
 * Set the current frame's palette.
 *
 * @param palette The new palette to use
 * @param start The first entry to use
 * @param amount The number of entries to use
 */
void Anim::setPalette (SDL_Color *palette, int start, int amount) {

	sprites[frame]->setPalette(palette, 0, 256);

	return;

}


/**
 * Turn the whole of the current frame a single colour.
 *
 * @param index The index of the colour to use
 */
void Anim::flashPalette (int index) {

	sprites[frame]->flashPalette(index);

	return;

}


/**
 * Restore the current frame's original palette.
 */
void Anim::restorePalette () {

	sprites[frame]->restorePalette();

	return;

}


