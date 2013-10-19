
/**
 *
 * @file jj1levelplayerframe.cpp
 *
 * Part of the OpenJazz project
 *
 * @section History
 * 23rd August 2005: Created level.c
 * 1st January 2006: Created events.c from parts of level.c
 * 3rd February 2009: Renamed events.c to events.cpp and level.c to level.cpp,
 *                    created player.cpp
 * 5th February 2009: Added parts of events.cpp and level.cpp to player.cpp
 * 18th July 2009: Created playerframe.cpp from parts of player.cpp
 * 24th June 2010: Renamed playerframe.cpp to levelplayerframe.cpp
 * 29th June 2010: Created jj2levelplayerframe.cpp from parts of
 *                 levelplayerframe.cpp
 * 1st August 2012: Renamed levelplayerframe.cpp to jj1levelplayerframe.cpp
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
 * Provides the once-per-frame functions of players in levels.
 *
 */


#include "../jj1bullet.h"
#include "../jj1event/jj1event.h"
#include "../jj1level.h"
#include "jj1bird.h"
#include "jj1levelplayer.h"

#include "io/controls.h"
#include "io/gfx/font.h"
#include "io/gfx/video.h"
#include "io/sound.h"
#include "util.h"


/**
 * Determine whether or not the area below the player is solid when travelling
 * downwards.
 *
 * @param yOffset Vertical offset of the mask values to check
 *
 * @return Solidity
 */
bool JJ1LevelPlayer::checkMaskDown (fixed yOffset) {

	return level->checkMaskDown(x + PXO_ML, y + yOffset) ||
		level->checkMaskDown(x + PXO_MID, y + yOffset) ||
		level->checkMaskDown(x + PXO_MR, y + yOffset);

}


/**
 * Determine whether or not the area above the player is solid when travelling
 * upwards.
 *
 * @param yOffset Vertical offset of the mask values to check
 *
 * @return Solidity
 */
bool JJ1LevelPlayer::checkMaskUp (fixed yOffset) {

	return level->checkMaskUp(x + PXO_ML, y + yOffset) ||
		level->checkMaskUp(x + PXO_MID, y + yOffset) ||
		level->checkMaskUp(x + PXO_MR, y + yOffset);

}


/**
 * Move the player to the ground's surface.
 */
void JJ1LevelPlayer::ground () {

	// If on an uphill slope, push the player upwards
	if (checkMaskUp(0) && !checkMaskUp(-F4)) y -= F4;

	// If on a downhill slope, push the player downwards
	if (!checkMaskUp(F4) && checkMaskUp(F8)) y += F4;

	return;

}


/**
 * Respond to controls, unless the player has been killed.
 *
 * @param ticks Time
 * @param msps Ticks per step
 */
void JJ1LevelPlayer::control (unsigned int ticks, int msps) {

	int speed;
	bool platform;


	// If the player has been killed, drop but otherwise do not move
	if (!energy) {

		dx = 0;

		if (floating) dy = 0;
		else {

			dy += PYA_GRAVITY * msps;
			if (dy > PYS_FALL) dy = PYS_FALL;

		}

		animType = facing? PA_RDIE: PA_LDIE;

		return;

	}

	if (player->pcontrols[C_RIGHT]) {

		// Walk/run right

		if (dx < 0) dx += PXA_REVERSE * msps;
		else if (dx < PXS_WALK) dx += PXA_WALK * msps;
		else if (dx < PXS_RUN) dx += PXA_RUN * msps;

		facing = true;

	} else if (player->pcontrols[C_LEFT]) {

		// Walk/run left

		if (dx > 0) dx -= PXA_REVERSE * msps;
		else if (dx > -PXS_WALK) dx -= PXA_WALK * msps;
		else if (dx > -PXS_RUN) dx -= PXA_RUN * msps;

		facing = false;

	} else {

		// Slow down

		if (dx > 0) {

			if (dx < PXA_STOP * msps) dx = 0;
			else dx -= PXA_STOP * msps;

		} else if (dx < 0) {

			if (dx > -PXA_STOP * msps) dx = 0;
			else dx += PXA_STOP * msps;

		}

	}

	if (dx < -PXS_RUN) dx = -PXS_RUN;
	else if (dx > PXS_RUN) dx = PXS_RUN;


	// Check for platform event, bridge or level mask below player
	platform = (event == LPE_PLATFORM) ||
		checkMaskDown(F4) ||
		((dx > 0) && level->checkMaskDown(x + PXO_ML, y + F8)) ||
		((dx < 0) && level->checkMaskDown(x + PXO_MR, y + F8));


	if (floating) {

		if (player->pcontrols[C_UP]) {

			// Fly upwards

			if (dy > 0) dy -= PXA_REVERSE * msps;
			else if (dy > -PXS_WALK) dy -= PXA_WALK * msps;
			else if (dy > -PXS_RUN) dy -= PXA_RUN * msps;

		} else if (player->pcontrols[C_DOWN]) {

			// Fly downwards

			if (dy < 0) dy += PXA_REVERSE * msps;
			else if (dy < PXS_WALK) dy += PXA_WALK * msps;
			else if (dy < PXS_RUN) dy += PXA_RUN * msps;

		} else {

			// Slow down

			if (dy > 0) {

				if (dy < PXA_STOP * msps) dy = 0;
				else dy -= PXA_STOP * msps;

			} else if (dy < 0) {

				if (dy > -PXA_STOP * msps) dy = 0;
				else dy += PXA_STOP * msps;

			}

		}

		if (event != LPE_NONE) {

			if (event == LPE_SPRING) dy = level->getEvent(eventX, eventY)->multiA * -F20;
			else if (event == LPE_FLOAT) dy = PYS_JUMP;

		}

		if (dy < -PXS_RUN) dy = -PXS_RUN;
		else if (dy > PXS_RUN) dy = PXS_RUN;

	} else if (y + PYO_MID > level->getWaterLevel()) {

		if (player->pcontrols[C_SWIM]) {

			// Swim upwards

			if (dy > 0) dy -= PXA_REVERSE * msps;
			else if (dy > -PXS_WALK) dy -= PXA_WALK * msps;
			else if (dy > -PXS_RUN) dy -= PXA_RUN * msps;

			// Prepare to jump upon leaving the water

			if (!level->checkMaskUp(x + PXO_MID, y - F36)) {

				jumpY = y - jumpHeight;

				if (dx < 0) jumpY += dx >> 4;
				else if (dx > 0) jumpY -= dx >> 4;

				event = LPE_NONE;

			}

		} else if (player->pcontrols[C_DOWN]) {

			// Swim downwards

			if (dy < 0) dy += PXA_REVERSE * msps;
			else if (dy < PXS_WALK) dy += PXA_WALK * msps;
			else if (dy < PXS_RUN) dy += PXA_RUN * msps;

		} else {

			// Sink

			dy += PYA_SINK * msps;
			if (dy > PYS_SINK) dy = PYS_SINK;

		}

		if (dy < -PXS_RUN) dy = -PXS_RUN;
		else if (dy > PXS_RUN) dy = PXS_RUN;

	} else {

		if (platform && player->pcontrols[C_JUMP] &&
			!level->checkMaskUp(x + PXO_MID, y - F36)) {

			// Jump

			jumpY = y - jumpHeight;

			// Increase jump height if walking/running
			if (dx < 0) jumpY += dx >> 3;
			else if (dx > 0) jumpY -= dx >> 3;

			event = LPE_NONE;

			playSound(S_JUMPA);

		}

		// Stop jumping
		if (!player->pcontrols[C_JUMP] && (event != LPE_SPRING) && (event != LPE_FLOAT))
			jumpY = TTOF(LH);

		if (y >= jumpY) {

			// If jumping, rise
			dy = (jumpY - y - F64) * 4;

			// Spring/float up speed limit
			if ((event == LPE_SPRING) || (event == LPE_FLOAT)) {

				speed = level->getEvent(eventX, eventY)->multiA * -F20;

				if (speed >= 0) speed = PYS_JUMP;

				if (dy < speed) dy = speed;

			}

			// Avoid jumping too fast, unless caused by an event
			if ((event == LPE_NONE) && (dy < PYS_JUMP)) dy = PYS_JUMP;

		} else if (!platform) {

			// Fall under gravity
			dy += PYA_GRAVITY * msps;
			if (dy > PYS_FALL) dy = PYS_FALL;

		}

		// Don't descend through platforms
		if ((dy > 0) && (event == LPE_PLATFORM)) dy = 0;

		if (platform && !lookTime) {

			// If requested, look up or down
			if (player->pcontrols[C_UP]) lookTime = -ticks;
			else if (player->pcontrols[C_DOWN]) lookTime = ticks;

		}

		// Stop looking if there is no platform or the control has been released
		if (!platform ||
			(!player->pcontrols[C_UP] && (lookTime < 0)) ||
			(!player->pcontrols[C_DOWN] && (lookTime > 0))) lookTime = 0;

	}

	// If there is an obstacle above and the player is not floating up, stop
	// rising
	if (checkMaskUp(PYO_TOP - F4) && (jumpY < y) && (event != LPE_FLOAT)) {

		jumpY = TTOF(LH);
		if (dy < 0) dy = 0;

		if (event != LPE_PLATFORM) event = LPE_NONE;

	}

	// If jump completed, stop rising
	if (y <= jumpY) {

		jumpY = TTOF(LH);

		if (event != LPE_PLATFORM) event = LPE_NONE;

	}


	// Handle firing
	if (player->pcontrols[C_FIRE]) {

		if ((ticks > fireTime) && (level->getBullet(player->ammoType + 1)[B_SPRITE] != 0)) {

			// Make sure bullet position is taken from correct animation
			if (platform) animType = facing? PA_RSHOOT: PA_LSHOOT;

			// Create new bullet
			level->bullets = new JJ1Bullet(this, false, ticks);

			// Set when the next bullet can be fired
			if (player->fireSpeed) fireTime = ticks + (1000 / player->fireSpeed);
			else fireTime = 0x7FFFFFFF;

			// Remove the bullet from the arsenal
			if (player->ammoType != -1) player->ammo[player->ammoType]--;

			/* If the current ammo type has been exhausted or TNT has been used,
			use the previous non-exhausted ammo type */
			while (((player->ammoType > -1) && !player->ammo[player->ammoType]) || (player->ammoType == 3)) player->ammoType--;

		}

	} else fireTime = 0;


	// Check for a change in ammo
	if (player->pcontrols[C_CHANGE]) {

		if (player == localPlayer) controls.release(C_CHANGE);

		player->ammoType = ((player->ammoType + 2) % 5) - 1;

		// If there is no ammo of this type, go to the next type that has ammo
		while ((player->ammoType > -1) && !player->ammo[player->ammoType])
			player->ammoType = ((player->ammoType + 2) % 5) - 1;

	}


	// Deal with the bird

	if (birds) birds = birds->step(ticks, msps);


	// Choose animation

	if ((reaction == PR_HURT) && (reactionTime - ticks > PRT_HURT - PRT_HURTANIM))
		animType = facing? PA_RHURT: PA_LHURT;

	else if (y + PYO_MID > level->getWaterLevel())
		animType = facing? PA_RSWIM: PA_LSWIM;

	else if (floating) animType = facing? PA_RBOARD: PA_LBOARD;

	else if (dy < 0) {

		if (event == LPE_SPRING) animType = facing? PA_RSPRING: PA_LSPRING;
		else animType = facing? PA_RJUMP: PA_LJUMP;

	} else if (platform) {

		if (dx) {

			if (dx <= -PXS_RUN) animType = PA_LRUN;
			else if (dx >= PXS_RUN) animType = PA_RRUN;
			else if ((dx < 0) && facing) animType = PA_LSTOP;
			else if ((dx > 0) && !facing) animType = PA_RSTOP;
			else animType = facing? PA_RWALK: PA_LWALK;

		} else if (!level->checkMaskDown(x + PXO_ML, y + F20) &&
			!level->checkMaskDown(x + PXO_L, y + F2) &&
			(event != LPE_PLATFORM))
			animType = PA_LEDGE;

		else if (!level->checkMaskDown(x + PXO_MR, y + F20) &&
			!level->checkMaskDown(x + PXO_R, y + F2) &&
			(event != LPE_PLATFORM))
			animType = PA_REDGE;

		else if ((lookTime < 0) && ((int)ticks > 1000 - lookTime))
			animType = PA_LOOKUP;

		else if (lookTime > 0) {

			if ((int)ticks < 1000 + lookTime) animType = facing? PA_RCROUCH: PA_LCROUCH;
			else animType = PA_LOOKDOWN;

		}

		else if (player->pcontrols[C_FIRE])
			animType = facing? PA_RSHOOT: PA_LSHOOT;

		else
			animType = facing? PA_RSTAND: PA_LSTAND;

	} else animType = facing? PA_RFALL: PA_LFALL;


	return;

}


/**
 * Move the player.
 *
 * @param ticks Time
 * @param msps Ticks per step
 */
void JJ1LevelPlayer::move (unsigned int ticks, int msps) {

	fixed pdx, pdy;
	bool grounded = false;
	int count;

	if (warpTime && (ticks > warpTime)) {

		x = TTOF(warpX);
		y = TTOF(warpY + 1);
		warpTime = 0;

	}

	// Apply as much of the trajectory as possible, without going into the
	// scenery

	if (fastFeetTime > ticks) {

		pdx = (dx * msps * 3) >> 11;
		pdy = (dy * msps * 3) >> 11;

	} else {

		pdx = (dx * msps) >> 10;
		pdy = (dy * msps) >> 10;

	}

	// First for the vertical component of the trajectory

	if (pdy < 0) {

		// Moving up

		count = (-pdy) >> 12;

		while (count > 0) {

			if (checkMaskUp(PYO_TOP - F4)) {

				y &= ~4095;
				dy = 0;

				break;

			}

			y -= F4;
			count--;

		}

		pdy = (-pdy) & 4095;

		if (checkMaskUp(PYO_TOP - pdy)) {

			y &= ~4095;
			dy = 0;

		} else y -= pdy;

	} else {

		if (pdy > 0) {

			// Moving down

			count = pdy >> 12;

			while (count > 0) {

				if (checkMaskDown(F4)) {

					y |= 4095;
					dy = 0;

					break;

				}

				y += F4;
				count--;

			}

			pdy &= 4095;

			if (checkMaskDown(pdy)) {

				y |= 4095;
				dy = 0;

			} else y += pdy;

		}

		if (!floating) {

			if (checkMaskDown(0)) {

				// In the ground, so move up
				if (y >= 4096) y = (y - 4096) | 4095;
				grounded = true;

			} else if (checkMaskDown(1)) {

				// On the ground
				grounded = true;

			}

		}

	}



	// Then for the horizontal component of the trajectory

	if (pdx < 0) {

		// Moving left

		count = (-pdx) >> 12;

		while (count > 0) {

			// If there is an obstacle, stop
			if (level->checkMaskUp(x + PXO_L - F4, y + PYO_MID)) {

				x &= ~4095;
				dx = 0;

				break;

			}

			x -= F4;
			count--;

			if (grounded) ground();

		}

		pdx = (-pdx) & 4095;

		if (level->checkMaskUp(x + PXO_L - pdx, y + PYO_MID)) {

			x &= ~4095;
			dx = 0;

		} else x -= pdx;

		if (grounded) ground();

	} else if (pdx > 0) {

		// Moving right

		count = pdx >> 12;

		while (count > 0) {

			// If there is an obstacle, stop
			if (level->checkMaskUp(x + PXO_R + F4, y + PYO_MID)) {

				x |= 4095;
				dx = 0;

				break;

			}

			x += F4;
			count--;

			if (grounded) ground();

		}

		pdx &= 4095;

		if (level->checkMaskUp(x + PXO_R + pdx, y + PYO_MID)) {

			x |= 4095;
			dx = 0;

		} else x += pdx;

		if (grounded) ground();

	}


	// If using a float up event and have hit a ceiling, ignore event
	if ((event == LPE_FLOAT) && level->checkMaskUp(x + PXO_MID, y + PYO_TOP - F4)) {

		jumpY = TTOF(LH);
		event = LPE_NONE;

	}


	if (level->getStage() == LS_END) return;


	// If the player has hit the bottom of the level, kill
	if (y + F4 > TTOF(LH)) kill(NULL, ticks);


	// Handle spikes
	if (level->checkSpikes(x + PXO_MID, y + PYO_TOP - F4) ||
		level->checkSpikes(x + PXO_MID, y + F4) ||
		level->checkSpikes(x + PXO_L - F4, y + PYO_MID) ||
		level->checkSpikes(x + PXO_R + F4, y + PYO_MID)) hit(NULL, ticks);


	return;

}


/**
 * Calculate viewport.
 *
 * @param ticks Time
 * @param mspf Ticks per frame
 * @param change Time since last step
 */
void JJ1LevelPlayer::view (unsigned int ticks, int mspf, int change) {

	int oldViewX, oldViewY, speed;

	// Record old viewport position for applying lag
	oldViewX = viewX;
	oldViewY = viewY;

	// Find new position

	viewX = x + ((dx * change) >> 10) + F8 - (canvasW << 9);
	viewY = y + ((dy * change) >> 10) - F24 - ((canvasH - 33) << 9);

	if ((lookTime > 0) && ((int)ticks > 1000 + lookTime)) {

		// Look down
		if ((int)ticks < 2000 + lookTime) viewY += 64 * (ticks - (1000 + lookTime));
		else viewY += F64;

	} else if ((lookTime < 0) && ((int)ticks > 1000 - lookTime)) {

		// Look up
		if ((int)ticks < 2000 - lookTime) viewY -= 64 * (ticks - (1000 - lookTime));
		else viewY -= F64;

	}


	// Apply lag proportional to player "speed"
	speed = ((dx >= 0? dx: -dx) + (dy >= 0? dy: -dy)) >> 14;

	if (speed && (mspf < speed)) {

		viewX = ((oldViewX * (speed - mspf)) + (viewX * mspf)) / speed;
		viewY = ((oldViewY * (speed - mspf)) + (viewY * mspf)) / speed;

	}


	return;

}

/**
 * Draw the player.
 *
 * @param ticks Time
 * @param change Time since last step
 */
void JJ1LevelPlayer::draw (unsigned int ticks, int change) {

	Anim *an;
	int frame;
	fixed drawX, drawY;
	fixed xOffset, yOffset;

	// The current frame for animations
	if (reaction == PR_KILLED) frame = (ticks + PRT_KILLED - reactionTime) / 75;
	else frame = ticks / 75;


	// Get position

	drawX = getDrawX(change);
	drawY = getDrawY(change);


	// Choose sprite

	an = anims[animType];
	an->setFrame(frame, reaction != PR_KILLED);


	// Show the player

	// Flash red if hurt, otherwise use player colour
	if ((reaction == PR_HURT) && (!((ticks / 30) & 3)))
		an->flashPalette(36);

	else {

		an->setPalette(palette, 23, 41);
		an->setPalette(palette, 88, 8);

	}


	// Draw "motion blur"
	if (fastFeetTime > ticks) an->draw(drawX - (dx >> 6), drawY);

	// Draw player
	an->draw(drawX, drawY);


	// Remove red flash or player colour from sprite
	an->restorePalette();


	// Uncomment the following to see the area of the player
	/*drawRect(FTOI(drawX + PXO_L),
		FTOI(drawY + PYO_TOP),
		FTOI(PXO_R - PXO_L),
		FTOI(-PYO_TOP), 89);
	drawRect(FTOI(drawX + PXO_ML),
		FTOI(drawY + PYO_TOP),
		FTOI(PXO_MR - PXO_ML),
		FTOI(-PYO_TOP), 88);*/

	// Uncomment the following to show the player's event tile
	// if (event != LPE_NONE) drawRect(FTOI(TTOF(eventX) - viewX), FTOI(TTOF(eventY) - viewY), 32, 32, 89);


	if (reaction == PR_INVINCIBLE) {

		// Show invincibility stars

		xOffset = fSin(ticks * 2) * 12;
		yOffset = fCos(ticks * 2) * 12;

		an = level->getMiscAnim(0);
		an->disableDefaultOffset();

		an->setFrame(frame, true);
		an->draw(drawX + PXO_MID + xOffset, drawY + PYO_MID + yOffset);

		an->setFrame(frame + 1, true);
		an->draw(drawX + PXO_MID - xOffset, drawY + PYO_MID - yOffset);

		an->setFrame(frame + 2, true);
		an->draw(drawX + PXO_MID + yOffset, drawY + PYO_MID + xOffset);

		an->setFrame(frame + 3, true);
		an->draw(drawX + PXO_MID - yOffset, drawY + PYO_MID - xOffset);

	} else if (shield > 2) {

		// Show the 4-hit shield

		xOffset = fCos(ticks) * 20;
		yOffset = fSin(ticks) * 20;

		an = level->getAnim(59);

		an->draw(drawX + xOffset, drawY + PYO_TOP + yOffset);

		if (shield > 3) an->draw(drawX - xOffset, drawY + PYO_TOP - yOffset);

		if (shield > 4) an->draw(drawX + yOffset, drawY + PYO_TOP - xOffset);

		if (shield > 5) an->draw(drawX - yOffset, drawY + PYO_TOP + xOffset);

	} else if (shield) {

		// Show the 2-hit shield

		xOffset = fCos(ticks) * 20;
		yOffset = fSin(ticks) * 20;

		an = level->getAnim(50);

		an->draw(drawX + xOffset, drawY + yOffset + PYO_TOP);

		if (shield == 2) an->draw(drawX - xOffset, drawY + PYO_TOP - yOffset);

	}


	// Show the bird
	if (birds) birds->draw(ticks, change);


	// Show the player's name
	if (nPlayers > 1)
		panelBigFont->showString(player->name,
			FTOI(drawX + PXO_MID) - (panelBigFont->getStringWidth(player->name) >> 1),
			FTOI(drawY - F32 - F16));

	return;

}


