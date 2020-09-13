#include "player.h"
#include "engine.h"
#include "multimedia.h"
#include "gamearea.h"
#include "tile.h"

#include "platform.h"

#include "event_handler.h"

#include <iostream> // Should be removed

using namespace std;

/// Declare global variables and pointers
c_playertype	*g_playerType = NULL;

/************************************************/
/**  Implement the c_playertype class methods  **/
/************************************************/
c_playertype::c_playertype()
{
	m_numAnimation = 0;

	m_image[0] = NULL;
	m_image[1] = NULL;
	m_animation = NULL;
}
c_playertype::~c_playertype()
{
	if(m_image[0]) delete m_image[0];
	if(m_image[1]) delete m_image[1];
	if(m_animation) delete[] m_animation;
}

/// General methods
bool c_playertype::createImage(const char *fname, uint frames, const char *fnameLeft)
{
	m_image[0] = new c_image();
	m_image[1] = new c_image();
	if(m_image[0]->load(fname, frames))
	{
		if(fnameLeft)
		{
			return m_image[1]->load(fnameLeft, frames);
		}
		else
		{
			m_image[1]->loadFlipped(fname, frames);
			return true;
		}
	}
	return false;
}
void c_playertype::setBBox(int left, int right, int top, int bottom)
{
	m_bbox.left = left; m_bbox.right = right;
	m_bbox.top = top; m_bbox.bottom = bottom;
}
void c_playertype::initAnimations(uint animations)
{
	m_numAnimation = animations;
	m_animation = new s_animation[m_numAnimation];
}
void c_playertype::setAnimation(uint animation, uint begframe, uint endframe)
{
	if(animation < m_numAnimation)
	{
		m_animation[animation].begframe = begframe;
		m_animation[animation].endframe = endframe;
	}
}

/************************************************/
/**    Implement the c_player class methods    **/
/************************************************/
c_player::c_player()
{
	m_type = 0;

	m_velocity.x = m_velocity.y = 0;

	m_acceleration = 0;
	m_lookRight = true;
	m_animationstate = PLAYER_STAND;

	m_climbing = false;
	m_freespace = false;

	// Set game stats
	m_immortal = 0;
	m_bVisible = true;
}
c_player::c_player(int x, int y)
{
	m_type = 0;

	m_position.x = x;
	m_position.y = y;
	m_oldPosition = m_position;
	m_velocity.x = m_velocity.y = 0;

	m_acceleration = 0;
	m_lookRight = true;
	m_animationstate = PLAYER_STAND;

	m_climbing = false;
	m_freespace = false;

	// Set game stats
	m_immortal = 0;
	m_bVisible = true;
}
c_player::~c_player()
{
}

/// Public Event methods
void c_player::onMovement(int direction)
{
	if(direction == MOVE_LEFT)
	{
		if(m_acceleration > -6) m_acceleration -= 2;
		else m_acceleration = -6;
		m_lookRight = false;

		if(m_animationstate == PLAYER_ROPE || (m_animationstate == PLAYER_LADDER && !c_gamearea::level()->ptIn_environment(m_position.x + m_acceleration, m_position.y, ENV_LADDER)))
		{
			m_climbing = false;
			setAnimation(PLAYER_MOVE, 0.0f);
		}
	}
	else if(direction == MOVE_RIGHT)
	{
		if(m_acceleration < 6) m_acceleration += 2;
		else m_acceleration = 6;
		m_lookRight = true;

		if(m_animationstate == PLAYER_ROPE || (m_animationstate == PLAYER_LADDER && !c_gamearea::level()->ptIn_environment(m_position.x + m_acceleration, m_position.y, ENV_LADDER)))
		{
			m_climbing = false;
			setAnimation(PLAYER_MOVE, 0.0f);
		}
	}
	if(direction == MOVE_UP)
	{
		// Climb ladder or rope
		s_environment env;
		if(c_gamearea::level()->ptIn_environment(m_position.x, m_position.y, ENV_LADDER, &env) || c_gamearea::level()->ptIn_environment(m_position.x, m_position.y, ENV_ROPE, &env))
		{
			//if(!c_gamearea::level()->bbox_inTile(m_position.x, m_position.y - 8, getBBox()))
			if(!c_gamearea::level()->bbox_inSolid(m_position.x, m_position.y - 8, getBBox(), this))
			{
				playeranimation climbanim = PLAYER_ROPE;
				if(env.type == ENV_LADDER) climbanim = PLAYER_LADDER;

				if(m_animationstate != climbanim)
				{
					m_animationstate = climbanim;
					m_curFrame = (g_playerType->getAnimation(m_animationstate)).begframe;
				}

				m_position.y -= 8;
				m_frameSpeed = 0.3f;
				m_freespace = true;
				m_climbing = true;
			}

			if(!m_freespace)
			{
				m_frameSpeed = 0.0f;
				m_curFrame = (g_playerType->getAnimation(m_animationstate)).begframe;
			}
		}
		//else if(c_gamearea::level()->bbox_inTile(m_position.x, m_position.y + 1, getBBox()))
		else if(c_gamearea::level()->bbox_inSolid(m_position.x, m_position.y + 1, getBBox(), this))
		{
			// Jump
			setAnimation(PLAYER_JUMP, 0.0f);
			m_velocity.y = -12;
		}
	}
	else if(direction == MOVE_DOWN)
	{
		// Climb down a ladder or rope
		s_environment env;
		if(c_gamearea::level()->ptIn_environment(m_position.x, m_position.y, ENV_LADDER, &env) || c_gamearea::level()->ptIn_environment(m_position.x, m_position.y, ENV_ROPE, &env))
		{
			//if(!c_gamearea::level()->bbox_inTile(m_position.x, m_position.y + 8, getBBox()))
			if(!c_gamearea::level()->bbox_inSolid(m_position.x, m_position.y + 8, getBBox()))
			{
				playeranimation climbanim = PLAYER_ROPE;
				if(env.type == ENV_LADDER) climbanim = PLAYER_LADDER;

				if(m_animationstate != climbanim)
				{
					m_animationstate = climbanim;
					m_curFrame = (g_playerType->getAnimation(m_animationstate)).begframe;
				}

				m_position.y += 8;
				m_frameSpeed = 0.3f;
				m_freespace = true;
				m_climbing = true;
			}

			if(!m_freespace)
			{
				m_frameSpeed = 0.0f;
				m_curFrame = (g_playerType->getAnimation(m_animationstate)).begframe;
			}
		}
	}
}
void c_player::onTileCollision(uint x, uint y, s_tile *pTile)
{
	// Move the entity
	s_bbox bbox = getBBox();

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
void c_player::onCollision(c_entity *pOther)
{
	s_bbox boxA, boxB;
	s_point pos;

	boxA = getBBox();

	if(pOther->getEntityType() == ENTITY_TYPE_PLATFORM)
	{
		c_platform *platform = (c_platform*)pOther;
		boxB = pOther->getBBox();
		pos = pOther->getPosition();

		// Stop the player from falling through the platform
		if(platform_get(platform->getType())->getType() == PLATFORM_LIFT)
		{
			if(m_velocity.y > 0)
			{
				if(/*m_position.y > m_oldPosition.y && */m_position.y + boxA.bottom >= pos.y - boxB.top && m_oldPosition.y + boxA.bottom <= pos.y - boxB.top)
				{
					m_position.y = pos.y - boxB.top - boxA.bottom;
					m_velocity.y = 0;
				}
			}
		}
		else
		{
			// Stop the player from going through
			//if(platform_get(platform->getType())->getType() != PLATFORM_LIFT)
			//{
				int yVel = 0;
				if(m_oldPosition.y < m_position.y) yVel = -1;
				else if(m_oldPosition.y > m_position.y) yVel = 1;

				if(yVel != 0)
				{
					for(int i = 0; i < GRID_H; i++)
					{
						if(pOther->collide_bbox(m_position, boxA))
							m_position.y += yVel;
						else
							break;
					}
				}
				m_velocity.y = 0;
			//}
		}
		if(platform_get(platform->getType())->getType() == PLATFORM_PUSHABLE)
		{
			// Push the pushable entity around if possible
			m_acceleration /= 2;
			s_point boxspeed = {m_acceleration, pOther->getVelocity().y};
			pOther->setVelocity(boxspeed);
		}
	}
}

void c_player::ev_cycle()
{
	if(m_immortal < 0) m_immortal++;
	else if(m_immortal > 0) m_immortal--;

	if(m_acceleration != 0)
	{
		// Move the player to its destination
		for(int y = 0; y <= GRID_H / 2; y++)
		{
			//if(!c_gamearea::level()->bbox_inTile(m_position.x + m_hspeed, m_position.y - y, getBBox()))
			if(!c_gamearea::level()->bbox_inSolid(m_position.x + m_acceleration, m_position.y - y, getBBox(), this))
			{
				m_position.x += m_acceleration;
				m_position.y -= y;

				if(m_animationstate == PLAYER_STAND)
				{
					setAnimation(PLAYER_MOVE, 0.2f);
				}
				m_freespace = true;
				break;
			}
		}
		// Make friction
		float friction = 1.0f;
		s_environment env;

		// Check for slippery surfaces
		if(c_gamearea::level()->bbox_inTile(m_position.x, m_position.y + 1, getBBox()))
		{
			if(c_gamearea::level()->bboxIn_environment(m_position.x, m_position.y + 1, getBBox(), ENV_ICE, &env))
			{
				friction = 0.25f;
			}
		}

		if(m_acceleration >= -0.5f && m_acceleration <= 0.5f)
		{
			m_acceleration = 0.0f;
		}

		if(m_acceleration > 0) m_acceleration -= friction;
		else if(m_acceleration < 0) m_acceleration += friction;
	}

	/*if(key_released(SDL_SCANCODE_LEFT) || key_released(SDL_SCANCODE_RIGHT))
	{
		//m_hspeed = 0;
		m_frameSpeed = 0;
		m_curFrame = (g_playerType->getAnimation(m_animationstate)).begframe;
	}
	else */if(key_released(SDL_SCANCODE_DOWN) || key_released(SDL_SCANCODE_UP))
	{
		m_frameSpeed = 0;
	}

	// Add gravity
	//if(!c_gamearea::level()->bbox_inTile(m_position.x, m_position.y + 1, getBBox()))
	if(!c_gamearea::level()->bbox_inSolid(m_position.x, m_position.y + 1, getBBox(), this))
	{
		if(m_velocity.y < 15) m_velocity.y++;
	}
	else
	{
		//m_velocity.y = 0;
	}

	// Check for a ladder or a rope
	if(	c_gamearea::level()->ptIn_environment(m_position.x, m_position.y, ENV_LADDER) ||
		c_gamearea::level()->ptIn_environment(m_position.x, m_position.y, ENV_ROPE))
	{
		//if(m_climbing)
		if(m_animationstate == PLAYER_LADDER || m_animationstate == PLAYER_ROPE)
			m_velocity.y = 0;
	}

	/// See if the player shall take damage from the environment
	s_environment envDamage;
	s_bbox plDmgBox = getBBox();
	plDmgBox.left--; plDmgBox.top--;
	plDmgBox.right++; plDmgBox.bottom++;
	if( c_gamearea::level()->bboxIn_environment(m_position.x, m_position.y, plDmgBox, ENV_DAMAGE, &envDamage) )
	{
		if(envDamage.enabled)
		{
			if(envDamage.flag & 1)
			{
				// Dies no matter how
				if(c_engine::getEngine()->loseHealth(10))
				{
					m_position = c_engine::getEngine()->getCheckPointPosition();
				}
			}
			else
			{
				// Take normal damage
				if(m_immortal == 0)
				{
					m_immortal = -60;
					if(c_engine::getEngine()->loseHealth((short)envDamage.value))
					{
						m_position = c_engine::getEngine()->getCheckPointPosition();
					}
					m_velocity.y = -6;
					m_acceleration = 6 - 12 * m_lookRight;
				}
			}
		}
	}
}
void c_player::ev_cycleEnd()
{
	// Control animation
	if(!m_freespace && (m_animationstate == PLAYER_STAND || m_animationstate == PLAYER_MOVE))
	{
		// Stop the animation, if there are no free space
		m_frameSpeed = 0.0f;
		//if(c_gamearea::level()->bbox_inTile(m_position.x, m_position.y + 1, getBBox()))
		if(c_gamearea::level()->bbox_inSolid(m_position.x, m_position.y + 1, getBBox(), this))
		{
			m_animationstate = PLAYER_STAND;
		}
		else if(m_velocity.y != 0.0f)
		{
			setAnimation(PLAYER_JUMP, 0.0f);
		}
	}
	m_freespace = false;

	// Make the player blink after taking damage
	if(m_immortal < 0)
	{
		m_bVisible = !m_bVisible;
	}
	else
		m_bVisible = true;

	// Not moving
	if(m_acceleration == 0)
	{
		if(m_animationstate == PLAYER_MOVE)
			setAnimation(PLAYER_STAND, 0.0f);
	}

	// Animate for jumping/falling
	if(m_velocity.y != 0)
	{
		setAnimation(PLAYER_JUMP, 0.0f);
	}
	else
	{
		//if(m_animationstate == PLAYER_JUMP && c_gamearea::level()->bbox_inTile(m_position.x, m_position.y + 1, getBBox()))
		if(m_animationstate == PLAYER_JUMP && c_gamearea::level()->bbox_inSolid(m_position.x, m_position.y + 1, getBBox(), this))
		{
			setAnimation(PLAYER_STAND, 0.0f);
		}
	}
}
void c_player::ev_render(int viewX, int viewY)
{
	s_bbox bbox = getBBox();
	draw_rect(m_position.x - bbox.left - viewX, m_position.y - bbox.top - viewY, bbox.left + bbox.right, bbox.top + bbox.bottom, 0, 0xFF, 0x0, 128);

	if(m_bVisible)
	{
		c_image *sprite = g_playerType->getImage(!m_lookRight); // 39/63
		sprite->draw(m_position.x - 19 - viewX, m_position.y - 32 - viewY, (uint)floor(m_curFrame));
	}
}

/// General methods
bool c_player::load(ifstream &ff)
{
	ushort x, y;
	char direction;
	uint type;

	ff.read(reinterpret_cast<char*>(&x), sizeof(x));
	ff.read(reinterpret_cast<char*>(&y), sizeof(y));

	ff.read(reinterpret_cast<char*>(&type), sizeof(type));
	ff.read(reinterpret_cast<char*>(&direction), sizeof(direction));

	m_type = (uint)type;

	s_bbox bbox = player_get()->getBBox();

	m_position.x = (float)x * 32 + bbox.left;
	m_position.y = (float)y * 32 + bbox.top;
	m_oldPosition = m_position;

	m_lookRight = direction == 1; // 1 = RIGHT

	return true;
}
void c_player::animate()
{
	s_animation animation = g_playerType->getAnimation(m_animationstate);
	float animLength = (float)animation.endframe - (float)animation.begframe;
	float frames = (float)numFrames();
	if(frames < 1.0f || animLength < 1.0f)
	{
		return;
	}

	if(m_frameSpeed != 0.0f)
	{
		m_curFrame += m_frameSpeed;
		if(m_curFrame < (float)animation.begframe) m_curFrame += animLength;
		else if(m_curFrame >= (float)animation.endframe + 1) m_curFrame -= animLength + 1;
	}
}
void c_player::setAnimation(playeranimation state, float frameSpeed)
{
	s_animation animation = g_playerType->getAnimation(state);
	m_animationstate = state;
	m_curFrame = animation.begframe;
	m_frameSpeed = frameSpeed;
}

/// Accessor methods
int c_player::getDepth()const { return 100; }
s_bbox c_player::getBBox()const
{
	return g_playerType->getBBox();
}
uint c_player::numFrames()const
{
	return g_playerType->getImage(!m_lookRight)->numFrames();
}

/************************************************/
/**          Declare global functions          **/
/************************************************/
void player_add(c_playertype *player)
{
	if(!g_playerType && player) g_playerType = player;
}
void player_free()
{
	if(g_playerType) delete g_playerType;
	g_playerType = NULL;
}
c_playertype *player_get()
{
	return g_playerType;
}
