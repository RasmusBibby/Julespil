#include "platform.h"
#include "gamearea.h"

#include <vector>
#include <iostream>

using namespace std;

vector<c_platformtype*>	g_vPlatform;

/** Implement the c_platformtype Class Methods **/
c_platformtype::c_platformtype(const char *fname, short width, short height, char type)
{
	m_sprite = new c_image();
	if(m_sprite->load(fname) == false)
	{
		cerr << "Unable to load " << fname << " to c_platformtype!" << endl;
		delete m_sprite;
		m_sprite = nullptr;
	}
	m_bbox.left = 0;
	m_bbox.right = width * 32;
	m_bbox.top = 0;
	m_bbox.bottom = height * 32;

	m_type = type;
}
c_platformtype::~c_platformtype()
{
	if(m_sprite) delete m_sprite;
}

/** Implement the c_platform Class Methods **/
c_platform::c_platform()
{
	m_type = 0;

	m_velocity.x = m_velocity.y = 0;
}
c_platform::~c_platform() {}

// Event methods
void c_platform::onMovement(int direction) {}
void c_platform::onTileCollision(uint x, uint y, s_tile *pTile)
{
	if(platform_get(m_type)->getType() == PLATFORM_PUSHABLE)
	{
		// Don't go through walls

		// Move the entity
		s_bbox bbox = getBBox();

		/*int xVel = 0;
		if(m_oldPosition.x < m_position.x) xVel = -1;
		else if(m_oldPosition.x > m_position.x) xVel = 1;

		if(xVel != 0)
		{
			for(int i = 0; i < GRID_W; i++)
			{
				if(c_gamearea::level()->bbox_inTile(x, y, m_position.x + xVel, m_position.y, bbox))
					m_position.x += xVel;
				else
					break;
			}
		}
		m_velocity.x = 0;*/

		int yVel = 0;
		if(m_oldPosition.y < m_position.y) yVel = -1;
		else if(m_oldPosition.y > m_position.y) yVel = 1;

		if(yVel != 0)
		{
			for(int i = 0; i < GRID_H; i++)
			{
				if(c_gamearea::level()->bbox_inTile(x, y, m_position.x, m_position.y + yVel, bbox))
					m_position.y += yVel;
				else
					break;
			}
		}
		m_velocity.y = 0;
	}
}
void c_platform::onCollision(c_entity *pOther)
{
	if(platform_get(m_type)->getType() == PLATFORM_PUSHABLE)
	{
		if(pOther->getEntityType() == ENTITY_TYPE_PLATFORM)
		{
			// Don't go through walls
			//c_platform *platform = (c_platform*)pOther;
			// Move the entity
			s_bbox bbox = getBBox();
			s_bbox boxB = pOther->getBBox();
			s_point pos = pOther->getPosition();

			if(m_velocity.y > 0)
			{
				// Stop the box from passing through
				if(m_position.y > m_oldPosition.y && m_position.y + bbox.bottom > pos.y - boxB.top)
				{
					m_position.y = pos.y - boxB.top - bbox.bottom;
					m_velocity.y = 0;
				}
			}
		}
	}
}

void c_platform::ev_cycle()
{
	if(platform_get(m_type)->getType() == PLATFORM_PUSHABLE)
	{
		// Affect the box with gravity
		if(!c_gamearea::level()->bbox_inTile(m_position.x, m_position.y + 1, getBBox()))
		{
			if(m_velocity.y < 15) m_velocity.y++;
		}
		else
		{
			//m_velocity.y = 0;
		}

		// Stop horizontal velocity then meeting a wall
		if(m_velocity.x != 0)
		{
			if(c_gamearea::level()->bbox_inTile(m_position.x + m_velocity.x, m_position.y - 1, getBBox()) == true)
			{
				m_velocity.x = 0;
			}
		}

		// Make friction
		float friction = 0.05f;
		s_environment env;
		if(m_velocity.x >= -0.5f && m_velocity.x <= 0.5f)
		{
			m_velocity.x = 0.0f;
		}

		if(m_velocity.x > 0) m_velocity.x -= friction;
		else if(m_velocity.x < 0) m_velocity.x += friction;
	}
}
void c_platform::ev_cycleEnd()
{
}
void c_platform::ev_render(int viewX, int viewY)
{
	s_bbox bbox = getBBox();

	if(viewX + 640 < m_position.x - bbox.left) return;
	if(viewY + 640 < m_position.y - bbox.top) return;
	if(viewX >= m_position.x + bbox.right) return;
	if(viewY >= m_position.y + bbox.bottom) return;

	//draw_rect(m_position.x - bbox.left - viewX, m_position.y - bbox.top - viewY, bbox.left + bbox.right, bbox.top + bbox.bottom, 0, 0xFF, 0x0, 128);

	platform_get(m_type)->getImage()->draw(m_position.x - viewX, m_position.y - viewY, (uint)floor(m_curFrame));
}

// General methods
bool c_platform::load(ifstream &ff)
{
	ushort x, y;
	int type;

	ff.read(reinterpret_cast<char*>(&x), sizeof(x));
	ff.read(reinterpret_cast<char*>(&y), sizeof(y));
	ff.read(reinterpret_cast<char*>(&type), sizeof(type));

	m_type = (uint)type;

	s_bbox bbox = platform_get(m_type)->getBBox();

	m_position.x = (float)x * 32 + bbox.left;
	m_position.y = (float)y * 32 + bbox.top;
	m_oldPosition = m_position;

	return true;
}

// Accessor methods
int c_platform::getDepth()const { return 50; }
s_bbox c_platform::getBBox()const
{
	return platform_get(m_type)->getBBox();
}
uint c_platform::numFrames()const { return 1; }

/** Implement Global Functions **/
void platform_add(c_platformtype *p)
{
	g_vPlatform.push_back(p);
}
void platform_free()
{
	for(uint i = 0; i < g_vPlatform.size(); i++)
	{
		delete g_vPlatform[i];
	}
}
c_platformtype *platform_get(uint x)
{
	return g_vPlatform[x];
}
