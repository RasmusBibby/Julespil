/** Header file for the playing area of the game **/
#ifndef _GAMEAREA
#define _GAMEAREA
/** Included files **/
#include "level.h"

#include <vector>

#include <string> // Used for the s_theme struct

using namespace std;

/** Define macros **/

/** Declare structs and classes **/
class	c_bitmask;
class	s_tile;
class	c_tile_animNode;
class	c_background;
class	c_entity;
class	c_player;
class	c_image;

struct	s_bbox;

struct s_environment
{
	uint	x, y;
	uint	w, h;
	ushort	type;
	uchar	value;
	uchar	flag;
	bool	enabled;
};

enum e_envtypes
{
	ENV_TRIGGER = 0,
	ENV_LADDER,
	ENV_ROPE,
	ENV_ICE,
	ENV_DAMAGE,
	ENV_CONVEYER
};

class c_gamearea: public c_level
{
	private:
		static c_gamearea	*m_pCurrentLevel;

		// Headers
		uint	m_width, m_height;

		bool		m_newLevel;
		c_player	*m_pPlayer;

		// Background data
		uint	m_numBackgrounds;
		uint	m_startForeground; // Tell where the backgrounds ends and become foregrounds
		c_background	*m_background;

		// Tiles data
		SDL_Texture	*m_pTileset;
		c_bitmask	*m_pBitmask;
		s_tile		**m_tiles;

		uint			m_numTileAnimations;
		c_tile_animNode	*m_tileAnimation;

		// Main containers
		vector<c_entity*>		m_entities;
		vector<s_environment>	m_environments;

		// Viewport
		uint		m_viewX, m_viewY;
		c_entity	*m_follow;

		// General Members
		void	tile_addToAnimator(ushort x, ushort y, uchar layer);
		void	addEnvironment(ifstream &ff);

		// Experimental
	public:
				c_gamearea();
		virtual ~c_gamearea();

		static c_gamearea	*level();

		/// Event members
		void	ev_input(SDL_Event &ev);
		void	ev_update();
		void	ev_render();

		void	ev_begin();
		void	ev_end();

		/// General members
		void	free();
		bool	load(const char *fname);
		bool	setTheme(int theme, uchar color[]);

		void	entity_add(c_entity *entity);
		void	entity_follow(c_entity *entity) { m_follow = entity; };

		bool	ptIn_environment(int x, int y, ushort type, s_environment *environment = NULL);
		bool	bboxIn_environment(int x, int y, const s_bbox &bbox, ushort type, s_environment *environment = NULL);

		// Collision code against tiles
		bool	ptin_tile(int x, int y);
		bool	ptin_tile(uint xx, uint yy, int x, int y);
		bool	bbox_inTile(int x, int y, const s_bbox &bbox);
		bool	bbox_inTile(uint xx, uint yy, int x, int y, const s_bbox &bbox);
		void	move_bboxTile(int x, int y, const s_bbox &bbox, int direction, s_point &q);
		bool	bbox_inSolid(int x, int y, const s_bbox &bbox, c_entity *ent = nullptr);

		/// Accessors
		uint	width()const { return m_width; };
		uint	height()const { return m_height; };

		void	setCheckPoint(ushort x, ushort y);

		/// Experimental functions
		bool	set_tileset(const char *fname, uchar red, uchar green, uchar blue);
		bool	set_bitmask(const char *fname);
		void	set_tile(uint x, uint y, int layer, uchar image_x, uchar image_y);
		void	set_tileflag(uint, uint, uchar);
		void	create_tileAnimators(uint numNodes);

		void	create_backgrounds(uint n);

		bool	test_level();
};

/** Declare global functions **/

/** Endif **/
#endif
