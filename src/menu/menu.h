
/**
 *
 * @file menu.h
 *
 * Part of the OpenJazz project
 *
 * @section History
 * 3rd February 2009: Created menu.h from parts of OpenJazz.h
 *
 * @section Licence
 * Copyright (c) 2005-2011 Alister Thomson
 *
 * OpenJazz is distributed under the terms of
 * the GNU General Public License, version 2.0
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef _MENU_H
#define _MENU_H


#include "game/gamemode.h"
#include "io/file.h"

#include "OpenJazz.h"

#include <SDL/SDL.h>


// Constants

#define ESCAPE_STRING "(esc) quits"

// Demo timeout
#define T_DEMO 20000

// Number of configurable player colour ranges
#define PCOLOURS 4


// Classes

/// Menu base class, providing generic menu screens
class Menu {

	protected:
		void showEscString ();
		int  message       (const char* text);
		int  generic       (const char** optionNames, int options, int& chosen);
		int  textInput     (const char* request, char*& text);

};

/// New game menus
class GameMenu : public Menu {

	private:
		SDL_Surface*  episodeScreens[11]; ///< Episode images
		SDL_Surface*  difficultyScreen; ///< 4 difficulty images
		SDL_Color     palette[256]; ///< Episode selection palette
		SDL_Color     greyPalette[256]; ///< Greyed-out episode selection palette
		int           episodes; ///< Number of episodes
		unsigned char difficulty; ///< Difficulty setting (0 = easy, 1 = medium, 2 = hard, 3 = turbo (hard in JJ2 levels))

		int playNewGame       (GameModeType mode, char* firstLevel);
		int newGameDifficulty (GameModeType mode, char* firstLevel);
		int newGameDifficulty (GameModeType mode, int levelNum, int worldNum);
		int newGameLevel      (GameModeType mode);
		int selectEpisode     (GameModeType mode, int episode);
		int newGameEpisode    (GameModeType mode);
		int joinGame          ();

	public:
		GameMenu  (File* file);
		~GameMenu ();

		int newGame  ();
		int loadGame ();

};

/// Setup menus
class SetupMenu : public Menu {

	private:
		int setupKeyboard   ();
		int setupJoystick   ();
		int setupResolution ();
#ifdef SCALE
		int setupScaling    ();
#endif
		int setupSound      ();

	public:
		int setupMain ();

};

/// Configuration
class Setup {

	public:
		char*         characterName;
		unsigned char characterCols[PCOLOURS];
		bool          slowMotion;
		bool          manyBirds;
		bool          leaveUnneeded;

		Setup  ();
		~Setup ();

};

/// Main menu
class MainMenu : public Menu {

	private:
		SDL_Surface* background; ///< Menu image
		SDL_Surface* highlight; ///< Menu image with highlighted text
		SDL_Surface* logo; ///< OJ logo image
		GameMenu*    gameMenu; ///< New game menu
		SDL_Color    palette[256]; ///< Menu palette

		int select (int option);

	public:
		MainMenu  ();
		~MainMenu ();

		int main ();

};


// Variables

EXTERN Setup setup;
EXTERN SDL_Color menuPalette[256]; /// Palette used by most menu screens

#endif

