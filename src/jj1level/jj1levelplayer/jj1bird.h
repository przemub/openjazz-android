
/**
 *
 * @file jj1bird.h
 *
 * Part of the OpenJazz project
 *
 * @section History
 * 1st March 2009: Created bird.h from parts of events.h
 * 1st August 2012: Renamed bird.h to jj1bird.h
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


#ifndef _BIRD_H
#define _BIRD_H


#include "level/movable.h"
#include "OpenJazz.h"


// Constants

// Animations
#define BIRD_LEFTANIM  51
#define BIRD_RIGHTANIM 52

// Time interval
#define T_BIRD_FIRE 500


// Classes

class JJ1LevelPlayer;

/// JJ1 bird companion
class JJ1Bird : public Movable {

	private:
		JJ1Bird*        next;
		JJ1LevelPlayer* player; ///< Player that rescued the bird
		bool            fleeing; ///< Flying away, player having been shot
		unsigned int    fireTime; ///< Next time the bird will fire

		JJ1Bird* remove ();

	public:
		JJ1Bird  (JJ1Bird* birds, JJ1LevelPlayer* player, unsigned char gX, unsigned char gY);
		~JJ1Bird ();

		int             getFlockSize ();
		JJ1LevelPlayer* getPlayer    ();
		void            hit          ();
		JJ1Bird*        setFlockSize (int size);

		JJ1Bird*     step      (unsigned int ticks, int msps);
		void         draw      (unsigned int ticks, int change);

};

#endif

