/** Header file for the tile data structure and such **/
#ifndef _TILES_H
#define _TILES_H
/** Included files **/
#include "base.h"

/** Define macros **/
#define TILE_LAYERS 6
#define TILE_LAYER_SOLID 2
#define TILE_LAYER_FORGROUND TILE_LAYERS - 2

// Tile flags		Value	Effect
#define TILE_NONE		0	//	No effect (default)
//#define TILE_SOLID		1	//	If checked, the tile will be used in collision detection and response
//#define TILE_FOREGROUND	2	//	Layer 2 and 3 is in front of entities and particles (Layer 1 is always behind)
#define TILE_PRECISION	4	//	Requires that the tile is solid. Use the bitmask to check for a more detailed collision
#define TILE_PRECLAYER3	8	//  Requires that the tile use precision. If set, the tile will use layer 3 instead of layer 2
#define TILE_ANIMATE_1	16	//	Animate the first layer of this tile
#define TILE_ANIMATE_2	32	//	Animate the second layer of this tile
#define TILE_ANIMATE_3	64	//	Animate the third layer of this tile
#define TILE_NOT_USED	128	//	Not in use

/** Declare structures and classes **/
struct s_tile
{
	uchar	x[TILE_LAYERS], y[TILE_LAYERS];

	/// Functions
	s_tile();
};
class c_tile_animNode
{
	private:
		uchar	m_begin, m_end; // Start and and columns of the animation
		uchar	m_row; // The row which is animated
		ushort	m_delay; // Delay before frame-shifting at the tiles
		ushort	m_current; // The current delay level

		uint	m_numTiles; // Number of tiles which are animated by this node
		s_tile	**m_tiles; // The array of tiles animated by this tile
		uchar	*m_layers;
	public:
		c_tile_animNode();
		~c_tile_animNode();

		// General and Accessor Methods
		void	init(uchar row, uchar begin, uchar end, ushort delay);
		bool	animate();
		void	tile_add(s_tile *tile, uchar layer);

		uchar	getRow()const   {return m_row;};
		uchar	getStart()const {return m_begin;};
		uchar	getEnd()const   {return m_end;};
};

/** Declare global functions **/

/** Endif **/
#endif
