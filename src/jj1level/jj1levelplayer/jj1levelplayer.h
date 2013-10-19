
/**
 *
 * @file jj1levelplayer.h
 *
 * Part of the OpenJazz project
 *
 * @section History
 * 23rd August 2005: Created OpenJazz.h
 * 31st January 2006: Created player.h from parts of OpenJazz.h
 * 24th June 2010: Created levelplayer.h from parts of player.h
 * 29th June 2010: Created jj2levelplayer.h from parts of levelplayer.h
 * 1st August 2012: Renamed levelplayer.h to jj1levelplayer.h
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
 */


/* "Tile" is a flexible term. Here it is used to refer specifically to the
   individual elements of the tile set.
   "Tiles" in the context of level units are referred to as grid elements. */


#ifndef _JJ1LEVELPLAYER_H
#define _JJ1LEVELPLAYER_H


#include "../jj1level.h"

#include "level/levelplayer.h"
#include "player/player.h"
#include "OpenJazz.h"

#include <SDL/SDL.h>


// Constants

// Colour offsets
#define PCO_GREY   0
#define PCO_SGREEN 16
#define PCO_BLUE   23
#define PCO_RED    32
#define PCO_LGREEN 48
#define PCO_LEVEL1 64
#define PCO_YELLOW 75
#define PCO_LEVEL2 80
#define PCO_ORANGE 88
#define PCO_LEVEL3 96
#define PCO_LEVEL4 104
#define PCO_SANIM  112
#define PCO_LANIM  116
#define PCO_LEVEL5 124

// Player reaction times
#define PRT_HURT       1000
#define PRT_HURTANIM   200
#define PRT_KILLED     2000
#define PRT_INVINCIBLE 10000

// Other time periods
#define T_FASTFEET 25000
#define T_WARP     1000

// Player offsets
#define PXO_MID F16
#define PXO_L    (PXO_MID - F12)
#define PXO_ML   (PXO_MID - F8)
#define PXO_MR   (PXO_MID + F8)
#define PXO_R    (PXO_MID + F12)
#define PYO_TOP  (-F20)
#define PYO_MID  (-F10)
#define PYO_JUMP ITOF(84)

// Player speeds
#define PXS_WALK  ITOF(300)
#define PXS_RUN   ITOF(325)
#define PYS_FALL  ITOF(350)
#define PYS_SINK  ITOF(150)
#define PYS_JUMP  ITOF(-350)

// Player accelerations
#define PXA_REVERSE 1800
#define PXA_STOP    1000
#define PXA_WALK    1000
#define PXA_RUN     200
#define PYA_GRAVITY 2750
#define PYA_SINK    1000


// Enum

/// JJ1 player reaction type
enum JJ1PlayerReaction {

	PR_NONE, ///< Not reacting
	PR_HURT, ///< Hurt
	PR_KILLED, ///< Killed
	PR_INVINCIBLE ///< Invincibility

};


// Classes

class Anim;
class JJ1Bird;

/// JJ1 level player
class JJ1LevelPlayer : public LevelPlayer {

	private:
		JJ1Bird*          birds; ///< Bird companion(s)
		Anim*             anims[JJ1PANIMS]; ///< Animations
		int               energy; ///< 0 = dead, 4 = maximum
		int               shield; ///< 0 = none, 1 = 1 yellow, 2 = 2 yellow, 3 = 1 orange, 4 = 2 orange, 5 = 3 orange, 6 = 4 orange
		bool              floating; ///< false = normal, true = boarding/bird/etc.
		bool              facing; ///< false = left, true = right
		unsigned char     animType; ///< Current animation
		unsigned char     eventX; ///< X-coordinate (in tiles) of an event (spring, platform, bridge)
		unsigned char     eventY; ///< Y-coordinate (in tiles) of an event (spring, platform, bridge)
		PlayerEvent       event; ///< Event type
		int               lookTime; ///< Negative if looking up, positive if looking down, 0 if neither
		JJ1PlayerReaction reaction; ///< Reaction type
		unsigned int      reactionTime; ///< Time reaction will end
		unsigned int      fireTime; ///< The next time the player can fire
		fixed             jumpHeight; ///< The height the player can reach when jumping
		fixed             jumpY; ///< Having started jumping, the y-coordinate the player can reach
		unsigned int      fastFeetTime; ///< Time fast feet will expire
		unsigned char     warpX; ///< X-coordinate (in tiles) player will warp to
		unsigned char     warpY; ///< Y-coordinate (in tiles) player will warp to
		unsigned int      warpTime; ///< Time the warp will happen
		int               enemies; ///< Number of enemies killed
		int               items; ///< Number of items collected
		bool              gem; ///< Bonus level gem collected

		bool checkMaskDown (fixed yOffset);
		bool checkMaskUp   (fixed yOffset);

		void ground ();

	public:
		JJ1LevelPlayer  (Player* parent, Anim** newAnims, unsigned char startX, unsigned char startY, int flockSize);
		~JJ1LevelPlayer ();

		void reset (int startX, int startY);

		void              addItem     ();
		void              clearEvent  (unsigned char gridX, unsigned char gridY);
		int               countBirds  ();
		Anim*             getAnim     ();
		int               getEnemies  ();
		int               getEnergy   ();
		bool              getFacing   ();
		int               getItems    ();
		bool              hasGem      ();
		bool              hit         (Player* source, unsigned int ticks);
		void              kill        (Player* source, unsigned int ticks);
		bool              overlap     (fixed left, fixed top, fixed width, fixed height);
		JJ1PlayerReaction reacted     (unsigned int ticks);
		void              setEvent    (unsigned char gridX, unsigned char gridY);
		void              setPosition (fixed newX, fixed newY);
		void              setSpeed    (fixed newDx, fixed newDy);
		bool              takeEvent   (unsigned char gridX, unsigned char gridY, unsigned int ticks);
		bool              touchEvent  (unsigned char gridX, unsigned char gridY, unsigned int ticks, int msps);

		void           send        (unsigned char* buffer);
		void           receive     (unsigned char* buffer);

		void           control     (unsigned int ticks, int msps);
		void           move        (unsigned int ticks, int msps);
		void           view        (unsigned int ticks, int mspf, int change);
		void           draw        (unsigned int ticks, int change);

};

#endif

