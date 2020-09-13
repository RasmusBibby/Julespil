#include "engine.h"

#include "level.h"
#include "multimedia.h"

#include <iostream>
using namespace std;

c_engine *c_engine::m_pEngine = NULL;

c_engine::c_engine()
{
	m_pEngine = this;

	// Setup media data
	m_pTransImage = new c_image();
	m_pTransImage->createBlank(640, 480, 0, 0, 0);

	m_pHealthIndicator = NULL;
	m_pCheckpoint = NULL;

	// Setup level data
	for(int i = 0; i < MAX_LEVELS; i++)
	{
		m_pLevel[i] = NULL;
	}
	m_pCurrentLevel = NULL;
	m_pNextLevel = NULL;
	m_bUseTransistion = false;
	m_iLevelTransAlpha = 255;

	// Setup game data
	m_health = 10;
}
c_engine::~c_engine()
{
	// Free multimedia data
	delete m_pTransImage;

	if(m_pHealthIndicator) delete m_pHealthIndicator;
	if(m_pCheckpoint) delete m_pCheckpoint;

	// Free the levels from memory
	for(int i = 0; i < MAX_LEVELS; i++)
	{
		if(m_pLevel[i])
		{
			delete m_pLevel[i];
			m_pLevel[i] = NULL;
		}
	}
}

c_engine *c_engine::getEngine() { return m_pEngine; }

/// General Members
bool c_engine::loadMedia()
{
	// Load images
	m_pHealthIndicator = new c_image();
	if(m_pHealthIndicator->load("data/images/honey_heart.png") == false)
	{
		return false;
	}
	m_pCheckpoint = new c_image();
	if(m_pCheckpoint->load("data/images/spr_CheckPoint.png", 5) == false)
	{
		return false;
	}

	// Add the themes
	m_sTheme[0] = "data/tilesets/forest.tst";
	m_sTheme[1] = "data/tilesets/mountain.tst";
	m_sTheme[2] = "data/tilesets/volcano.tst";
	m_sTheme[3] = "data/tilesets/factory.tst";
	m_sTheme[4] = "data/tilesets/castle.tst";

	return true;
}

void c_engine::drawHealth(int x, int y)
{
	m_pHealthIndicator->draw(x, y);
	for(short i = 0; i < m_health; i++)
	{
		m_pHealthIndicator->draw(x + 20 * i, y);
	}
}
void c_engine::drawTransistion()
{
	if(m_iLevelTransAlpha > 0)
	{
		m_pTransImage->setAlpha((uchar)m_iLevelTransAlpha);
		m_pTransImage->draw(0, 0);
	}
}

/// Level Members
void c_engine::addLevel(c_level *level)
{
	for(int i = 0; i < MAX_LEVELS; i++)
	{
		if(m_pLevel[i] == NULL)
		{
			m_pLevel[i] = level;
			return;
		}
	}
}
void c_engine::changeLevel()
{
	if(m_pNextLevel)
	{
		// Execute the end event of the current level before starting the new level
		if((m_bUseTransistion && (int)m_iLevelTransAlpha + 5 >= 255) || !m_bUseTransistion)
		{
			if(m_pCurrentLevel) m_pCurrentLevel->ev_end();

			m_pCurrentLevel = m_pNextLevel;
			m_pNextLevel = NULL;

			m_pCurrentLevel->ev_begin();
			m_bUseTransistion = false;
		}
		else if(m_bUseTransistion)
		{
			m_iLevelTransAlpha += 5;
		}
	}
	else
	{
		// Fade back to normal
		if((int)m_iLevelTransAlpha - 5 > 0) m_iLevelTransAlpha -= 5;
		else m_iLevelTransAlpha = 0;
	}
}
void c_engine::gotoLevel(int index, bool transistion)
{
	if(index >= MAX_LEVELS) return;

	m_pNextLevel = m_pLevel[index];
	m_bUseTransistion = transistion;
}

/// Game Members
bool c_engine::loseHealth(short damage)
{
	m_health -= damage;
	if(m_health <= 0)
	{
		// Lose a life
		m_health = 10;
		return true;
	}
	return false;
}
void c_engine::gainHealth(short heal)
{
	m_health += heal;
}

/// Accessor Members
c_image *c_engine::getCheckpointSprite()const { return m_pCheckpoint; }
string c_engine::getThemeName(uint index)const { return m_sTheme[index]; }
c_level *c_engine::currentLevel()const { return m_pCurrentLevel; }
void c_engine::setCheckPoint(int x, int y)
{
	m_startX = x;
	m_startY = y;
}
s_point c_engine::getCheckPointPosition()
{
	s_point pos = {(float)m_startX, (float)m_startY};
	return pos;
}
