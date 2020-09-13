/** Header file for the bitmask class and data **/
#ifndef _COLBITMASK_H
#define _COLBITMASK_H
/** Included files **/
#include "base.h"

/** Define macros **/
#define GRID_W 32
#define GRID_H 32

/** Declare classes and structs **/
class s_tile;
struct s_bbox
{
	short left, right;
	short top, bottom;
};

class c_bitmask
{
	private:
		uint	m_width, m_height;
		char	*m_bitmask;
	public:
		// Constructor(s)/destructor
				c_bitmask();
		virtual	~c_bitmask();

		// General functions
		void	free();
		bool	load(const char *fname);
		bool	bit(uint x, uint y)const;
		bool	point_inside(uint tx, uint ty, s_tile *tile, int x, int y);
		bool	bbox_inside(uint tx, uint ty, s_tile *tile, int x, int y, const s_bbox &bbox);

		// Accessor functions
		uint	width()const	{return m_width;};
		uint	height()const	{return m_height;};
		char	*mask()const	{return m_bitmask;};
};

/** Endif **/
#endif
