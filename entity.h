/** Header for the entity base class **/
#ifndef _ENTITY_H
#define _ENTITY_H
/** Included files **/
#include "base.h"
#include "bitmask.h" // Only for the s_bbox struct

#include <fstream>

using namespace std;

enum eInput
{
	MOVE_UP = 0,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT
};

/** Declare classes **/
struct s_tile;

class c_entity
{
	protected:
		uint	m_type;
		bool	m_enabled;
		bool	m_toBeDestroyed;

		s_point	m_position;
		s_point	m_oldPosition;
		s_point	m_velocity;

		float	m_curFrame;
		float	m_frameSpeed;
	public:
		c_entity();
		virtual ~c_entity();

		// Event methods
		virtual void	onMovement(int direction) = 0;
		virtual void	onTileCollision(uint x, uint y, s_tile *pTile) = 0;
		virtual void	onCollision(c_entity *pOther) = 0;

		virtual void	ev_cycle() = 0;
		virtual void	ev_cycleEnd() = 0;
		virtual void	ev_render(int viewX, int viewY) = 0;

		// General methods
		void	destroy();
		void	enable();
		void	disable();

		virtual bool	load(ifstream &ff) = 0;

		bool	collide_point(const s_point &p);
		bool	collide_point(int x, int y);
		bool	collide_bbox(const s_point &p, const s_bbox &bbox);
		bool	collide_bbox(int x, int y, const s_bbox &bboxA);

		virtual void	animate();

		// Accessor methods
		virtual uchar	getEntityType()const = 0;
		uint	getType()const;
		bool	isDestroyed()const;
		bool	isEnabled()const;

		s_point	getPosition()const;
		void	setPosition(s_point p);
		s_point	getOldPosition()const;
		void	setOldPosition(s_point p);
		s_point	getVelocity()const;
		void	setVelocity(s_point v);

		virtual int		getDepth()const;
		virtual s_bbox	getBBox()const = 0;
		virtual	uint	numFrames()const = 0;
};

#endif // _ENTITY_H
