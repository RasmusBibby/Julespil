/** Source file for the bitmask class **/
#include "bitmask.h"
#include "tile.h"

#include <fstream>

/** Implement the functions of the c_bitmask class **/
c_bitmask::c_bitmask():
m_width(0), m_height(0)
{
	m_bitmask = NULL;
}
c_bitmask::~c_bitmask()
{
	free();
}

// General functions
void c_bitmask::free()
{
	/// Clear the bitmask
	if(m_bitmask) delete[] m_bitmask;
	m_bitmask = NULL;
	m_width = m_height = 0;
}
bool c_bitmask::load(const char *fname)
{
	/// Load a bitmask from a file
	std::ifstream	ff;
	char		l_ver_nr;

	// Try to open the file
	ff.open(fname, std::ios::binary);
	if(ff.is_open() == false)
		return false;

	// Check the file's version number
	DTR(ff, l_ver_nr);
	if(l_ver_nr != 0)
	{
		ff.close();
		return false;
	}

	free();

	// Get the mask's size
	DTR(ff, m_width);
	DTR(ff, m_height);

	// Get the mask's data
	m_bitmask = new char[m_width * m_height * 8];
	ff.read(m_bitmask, m_width * m_height * 8);

	// Close the file, and return true
	ff.close();
	return true;
}

bool c_bitmask::bit(uint x, uint y)const
{
	/// Get a bit from the bitmask
	// Tell which row to get the bit from
	uint	l_byte = m_width * y;

	// Find the column from which the bit is in
	while(x >= 8)
	{
		l_byte++;
		x -= 8;
	}
	// Return the wanted bit from the bitmask
	return ((m_bitmask[l_byte] >> x) & 1);
}
bool c_bitmask::point_inside(uint tx, uint ty, s_tile *tile, int x, int y)
{
	// Get tile information
	uchar layer = TILE_LAYER_SOLID;


	// Calculate the size of the tile
	uint	tile_max_x = tile->x[layer] * 8 + 8;
	uint	tile_max_y = tile->y[layer] * 8 + 8;

	// Test if the tile is larger or smaller than the bitmask
	uint	l_width  = (tile_max_x < m_width) ? tile_max_x : m_width;
	uint	l_height = (tile_max_y < m_height) ? tile_max_y : m_height;

	// Calculate the offset of the mask
	int	mask_x = tx * GRID_W - tile->x[layer] * GRID_W;
	int	mask_y = ty * GRID_H - tile->y[layer] * GRID_W;

	// Detect whether the point is inside the tile or not
	for(uint yy = tile->y[layer] * 8; yy < l_height; yy++)
	{
		for(uint xx = tile->x[layer] * 8; xx < l_width; xx++)
		{
			if(bit(xx, yy) == true)
			{
				if((uint)x < mask_x + 4 * xx) continue;
				if((uint)y < mask_y + 4 * yy) continue;
				if((uint)x >= mask_x + 4 * xx + 4) continue;
				if((uint)y >= mask_y + 4 * yy + 4) continue;
				return true;
			}
		}
	}
	// Return false, when no detection was found
	return false;
}
bool c_bitmask::bbox_inside(uint tx, uint ty, s_tile *tile, int x, int y, const s_bbox &bbox)
{
	/// Check whether the bounding box (bbox) is inside the part of the bitmask pointed by "tile" at position (x,y)

	// Get tile information
	uchar layer = TILE_LAYER_SOLID;

	// Calculate the size of the tile
	uint	tile_max_w = tile->x[layer] * 8 + 8;
	uint	tile_max_h = tile->y[layer] * 8 + 8;

	// Test if the tile is larger or smaller than the bitmask
	uint	l_width  = (tile_max_w < m_width * 8)  ? tile_max_w : m_width * 8;
	uint	l_height = (tile_max_h < m_height * 8) ? tile_max_h : m_height * 8;

	// Calculate the offset of the mask
	int	mask_x = tx * GRID_W - tile->x[layer] * GRID_W;
	int	mask_y = ty * GRID_H - tile->y[layer] * GRID_W;

	// Detect whether the point is inside the tile or not
	for(uint yy = tile->y[layer] * 8; yy < l_height; yy += 1)
	{
		for(uint xx = tile->x[layer] * 8; xx < l_width; xx += 1)
		{
			if(bit(xx, yy) == true)
			{
				if(x + bbox.right < mask_x + 4 * (int)xx) continue;
				if(y + bbox.bottom < mask_y + 4 * (int)yy) continue;
				if(x - bbox.left >= mask_x + 4 * (int)xx + 4) continue;
				if(y - bbox.top >= mask_y + 4 * (int)yy + 4) continue;
				return true;
			}
		}
	}
	return false;
}
