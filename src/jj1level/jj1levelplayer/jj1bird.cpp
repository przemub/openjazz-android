
/**
 *
 * @file jj1bird.cpp
 *
 * Part of the OpenJazz project
 *
 * @section History
 * 1st March 2009: Created bird.cpp from parts of events.cpp
 * 1st August 2012: Renamed bird.cpp to jj1bird.cpp
 *
 * @section Licence
 * Copyright (c) 2005-2012 Alister Thomson
 *
 * OpenJazz is distributed under the terms of
 * the GNU General Public License, version 2.0
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "../jj1bullet.h"
#include "../jj1event/jj1event.h"
#include "../jj1level.h"
#include "jj1bird.h"
#include "jj1levelplayer.h"

#include "io/gfx/video.h"


/**
 * Create a bird for the specified player.
 *
 * @param birds The player's existing birds (NULL if none)
 * @param rescuer The player that freed the bird
 * @param gX The new bird's grid x-coordinate
 * @param gY The new bird's grid y-coordinate
 */
JJ1Bird::JJ1Bird (JJ1Bird* birds, JJ1LevelPlayer *rescuer, unsigned char gX, unsigned char gY) {

	next = birds;
	player = rescuer;
	x = TTOF(gX);
	y = TTOF(gY);
	dx = 0;
	dy = 0;
	fleeing = false;
	fireTime = 0;

	return;

}


/**
 * Delete all birds.
 */
JJ1Bird::~JJ1Bird () {

	if (next) delete next;

	return;

}


/**
 * Delete this bird.
 *
 * @return The next bird
 */
JJ1Bird* JJ1Bird::remove () {

	JJ1Bird* oldNext;

	oldNext = next;
	next = NULL;
	delete this;

	return oldNext;

}


/**
 * Get the player that freed the bird.
 *
 * @return The player
 */
JJ1LevelPlayer * JJ1Bird::getPlayer () {

	return player;

}


/**
 * Notify the bird that the player has been hit.
 */
void JJ1Bird::hit () {

	fleeing = true;

	return;

}


/**
 * Recursively count the number of birds.
 *
 * @return The number of birds ahead of this bird, plus one
 */
int JJ1Bird::getFlockSize () {

	if (next) return next->getFlockSize() + 1;

	return 1;

}


/**
 * Recursively set the number of birds.
 *
 * @param size The number of birds not already counted
 *
 * @return Remaining bird (NULL if none)
 */
JJ1Bird* JJ1Bird::setFlockSize (int size) {

	if (size <= 0) {

		delete this;
		return NULL;

	}

	if (size > 1) {

		if (!next) next = new JJ1Bird(NULL, player, FTOT(x), FTOT(y));

		next = next->setFlockSize(size - 1);

	}

	return this;

}


/**
 * JJ1Bird iteration.
 *
 * @param ticks Time
 * @param msps Ticks per step
 *
 * @return Remaining bird (NULL if none)
 */
JJ1Bird* JJ1Bird::step (unsigned int ticks, int msps) {

	Movable* leader;
	JJ1Event* event;
	bool target;

	// Process the next bird
	if (next) next = next->step(ticks, msps);

	if (next) leader = next;
	else leader = player;

	if (fleeing) {

		// Trajectory for flying away
		dx = F80;
		dy = -F80;

		// If the bird has flown off-screen, remove it
		if (y < viewY - F160) return remove();

	} else {

		// Trajectory for flying towards the leader

		if ((x < leader->getX() - F160) || (x > leader->getX() + F160)) {

			// Far away from the leader
			// Approach the leader at a speed proportional to the distance

			dx = leader->getX() - x;

		} else if (x < leader->getX()) {

			// To the left of the leader, so move right

			if (dx < F80) dx += 400 * msps;

		} else {

			// To the right of the leader, so move left

			if (dx > -F80) dx -= 400 * msps;

		}

		if (y > level->getWaterLevel() - F24) {

			// Always stay above water

			y = level->getWaterLevel() - F24;
			dy = 0;

		} else {

			if ((y < leader->getY() - F100) || (y > leader->getY() + F100)) {

				// Far away from the leader
				// Approach the leader at a speed proportional to the distance

				dy = (leader->getY() - F64) - y;

			} else if (y < leader->getY() - F64) {

				// Above the leader, so move downwards

				if (dy < F80) dy += 400 * msps;

			} else {

				// Below the leader, so move upwards

				if (dy > -F80) dy -= 400 * msps;

			}


		}


		if (ticks > fireTime) {

			// Check for nearby targets

			target = false;
			event = level->getEvents();

			if (player->getFacing()) {

				while (event && !target) {

					target = event->isEnemy() && event->overlap(x, y, F160, F100);

					event = event->getNext();

				}

			} else {

				while (event && !target) {

					target = event->isEnemy() && event->overlap(x - F160, y, F160, F100);

					event = event->getNext();

				}

			}

			// If there is a target in the vicinity, generate bullets
			if (target) {

				level->bullets = new JJ1Bullet(this, false, ticks);

				fireTime = ticks + T_BIRD_FIRE;

			}

		}

	}

	// Apply trajectory
	x += (dx * msps) >> 10;
	y += (dy * msps) >> 10;

	return this;

}


/**
 * Draw the bird.
 *
 * @param ticks Time
 * @param change Time since last step
 */
void JJ1Bird::draw (unsigned int ticks, int change) {

	Anim *anim;

	if (next) next->draw(ticks, change);

	anim = level->getAnim((player->getFacing() || fleeing)? BIRD_RIGHTANIM: BIRD_LEFTANIM);
	anim->setFrame(ticks / 80, true);

	anim->draw(getDrawX(change), getDrawY(change));

	return;

}


