#ifndef _PLAYER_H
#define _PLAYER_H
/// Included files and defined macros
#include "entity.h"
#include "multimedia.h"

#define ENTITY_TYPE_PLAYER 0

enum playeranimation
{
	PLAYER_STAND = 0,
	PLAYER_MOVE,
	PLAYER_LADDER,
	PLAYER_ROPE,
	PLAYER_JUMP,
	PLAYER_AIM_SNOWBALL,
	PLAYER_THROW_SNOWBALL,
	PLAYER_THROW_OBJECT,
	PLAYER_REMOTE_CONTROL
};

/// Declare classes
class c_playertype
{
	private:
		uint	m_numAnimation;

		s_animation	*m_animation;
		c_image	*m_image[2];

		s_bbox	m_bbox;
	public:
		c_playertype();
		~c_playertype();

		// General methods
		bool	createImage(const char *fname, uint frames, const char *fnameLeft = NULL);
		void	setBBox(int left, int right, int top, int bottom);
		void	initAnimations(uint animations);
		void	setAnimation(uint animation, uint begframe, uint endframe);

		// Accessor methods
		c_image	*getImage(bool right)const { return m_image[right]; };
		s_bbox	getBBox()const   { return m_bbox; };
		uint	numAnimations()const { return m_numAnimation; };
		s_animation	getAnimation(uint animation)const { return m_animation[animation]; };
};

class c_player: public c_entity
{
	private:
		float	m_hspeed;
		bool	m_lookRight;
		uint	m_animationstate;

		// Player states
		bool	m_climbing;
		bool	m_freespace;

		// Stats
		bool	m_bVisible;
		int		m_immortal;
	public:
		c_player();
		c_player(int x, int y);
		virtual ~c_player();

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
		void	setAnimation(playeranimation state, float frameSpeed);

		// Accessor methods
		uchar	getEntityType()const { return ENTITY_TYPE_PLAYER; };
		int		getDepth()const;
		s_bbox	getBBox()const;
		uint	numFrames()const;
};

/// Declare global functions
void	player_add(c_playertype *playertype);
void	player_free();
c_playertype	*player_get();

#endif // _PLAYER_H
