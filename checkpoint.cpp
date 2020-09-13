#include "checkpoint.h"
#include "engine.h"
#include "multimedia.h"
#include "player.h"

c_checkpoint::c_checkpoint()
{
	m_type = 0;

	m_activated = false;
	m_curFrame = 0.0f;
	m_frameSpeed = 0.0f;
}
c_checkpoint::~c_checkpoint()
{
}

// Event methods
void c_checkpoint::onMovement(int direction) {}
void c_checkpoint::onTileCollision(uint x, uint y, s_tile *pTile) {}
void c_checkpoint::onCollision(c_entity *pOther)
{
	if(pOther->getEntityType() == ENTITY_TYPE_PLAYER)
	{
		if(!m_activated)
		{
			m_activated = true;
			m_frameSpeed = 0.4f;

			//c_engine::getEngine()->setCheckPoint(
			//	(m_pPlayer->getPosition().x - m_pPlayer->getBBox().left ) / 32,
			//	(m_pPlayer->getPosition().y - m_pPlayer->getBBox().right) / 32);
			c_engine::getEngine()->setCheckPoint(m_position.x + 16, m_position.y);
		}
	}
}

void c_checkpoint::ev_cycle() {}
void c_checkpoint::ev_cycleEnd() {}
void c_checkpoint::ev_render(int viewX, int viewY)
{
	if(viewX >= m_position.x + 47) return;
	if(viewY >= m_position.y + 47) return;
	if(viewX + 640 < m_position.x - 15) return;
	if(viewY + 480 < m_position.y - 15) return;

	c_engine::getEngine()->getCheckpointSprite()->draw(m_position.x - 15 - viewX, m_position.y - 9 - viewY, (uint)floor(m_curFrame));
}

// General methods
bool c_checkpoint::load(ifstream &ff)
{
	ushort x, y;

	ff.read(reinterpret_cast<char*>(&x), sizeof(x));
	ff.read(reinterpret_cast<char*>(&y), sizeof(y));

	m_position.x = (float)x * 32;
	m_position.y = (float)y * 32;

	return true;
}
void c_checkpoint::animate()
{
	if(m_activated)
	{
		m_curFrame += m_frameSpeed;
		if(m_curFrame >= (float)numFrames())
			m_curFrame -= (float)numFrames() - 1.0f;
	}
}

// Accessor methods
uchar c_checkpoint::getEntityType()const { return ENTITY_TYPE_CHECKPOINT; }
s_bbox c_checkpoint::getBBox()const
{
	s_bbox bbox = {0, 0, 32, 32};
	return bbox;
}
uint c_checkpoint::numFrames()const { return 5; }
