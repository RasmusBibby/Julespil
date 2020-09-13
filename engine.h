#ifndef _GAMEENGINE_H
#define _GAMEENGINE_H
/** Included files **/

#include "base.h"

#include "tile.h"

#include <vector>
#include <string>

using namespace std;

#define MAX_THEMES 5
#define MAX_LEVELS 1

class	c_image;
class	c_level;
class	c_gamearea;

class c_engine
{
	private:
		static c_engine	*m_pEngine;

		// Multimedia Data
		c_image		*m_pTransImage;
		c_image		*m_pHealthIndicator;
		c_image		*m_pCheckpoint;

		string		m_sTheme[MAX_THEMES];

		// Level Data
		c_level	*m_pLevel[MAX_LEVELS];
		c_level	*m_pCurrentLevel;
		c_level	*m_pNextLevel;
		bool	m_bUseTransistion;
		int		m_iLevelTransAlpha;

		// Game Data
		int		m_startX, m_startY; // Start location for the player (if the player dies)
		short	m_health;
	public:
		c_engine();
		~c_engine();

		static c_engine	*getEngine();

		// General Members
		bool	loadMedia();

		void	drawHealth(int x, int y);
		void	drawTransistion();

		// Level Members
		void	addLevel(c_level *level);
		void	changeLevel();
		void	gotoLevel(int index, bool transistion = true);

		// Game Mechanic Members
		bool	loseHealth(short damage);
		void	gainHealth(short heal);

		// Accessor Members
		c_image	*getCheckpointSprite()const;
		string	getThemeName(uint index)const;
		c_level	*currentLevel()const;
		void	setCheckPoint(int x, int y);
		s_point	getCheckPointPosition();
};

#endif
