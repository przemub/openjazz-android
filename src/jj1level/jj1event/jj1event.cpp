
/**
 *
 * @file jj1event.cpp
 *
 * Part of the OpenJazz project
 *
 * @section History
 * 23rd August 2005: Created level.c
 * 1st January 2006: Created events.c from parts of level.c
 * 3rd February 2009: Renamed events.c to events.cpp
 * 5th February 2009: Added parts of events.cpp and level.cpp to player.cpp
 * 11th February 2009: Created bullet.cpp from parts of events.cpp
 * 1st March 2009: Created bird.cpp from parts of events.cpp
 * 19th March 2009: Created sprite.cpp from parts of event.cpp and player.cpp
 * 19th July 2009: Created eventframe.cpp from parts of events.cpp
 * 19th July 2009: Renamed events.cpp to event.cpp
 * 2nd March 2010: Created guardians.cpp from parts of event.cpp and
 *                 eventframe.cpp
 * 2nd March 2010: Created bridge.cpp from parts of event.cpp and eventframe.cpp
 * 5th February 2011: Moved parts of eventframe.cpp to event.cpp
 * 1st August 2012: Renamed event.cpp to jj1event.cpp
 *
 * @section Licence
 * Copyright (c) 2005-2013 Alister Thomson
 *
 * OpenJazz is distributed under the terms of
 * the GNU General Public License, version 2.0
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @section Description
 * Deals with events in ordinary levels.
 *
 */


#include "../jj1level.h"
#include "jj1event.h"

#include "io/gfx/video.h"
#include "io/sound.h"
#include "util.h"


/**
 * Create event
 *
 * @param gX X-coordinate
 * @param gY Y-coordinate
 */
JJ1Event::JJ1Event (unsigned char gX, unsigned char gY) {

	set = level->getEvent(gX, gY);

	x = TTOF(gX);
	y = TTOF(gY + 1);
	dx = 0;
	dy = 0;

	next = level->getEvents();
	gridX = gX;
	gridY = gY;
	flashTime = 0;

	animType = E_NOANIM;
	setAnimType(E_LEFTANIM);
	noAnimOffset = false;

	return;

}


/**
 * Delete all events
 */
JJ1Event::~JJ1Event () {

	if (next) delete next;

	return;

}


/**
 * Delete this event
 *
 * @return The next event
 */
JJ1Event* JJ1Event::remove () {

	JJ1Event *oldNext;

	oldNext = next;
	next = NULL;
	delete this;

	return oldNext;

}


/**
 * Get the next event
 *
 * @return The next event
 */
JJ1Event * JJ1Event::getNext () {

	return next;

}


/**
 * Initiate the destruction of the event
 *
 * @param ticks Time
 */
void JJ1Event::destroy (unsigned int ticks) {

	setAnimType(E_LFINISHANIM | (animType & 1));

	level->setEventTime(gridX, gridY, ticks + (anim->getLength() * set->animSpeed << 3));

	level->playSound(set->sound);

	return;

}


/**
 * Deal with bullet collisions
 *
 * @param source Source of the bullet
 * @param ticks Current time
 *
 * @return Whether or not the hit was successful
 */
bool JJ1Event::hit (JJ1LevelPlayer *source, unsigned int ticks) {

	int hitsRemaining;

	// Check if event has already been destroyed
	if (((animType & ~1) == E_LFINISHANIM) || (ticks < flashTime)) return false;

	hitsRemaining = level->hitEvent(gridX, gridY, source);

	// If the event cannot be hit, do not register hit
	if (hitsRemaining < 0) return false;

	// Check if the hit has destroyed the event
	if (hitsRemaining == 0) destroy(ticks);

	// The event has been hit, so it should flash
	flashTime = ticks + T_FLASH;

	// Register hit
	return true;

}


/**
 * Determine whether or not the event is an enemy
 *
 * @return Whether or not the event is an enemy
 */
bool JJ1Event::isEnemy () {

	return set->strength && (set->modifier == 0);

}


/**
 * Determine whether or not the event is from the given position
 *
 * @return Whether or not the event is from the given position
 */
bool JJ1Event::isFrom (unsigned char gX, unsigned char gY) {

	return (gX == gridX) && (gY == gridY);

}


/**
 * Calculate the width and height of the event
 */
void JJ1Event::calcDimensions () {

	if (animType == E_NOANIM) height = F32;
	else if ((set->anims[animType] & 0x7F) == 0) height = 0;
	else height = ITOF(anim->getHeight());

	if (animType == E_NOANIM) width = F32;
	else if ((set->anims[animType] & 0x7F) == 0) width = 0;
	else {

		width = ITOF(anim->getWidth());

		// Blank sprites for e.g. invisible springs
		if ((width == F1) && (height == F1)) width = F32;

	}

	return;

}


/**
 * Determine whether or not the event is overlapping the given area
 *
 * @param left The x-coordinate of the left of the area
 * @param top The y-coordinate of the top of the area
 * @param width The width of the area
 * @param height The height of the area
 *
 * @return Whether or not there is an overlap
 */
bool JJ1Event::overlap (fixed left, fixed top, fixed width, fixed height) {

	fixed offset = 0;

	if (anim && noAnimOffset) offset = anim->getOffset();

	return (x + this->width >= left) &&
		(x < left + width) &&
		(y + offset >= top) &&
		(y + offset - this->height < top + height);

}


/**
 * Sets the animation type and updates the current animation and dimensions
 *
 * @param type The new animation type
 */
void JJ1Event::setAnimType(unsigned char type) {

	if (type == animType) return;

	animType = type;

	if (animType == E_NOANIM) anim = NULL;

	// If there is no shooting animation, use the normal animation instead
	else if (((animType & ~1) == E_LSHOOTANIM) && (set->anims[animType] == 0))
		anim = level->getAnim(set->anims[animType & 1] & 0x7F);

	else anim = level->getAnim(set->anims[animType] & 0x7F);

	calcDimensions();

	return;

}


/**
 * Sets the animation frame and updates the current dimensions
 */
void JJ1Event::setAnimFrame (int frame) {

	if (!anim) return;

	anim->setFrame(frame, true);
	calcDimensions();

	return;

}


/**
 * Functionality required by all event types on each iteration
 *
 * @param ticks Time
 * @param msps Ticks per step
 *
 * @return Animation
 */
JJ1EventType* JJ1Event::prepareStep (unsigned int ticks, int msps) {

	// Process the next event
	if (next) next = next->step(ticks, msps);

	// Get the event properties
	set = level->getEvent(gridX, gridY);

	// If the event has been removed from the grid, destroy it
	if (!set) return NULL;

	// If the event and its origin are off-screen, the event is not in the
	// process of self-destruction, remove it
	if (((animType & ~1) != E_LFINISHANIM) &&
		((x < viewX - F192) || (x > viewX + ITOF(canvasW) + F192) ||
		(y < viewY - F160) || (y > viewY + ITOF(canvasH) + F160)) &&
		((gridX < FTOT(viewX) - 1) ||
		(gridX > ITOT(FTOI(viewX) + canvasW) + 1) ||
		(gridY < FTOT(viewY) - 1) ||
		(gridY > ITOT(FTOI(viewY) + canvasH) + 1))) return NULL;

	return set;

}


/**
 * Draw the event's energy bar
 *
 * @param ticks Time
 */
void JJ1Event::drawEnergy (unsigned int ticks) {

	Anim* miscAnim;
	int hits;

	if (!set || set->modifier != 8) {

		if (next) next->drawEnergy(ticks);

		return;

	} else if (set->strength) {

		// Draw boss energy bar

		hits = level->getEventHits(gridX, gridY) * 100 / set->strength;


		// Devan head

		miscAnim = level->getMiscAnim(1);
		miscAnim->setFrame(0, true);

		if (ticks < flashTime) miscAnim->flashPalette(0);

		miscAnim->draw(ITOF(canvasW - 44), ITOF(hits + 48));

		if (ticks < flashTime) miscAnim->restorePalette();


		// Bar
		drawRect(canvasW - 40, hits + 40, 12, 100 - hits, (ticks < flashTime)? 0: 32);

	}

	return;

}

