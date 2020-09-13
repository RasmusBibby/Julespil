#include "entity.h"

/****************************************/
/**  Implement c_entity class methods  **/
/****************************************/
c_entity::c_entity()
{
	m_type = 0;
	m_toBeDestroyed = false;
	m_enabled = true;

	m_position.x = m_position.y = 0;
	m_velocity = m_oldPosition = m_position;
}
c_entity::~c_entity() {}

// Public General Methods
void c_entity::destroy() { m_toBeDestroyed = true; }
void c_entity::enable() { m_enabled = true; }
void c_entity::disable() { m_enabled = false; }

bool c_entity::collide_point(const s_point &p) { return collide_point(p.x, p.y); }
bool c_entity::collide_point(int x, int y)
{
	// Check if point (x,y) is inside the entity
	s_bbox bbox = getBBox();
	if(x < m_position.x - bbox.left) return false;
	if(x >= m_position.x + bbox.right) return false;
	if(y < m_position.y - bbox.top) return false;
	if(y >= m_position.y + bbox.bottom) return false;
	return true;
}
bool c_entity::collide_bbox(const s_point &p, const s_bbox &bbox) { return collide_bbox(p.x, p.y, bbox); }
bool c_entity::collide_bbox(int x, int y, const s_bbox &bboxA)
{
	s_bbox bboxB = getBBox();
	if(x + bboxA.right  < m_position.x - bboxB.left) return false;
	if(x - bboxA.left   >= m_position.x + bboxB.right) return false;
	if(y + bboxA.bottom < m_position.y - bboxB.top) return false;
	if(y - bboxA.top    >= m_position.y + bboxB.bottom) return false;
	return true;
}

void c_entity::animate()
{
	float frames = (float)numFrames();
	if(frames <= 1.0f) return;

	if(m_frameSpeed != 0.0f)
	{
		m_curFrame += m_frameSpeed;
		if(m_curFrame < 0.0f) m_curFrame += frames;
		else if(m_curFrame >= frames) m_curFrame -= frames;
	}
}

// Public Accessor Methods
uint c_entity::getType()const { return m_type; }
bool c_entity::isDestroyed()const { return m_toBeDestroyed; }
bool c_entity::isEnabled()const { return m_enabled; }

s_point c_entity::getPosition()const	{ return m_position; }
void c_entity::setPosition(s_point p)	{ m_position = p; }
s_point c_entity::getOldPosition()const	{ return m_oldPosition; }
void c_entity::setOldPosition(s_point p){ m_oldPosition = p; }
s_point c_entity::getVelocity()const	{ return m_velocity; }
void c_entity::setVelocity(s_point p)	{ m_velocity = p; }

int c_entity::getDepth()const { return 0; }
