#ifndef _CHECKPOINT_H
#define _CHECKPOINT_H

#include "entity.h"

#define ENTITY_TYPE_CHECKPOINT 6

class c_image;

class c_checkpoint: public c_entity
{
	private:
		bool	m_activated;
	public:
		c_checkpoint();
		virtual ~c_checkpoint();

		// Event methods
		void	onMovement(int direction);
		void	onTileCollision(uint x, uint y, s_tile *pTile);
		void	onCollision(c_entity *pOther);

		void	ev_cycle();
		void	ev_cycleEnd();
		void	ev_render(int viewX, int viewY);

		// General methods
		bool	load(ifstream &ff);

		void	animate();

		// Accessor methods
		uchar	getEntityType()const;
		s_bbox	getBBox()const;
		uint	numFrames()const;
};

#endif
