/** Source file for the tile instructions **/
#include "tile.h"

#include <vector>
#include <iostream>

using namespace std;

/** Implement c_tile class functions **/
s_tile::s_tile()
{
	for(int i = 0; i < TILE_LAYERS; i++) { x[i] = y[i] = 0; }
}

/***********************************************************/
/**       Implement c_tile_animNode class functions       **/
/***********************************************************/
c_tile_animNode::c_tile_animNode()
{
	m_begin = 0;
	m_end = 0;
	m_row = 0;
	m_delay = 0;
	m_current = 0;

	m_numTiles = 0;
	m_tiles = NULL;
	m_layers = NULL;
}
c_tile_animNode::~c_tile_animNode()
{
	if(m_tiles)
	{
		delete[] m_tiles;
		delete[] m_layers;
		m_tiles = NULL;
		m_layers = NULL;
	}
}

/// General and Accessor Methods
void c_tile_animNode::init(uchar row, uchar begin, uchar end, ushort delay)
{
	// Set the row position and the start and end position (X-coordinate)
	m_row = row;
	m_begin = begin;
	m_end = end;

	// Set the animation speed and delay
	m_delay = delay;

	m_numTiles = 0;
	m_tiles = NULL;
}
bool c_tile_animNode::animate()
{
	// Returns true if an animation should occur
	m_current++;
	if(m_current >= m_delay)
	{
		m_current -= m_delay;
		for(uint i = 0; i < m_numTiles; i++)
		{
			// Animate all the tiles on this list
			if(m_tiles[i]->x[m_layers[i]] + 1 >= m_end)
				m_tiles[i]->x[m_layers[i]] = m_begin;
			else
				m_tiles[i]->x[m_layers[i]]++;
		}
		return true;
	}
	return false;
}
void c_tile_animNode::tile_add(s_tile *tile, uchar layer)
{
	if(!m_tiles)
	{
		m_numTiles = 1;
		m_tiles = new s_tile*[1];
		m_layers = new uchar[1];
		m_tiles[0] = tile;
		m_layers[0] = layer;
	}
	else
	{
		s_tile **tmp = new s_tile*[m_numTiles + 1];
		uchar *layers = new uchar[m_numTiles + 1];

		for(uint i = 0; i < m_numTiles; i++)
		{
			tmp[i] = m_tiles[i];
			layers[i] = m_layers[i];
		}
		tmp[m_numTiles] = tile;
		layers[m_numTiles] = layer;

		delete[] m_tiles;
		delete[] m_layers;

		m_numTiles++;
		m_tiles = tmp;
		m_layers = layers;
	}
}
