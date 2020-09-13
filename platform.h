#ifndef _PLATFORM_H
#define _PLATFORM_H
/// Included files and defined macros
#include "entity.h"
#include "multimedia.h"

#define ENTITY_TYPE_PLATFORM 5

#define PLATFORM_LIFT 0
#define PLATFORM_DOOR 1
#define PLATFORM_PUSHABLE 2

class c_platformtype
{
	private:
		c_image	*m_sprite;
		s_bbox	m_bbox;
		char	m_type;
	public:
		c_platformtype(const char *fname, short width, short height, char type);
		~c_platformtype();

		// Accessor Methods
		c_image	*getImage()const  { return m_sprite; };
		s_bbox	getBBox()const   { return m_bbox; };
		char	getType()const   { return m_type; };
};

class c_platform: public c_entity
{
	private:
	public:
		c_platform();
		~c_platform();

		// Event methods
		void	onMovement(int direction);
		void	onTileCollision(uint x, uint y, s_tile *pTile);
		void	onCollision(c_entity *pOther);

		void	ev_cycle();
		void	ev_cycleEnd();
		void	ev_render(int viewX, int viewY);

		// General methods
		bool	load(ifstream &ff);

		// Accessor methods
		uchar	getEntityType()const { return ENTITY_TYPE_PLATFORM; };
		int		getDepth()const;
		s_bbox	getBBox()const;
		uint	numFrames()const;
};

void	platform_add(c_platformtype *p);
void	platform_free();
c_platformtype	*platform_get(uint x);

#endif
