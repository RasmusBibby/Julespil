/** Source file for the game level class **/
#include "gamearea.h"

#include "engine.h"

#include "multimedia.h"
#include "bitmask.h"
#include "background.h"
#include "tile.h"
#include "entity.h"

// Entity type classes
#include "player.h"
#include "checkpoint.h"
#include "platform.h"

#include <fstream>
#include <cmath>

#include <iostream>
using namespace std;

/** Declare global and extern variables **/
const char	g_editorID[4] = {'X', 'M', 'A', 'S'};
const char	g_cThemeID[4] = {'T', 'I', 'L', 'E'};
uint	g_versionnr = 0;

void	readBuffer(ifstream &ff, string &str, uint size);

/** Implement the c_gamearea class functions **/
c_gamearea *c_gamearea::m_pCurrentLevel = NULL;

c_gamearea::c_gamearea()
{
	m_width = 0;
	m_height = 0;

	m_newLevel = false;
	m_pPlayer = NULL;

	// Set background stats
	m_numBackgrounds = 0;
	m_startForeground = 0;
	m_background = NULL;

	// Set tileset stats
	m_pTileset = NULL;
	m_pBitmask = NULL;
	m_tiles = NULL;

	m_numTileAnimations = 0;
	m_tileAnimation = NULL;

	// Set viewport data
	m_viewX = m_viewY = 0;
	m_follow = NULL;

	// Set the experimental data
}
c_gamearea::~c_gamearea()
{
	free();
}

c_gamearea *c_gamearea::level()
{
	return m_pCurrentLevel;
}

/// Event functions
void c_gamearea::ev_input(SDL_Event &ev)
{
	/*if(ev.type == SDL_MOUSEMOTION)
	{
		g_pos[0] = ev.motion.x;
		g_pos[1] = ev.motion.y;

		if(bbox_inTile(g_pos[0] + m_viewX, g_pos[1] + m_viewY, g_bbox) == true)
			g_testcolor = 0x00FF00;
		else g_testcolor = 0xFF0000;
	}*/
}
void c_gamearea::ev_update()
{
	// Increase entity capacity if needed
	if(m_entities.size() >= m_entities.capacity() / 2)
		m_entities.reserve(m_entities.capacity() * 2);

	// Move the viewport around
	if(m_follow)
	{
		const uchar *key = SDL_GetKeyboardState(NULL);

		if(key[SDL_SCANCODE_UP])
		{
			m_follow->onMovement(MOVE_UP);
		}
		if(key[SDL_SCANCODE_DOWN])
		{
			m_follow->onMovement(MOVE_DOWN);
		}
		if(key[SDL_SCANCODE_LEFT])
		{
			m_follow->onMovement(MOVE_LEFT);
		}
		if(key[SDL_SCANCODE_RIGHT])
		{
			m_follow->onMovement(MOVE_RIGHT);
		}

		// Make the box only appear inside the level
		s_point pos = m_follow->getPosition();
		s_bbox bbox = m_follow->getBBox();

		if(pos.x - bbox.left < 0) pos.x = bbox.left;
		else if(pos.x + bbox.right >= (int)m_width * GRID_W) pos.x = m_width * GRID_W - bbox.right;
		if(pos.y < bbox.top) pos.y = bbox.top;
		else if(pos.y + bbox.bottom >= (int)m_height * GRID_H) pos.y = m_height * GRID_H - bbox.bottom;

		m_follow->setPosition(pos);
	}

	// Make some collision detection
	for(uint i = 0; i < m_entities.size(); i++)
	{
		if(m_entities[i]->isDestroyed())
		{
			delete m_entities[i];
			m_entities.erase(m_entities.begin() + i);
			--i;
			continue;
		}

		if(!m_entities[i]->isEnabled()) continue;

		// Update the entity
		m_entities[i]->ev_cycle();

		// Move the entity and update it to the tileset and other entities
		s_point pos = m_entities[i]->getPosition();
		s_bbox bbox = m_entities[i]->getBBox();

		m_entities[i]->setOldPosition(pos);
		m_entities[i]->setPosition(pos + m_entities[i]->getVelocity());
		pos = m_entities[i]->getPosition();


		const uint start_y = (pos.y - bbox.top) / GRID_H;
		const uint stop_x = (pos.x + bbox.right < (int)m_width * GRID_W) ? (pos.x + bbox.right) / GRID_W + 1 : m_width;
		const uint stop_y = (pos.x + bbox.bottom < (int)m_height * GRID_H) ? (pos.y + bbox.bottom) / GRID_H + 1 : m_height;

		//uchar tile_flag;

		for(uint xx = (pos.x - bbox.left) / GRID_W; xx < stop_x; xx++)
		{
			if(pos.x - bbox.left < (int)xx * GRID_W + GRID_W && pos.x + bbox.right >= (int)xx * GRID_W)
			{
				for(uint yy = start_y; yy < stop_y; yy++)
				{
					if(pos.y - bbox.top < (int)yy * GRID_H + GRID_H && pos.y + bbox.bottom >= (int)yy * GRID_H)
					{
						// See if the tile is solid before more collision detection and response occur
						//tile_flag = m_tiles[xx][yy].flags;
						//if(tile_flag & TILE_SOLID)
						//{
						//	if(tile_flag & TILE_PRECISION)
						//	{
								if(m_pBitmask->bbox_inside(xx, yy, &m_tiles[xx][yy], pos.x, pos.y, bbox) == true)
								{
									m_entities[i]->onTileCollision(xx, yy, &m_tiles[xx][yy]);
								}
						//	}
						//	else
						//	{
						//		m_entities[i]->onTileCollision(xx, yy, &m_tiles[xx][yy]);
						//	}
						//}
					}
				}
			}
		}

		// Entity collision
		for(uint j = 0; j < m_entities.size(); j++)
		{
			if(i == j) continue;
			if(m_entities[j]->isDestroyed()) continue;
			if(!m_entities[j]->isEnabled()) continue;

			if(m_entities[j]->collide_bbox(pos, bbox))
				m_entities[i]->onCollision(m_entities[j]);
		}

		// After cycle and animate
		m_entities[i]->ev_cycleEnd();
		m_entities[i]->animate();
	}

	// Make the camera follow the box
	if(m_follow)
	{
		s_point pos = m_follow->getPosition();
		m_viewX = pos.x - 320;
		m_viewY = pos.y - 240;
		if((int)m_viewX < 0) m_viewX = 0;
		else if(m_viewX + 640 >= m_width * GRID_W) m_viewX = m_width * GRID_W - 640;
		if((int)m_viewY < 0) m_viewY = 0;
		else if(m_viewY + 480 >= m_height * GRID_H) m_viewY = m_height * GRID_H - 480;
	}

	// Animate the tiles
	for(uint i = 0; i < m_numTileAnimations; i++)
	{
		m_tileAnimation[i].animate();
	}
}
void c_gamearea::ev_render()
{
	// Render a temporary dark-blue background
	//draw_rect(0, 0, 640, 480, 0, 0, 0x7C);

	// Draw a background as a background
	for(uint i = 0; i < m_startForeground; i++)
	{
		m_background[i].draw((int)m_viewX, (int)m_viewY);
	}

	// Draw for the background tiles
	const uint viewW = (m_viewX + 640 < m_width  * GRID_W) ? (m_viewX + 640) / GRID_W + 1 : m_width;
	const uint viewH = (m_viewY + 480 < m_height * GRID_H) ? (m_viewY + 480) / GRID_H + 1 : m_height;
	const uint tile_w = m_viewX / GRID_W;
	const uint tile_h = m_viewY / GRID_H;
	SDL_Rect tile_pos = {0, 0, GRID_W, GRID_H};
	SDL_Rect clip = {0, 0, GRID_W, GRID_H};

	for(uint x = tile_w; x < viewW; x++)
	{
		for(uint y = tile_h; y < viewH; y++)
		{
			// Set the image coordinates of the tile
			for(int i = 0; i < TILE_LAYER_FORGROUND; i++)
			{
				tile_pos.x = GRID_W * x - m_viewX;
				tile_pos.y = GRID_H * y - m_viewY;
				clip.x = m_tiles[x][y].x[i] * GRID_W;
				clip.y = m_tiles[x][y].y[i] * GRID_H;
				draw_image(m_pTileset, &tile_pos, &clip);
			}
		}
	}

	// Draw entities
	for(uint i = 0; i < m_entities.size(); i++)
	{
		if(m_entities[i]->isEnabled())
		{
			m_entities[i]->ev_render(m_viewX, m_viewY);
		}
	}

	// Draw a foreground tileset using the same algorithm as for the background tiles, just with (fx,fy) instead of (ix,iy)
	for(uint x = tile_w; x < viewW; x++)
	{
		for(uint y = tile_h; y < viewH; y++)
		{
			for(int i = TILE_LAYER_FORGROUND; i < TILE_LAYERS; i++)
			{
				tile_pos.x = GRID_W * x - m_viewX;
				tile_pos.y = GRID_H * y - m_viewY;
				clip.x = m_tiles[x][y].x[i] * GRID_W;
				clip.y = m_tiles[x][y].y[i] * GRID_H;
				draw_image(m_pTileset, &tile_pos, &clip);
			}
		}
	}

	// Draw a background as a foreground
	for(uint i = m_startForeground; i < m_numBackgrounds; i++)
	{
		m_background[i].draw((int)m_viewX, (int)m_viewY);
	}

	/// Draw the Head Up Display

	// Draw health
	c_engine::getEngine()->drawHealth(48, 16);
}

void c_gamearea::ev_begin()
{
	m_pCurrentLevel = this;
}
void c_gamearea::ev_end()
{}

/// Private General Members
void c_gamearea::tile_addToAnimator(ushort x, ushort y, uchar layer)
{
	s_tile *tile = &m_tiles[x][y];

	// Find a tile animated
	for(uint i = 0; i < m_numTileAnimations; i++)
	{
		if(tile->y[layer] != m_tileAnimation[i].getRow()) continue;
		if(tile->x[layer] >= m_tileAnimation[i].getStart() && tile->x[layer] <= m_tileAnimation[i].getEnd())
		{
			m_tileAnimation[i].tile_add(tile, layer);
			return;
		}
	}
}
void c_gamearea::addEnvironment(ifstream &ff)
{
	s_environment env;
	ushort x, y, w, h;
	ff.read(reinterpret_cast<char*>(&x), sizeof(x));
	ff.read(reinterpret_cast<char*>(&y), sizeof(y));
	ff.read(reinterpret_cast<char*>(&w), sizeof(w));
	ff.read(reinterpret_cast<char*>(&h), sizeof(h));
	ff.read(reinterpret_cast<char*>(&env.type), sizeof(env.type));

	ff.read(reinterpret_cast<char*>(&env.value), sizeof(env.value));
	ff.read(reinterpret_cast<char*>(&env.flag), sizeof(env.flag));
	env.enabled = true;

	env.x = x * GRID_W;
	env.y = y * GRID_H;
	env.w = w * GRID_W;
	env.h = h * GRID_H;

	m_environments.push_back(env);
}

/// Public General Members
void c_gamearea::free()
{
	// Free the memory allocated by the backgrounds
	if(m_background)
	{
		delete[] m_background;
		m_background = NULL;
	}

	// Clear the tile array and set it to point to nothing
	if(m_tiles)
	{
		for(uint x = 0; x < m_width; x++)
		{
			delete[] m_tiles[x];
		}
		delete[] m_tiles;
		m_tiles = NULL;
	}
	if(m_tileAnimation)
	{
		delete[] m_tileAnimation;
		m_tileAnimation = NULL;
		m_numTileAnimations = 0;
	}

	// Clean the memory used by the tileset
	if(m_pTileset)
	{
		SDL_DestroyTexture(m_pTileset);
		m_pTileset = NULL;
	}
	if(m_pBitmask)
	{
		delete m_pBitmask;
		m_pBitmask = NULL;
	}

	// Remove the entities
	for(uint i = 0; i < m_entities.size(); i++)
	{
		delete m_entities[i];
	}
	m_entities.clear();
	m_environments.clear();
}
bool c_gamearea::load(const char *fname)
{
	ifstream ff;
	ff.open(fname);
	if(!ff.is_open())
	{
		return false;
	}

	// Read the security numbers
	char id[4];
	uint versionnr = 0;
	ff.read(id, 4);
	for(int i = 0; i < 4; i++)
	{
		if(id[i] != g_editorID[i])
		{
			ff.close();
			return false;
		}
	}
	ff.read(reinterpret_cast<char*>(&versionnr), sizeof(versionnr));
	if(versionnr != g_versionnr)
	{
		ff.close();
		return false;
	}

	/// Read header data
	free();

	int   theme;
	uchar ambient[3];
	uint  numEntities;

	ff.read(reinterpret_cast<char*>(&m_width), sizeof(m_width));
	ff.read(reinterpret_cast<char*>(&m_height), sizeof(m_height));
	ff.read(reinterpret_cast<char*>(&theme), sizeof(theme));
	ff.read(reinterpret_cast<char*>(&ambient[0]), 3);
	ff.read(reinterpret_cast<char*>(&m_numBackgrounds), sizeof(m_numBackgrounds));
	ff.read(reinterpret_cast<char*>(&m_startForeground), sizeof(m_startForeground));
	ff.read(reinterpret_cast<char*>(&numEntities), sizeof(numEntities));

	// Set the theme
	if(!setTheme(theme, ambient))
	{
		ff.close();
		return false;
	}

	// Set tileset
	//if(!set_tileset(g_themes[theme].tileset.c_str(), ambient[0], ambient[1], ambient[2]) || !set_bitmask(g_themes[theme].colmask.c_str()))
	//{
	//	return false;
	//}

	//create_tileAnimators(g_themes[theme].numAnimations);
	//for(uint i = 0; i < m_numTileAnimations; i++)
	//{
	//	m_tileAnimation[i].init(g_themes[theme].row[i], g_themes[theme].begframe[i], g_themes[theme].endframe[i], g_themes[theme].delay[i]);
	//}

	/// Read body data
	uchar f;

	m_tiles = new s_tile*[m_width];
	for(uint x = 0; x < m_width; x++)
	{
		m_tiles[x] = new s_tile[m_height];
		for(uint y = 0; y < m_height; y++)
		{
			ff.read(reinterpret_cast<char*>(&m_tiles[x][y].x[0]), TILE_LAYERS);
			ff.read(reinterpret_cast<char*>(&m_tiles[x][y].y[0]), TILE_LAYERS);
			ff.read(reinterpret_cast<char*>(&f), sizeof(f));

			for(uint l = 0; l < TILE_LAYERS; l++)
			{
				if(f & (uchar)pow(2, l))
				{
					tile_addToAnimator(x, y, (uchar)l);
				}
			}
		}
	}

	// Read entities
	uchar entityType;
	c_entity *entity = NULL;
	for(uint i = 0; i < numEntities; i++)
	{
		ff.read(reinterpret_cast<char*>(&entityType), sizeof(entityType));
		switch(entityType)
		{
			case ENTITY_TYPE_PLAYER:
				m_pPlayer = new c_player();
				entity_follow(m_pPlayer);
				entity = m_pPlayer;
				break;
			case 1: //ENTITY_TYPE_ENVIRONMENT:
				// Create an environment object instead of an entity
				addEnvironment(ff);
				entity = 0;
				break;
			case ENTITY_TYPE_CHECKPOINT:
				entity = new c_checkpoint();
				break;
			case ENTITY_TYPE_PLATFORM:
				entity = new c_platform();
				break;
			default:
				entity = 0;
				break;
		}

		if(entity)
		{
			entity_add(entity);
			entity->load(ff);
		}
	}

	/// Close the file
	ff.close();
	return true;
}
bool c_gamearea::setTheme(int theme, uchar color[])
{
	if(theme < 0 || theme >= MAX_THEMES) return false;

	ifstream ff(c_engine::getEngine()->getThemeName(theme).c_str(), ios::binary);
	if(ff.is_open() == false)
	{
		cerr << "Failed to load theme " << c_engine::getEngine()->getThemeName(theme) << endl;
		return false;
	}
	// Read file ID
	char id[4];
	uchar version;

	ff.read(id, 4);
	for(int i = 0; i < 4; i++)
	{
		if(id[i] != g_cThemeID[i])
		{
			ff.close();
			cerr << "Theme ID invalid!\n";
			cerr << id << " should be " << g_cThemeID << endl;
			return false;
		}
	}

	// Read version number
	ff.read(reinterpret_cast<char*>(&version), sizeof(version));
	if(version != 0)
	{
		ff.close();
		return false;
	}

	// Read header
	uint pathlength, numNodes;
	string tilesetpath;

	ff.read(reinterpret_cast<char*>(&pathlength), sizeof(pathlength));
	ff.seekg(pathlength, ios::cur);
	ff.read(reinterpret_cast<char*>(&pathlength), sizeof(pathlength));
	readBuffer(ff, tilesetpath, pathlength); // Tileset
	set_tileset(tilesetpath.c_str(), color[0], color[1], color[2]);

	ff.read(reinterpret_cast<char*>(&pathlength), sizeof(pathlength));
	readBuffer(ff, tilesetpath, pathlength); // Bitmask
	set_bitmask(tilesetpath.c_str());

	ff.read(reinterpret_cast<char*>(&numNodes), sizeof(numNodes));
	create_tileAnimators(numNodes);

	// Read body
	uchar  nodeBegin, nodeEnd, nodeRow;
	ushort nodeDelay;
	for(uint i = 0; i < numNodes; i++)
	{
		ff.read(reinterpret_cast<char*>(&nodeBegin), sizeof(nodeBegin));
		ff.read(reinterpret_cast<char*>(&nodeEnd), sizeof(nodeEnd));
		ff.read(reinterpret_cast<char*>(&nodeRow), sizeof(nodeRow));
		ff.read(reinterpret_cast<char*>(&nodeDelay), sizeof(nodeDelay));
		m_tileAnimation[i].init(nodeRow, nodeBegin, nodeEnd, nodeDelay);
	}

	ff.close();
	return true;
}

void c_gamearea::entity_add(c_entity *entity)
{
	// Put the entity into the vector sorted by depth
	if(entity)
	{
		if(m_entities.size() > 0)
		{
			vector<c_entity*>::iterator it;
			for(it = m_entities.begin(); it != m_entities.end(); it++)
			{
				if(entity->getDepth() < (*it)->getDepth())
				{
					m_entities.insert(it, entity);
					return;
				}
			}
		}
		m_entities.push_back(entity);
	}
}

// Collision code against tiles
bool c_gamearea::ptIn_environment(int x, int y, ushort type, s_environment *environment)
{
	// Check if position (x,y) is inside an environment of "type"
	s_environment *env;
	for(uint i = 0; i < m_environments.size(); i++)
	{
		env = &m_environments[i];

		if(!env->enabled) continue;
		if(env->type != type) continue;

		if(x < (int)env->x) continue;
		if(y < (int)env->y) continue;
		if(x >= (int)env->x + (int)env->w) continue;
		if(y >= (int)env->y + (int)env->h) continue;

		if(environment) *environment = *env;

		return true;
	}
	return false;
}
bool c_gamearea::bboxIn_environment(int x, int y, const s_bbox &bbox, ushort type, s_environment *environment)
{
	// Check if position (x,y) with mask 'bbox' is inside an environment of "type"
	s_environment *env;
	for(uint i = 0; i < m_environments.size(); i++)
	{
		env = &m_environments[i];

		if(!env->enabled) continue;
		if(env->type != type) continue;

		if(x + bbox.right  <  (int)env->x) continue;
		if(x - bbox.left   >= int(env->x + env->w)) continue;
		if(y - bbox.top    >= int(env->y + env->h)) continue;
		if(y + bbox.bottom <  (int)env->y) continue;

		if(environment) *environment = *env;

		return true;
	}
	return false;
}

bool c_gamearea::ptin_tile(int x, int y)
{
	/// Check whether point x,y is inside a solid tile

	// Make sure the point is not outside the level (return true as if they are inside a tile)
	if(x < 0) return true;
	if(y < 0) return true;
	if(x >= (int)m_width * GRID_W) return true;
	if(y >= (int)m_height * GRID_H) return true;

	const uint xx = x / GRID_W;
	const uint yy = y / GRID_H;

	//uchar tile_flag = m_tiles[xx][yy].flags;

	// Check if position (x,y) is inside the tile. If the tile also requires more precise accuracy check that too
	//if(tile_flag & TILE_SOLID)
	//{
	//	if(tile_flag & TILE_PRECISION)
	//	{
			if(m_pBitmask->point_inside(xx, yy, &m_tiles[xx][yy], x, y) == true)
				return true;
	//	}
	//	else return true;
	//}

	return false;
}
bool c_gamearea::ptin_tile(uint xx, uint yy, int x, int y)
{
	/// Check whether point x,y is inside a solid tile

	// Make sure the point is not outside the level (return true as if they are inside a tile)
	if(x < 0) return true;
	if(y < 0) return true;
	if(x >= (int)m_width * GRID_W) return true;
	if(y >= (int)m_height * GRID_H) return true;

	//uchar tile_flag = m_tiles[xx][yy].flags;

	// Check if position (x,y) is inside the tile. If the tile also requires more precise accuracy check that too
	//if(tile_flag & TILE_SOLID)
	//{
	//	if(tile_flag & TILE_PRECISION)
	//	{
			if(m_pBitmask->point_inside(xx, yy, &m_tiles[xx][yy], x, y) == true)
				return true;
	//	}
	//	else return true;
	//}

	return false;
}
bool c_gamearea::bbox_inTile(int x, int y, const s_bbox &bbox)
{
	// Make sure that the bounding box is inside the level. Outside of the level will count as a solid tile
	if(x - bbox.left < 0) return true;
	if(y - bbox.top < 0) return true;
	if(x + bbox.right >= (int)m_width * GRID_W) return true;
	if(y + bbox.bottom >= (int)m_height * GRID_H) return true;

	// Check if the bounding box at position x,y is inside or touching a solid tile
	const uint start_y = (y - bbox.top) / GRID_H;
	const uint stop_x = (x + bbox.right < (int)m_width * GRID_W) ? (x + bbox.right) / GRID_W + 1 : m_width;
	const uint stop_y = (x + bbox.bottom < (int)m_height * GRID_H) ? (y + bbox.bottom) / GRID_H + 1 : m_height;

	//uchar tile_flag;

	for(uint xx = (x - bbox.left) / GRID_W; xx < stop_x; xx++)
	{
		if(x - bbox.left < (int)xx * GRID_W + GRID_W && x + bbox.right >= (int)xx * GRID_W)
		{
			for(uint yy = start_y; yy < stop_y; yy++)
			{
				if(y - bbox.top < (int)yy * GRID_H + GRID_H && y + bbox.bottom >= (int)yy * GRID_H)
				{
					// See if the tile is solid before more collision detection and response occur
					//tile_flag = m_tiles[xx][yy].flags;
					//if(tile_flag & TILE_SOLID)
					//{
					//	if(tile_flag & TILE_PRECISION)
					//	{
							if(m_pBitmask->bbox_inside(xx, yy, &m_tiles[xx][yy], x, y, bbox) == true)
								return true;
					//	}
					//	else return true;
					//}
				}
			}
		}
	}
	return false;
}
bool c_gamearea::bbox_inTile(uint xx, uint yy, int x, int y, const s_bbox &bbox)
{
	// Make sure that the bounding box is inside the level. Outside of the level will count as a solid tile
	if(x - bbox.left < 0) return true;
	if(y - bbox.top < 0) return true;
	if(x + bbox.right >= (int)m_width * GRID_W) return true;
	if(y + bbox.bottom >= (int)m_height * GRID_H) return true;

	//uchar tile_flag;

	if(x - bbox.left < (int)xx * GRID_W + GRID_W && x + bbox.right >= (int)xx * GRID_W)
	{
		if(y - bbox.top < (int)yy * GRID_H + GRID_H && y + bbox.bottom >= (int)yy * GRID_H)
		{
			// See if the tile is solid before more collision detection and response occur
			//tile_flag = m_tiles[xx][yy].flags;
			//if(tile_flag & TILE_SOLID)
			//{
			//	if(tile_flag & TILE_PRECISION)
			//	{
					if(m_pBitmask->bbox_inside(xx, yy, &m_tiles[xx][yy], x, y, bbox) == true)
						return true;
			//	}
			//	else return true;
			//}
		}
	}
	return false;
}
void c_gamearea::move_bboxTile(int x, int y, const s_bbox &bbox, int direction, s_point &q)
{
	// Try to get the object in positon (x,y) to move to (x+vx,y+vy) as good as possible with 'bbox' as the collision rectangle.
	// The object will end at position (ex,ey) based on obstacles
	const uint start_y = (y - bbox.top) / GRID_H;
	const uint stop_x = (x + bbox.right < (int)m_width * GRID_W) ? (x + bbox.right) / GRID_W + 1 : m_width;
	const uint stop_y = (x + bbox.bottom < (int)m_height * GRID_H) ? (y + bbox.bottom) / GRID_H + 1 : m_height;

	//uchar tile_flag;
	q.x = x; q.y = y;

	for(int i = 0; i < 16; i++)
	{
		for(uint xx = (x - bbox.left) / GRID_W; xx < stop_x; xx++)
		{
			if(x - bbox.left < (int)xx * GRID_W + GRID_W && x + bbox.right >= (int)xx * GRID_W)
			{
				for(uint yy = start_y; yy < stop_y; yy++)
				{
					if(y - bbox.top < (int)yy * GRID_H + GRID_H && y + bbox.bottom >= (int)yy * GRID_H)
					{
						// See if the tile is solid before more collision detection and response occur
						//tile_flag = m_tiles[xx][yy].flags;
						//if(tile_flag & TILE_SOLID)
						//{
						//	if(tile_flag & TILE_PRECISION)
						//	{
								if(m_pBitmask->bbox_inside(xx, yy, &m_tiles[xx][yy], x, y, bbox) == true)
								{
									if(direction == 0) q.y--;
									else if(direction == 1) q.y++;
								}
						//	}
						//	else
						//	{
						//		if(direction == 0) q.y--;
						//		else if(direction == 1) q.y++;
						//	}
						//}
					}
				}
			}
		}
	}
}
bool c_gamearea::bbox_inSolid(int x, int y, const s_bbox &bbox, c_entity *ent)
{
	if(bbox_inTile(x, y, bbox) == true) return true;

	c_entity *e;
	s_point B;
	s_bbox boxB;

	for(uint i = 0; i < m_entities.size(); i++)
	{
		e = m_entities[i];
		if(e->isDestroyed()) continue;

		if(e->getEntityType() == ENTITY_TYPE_PLATFORM)
		{
			c_platform *platform = (c_platform*)e;
			B = e->getPosition();
			boxB = e->getBBox();

			// See if 'bbox' is touching/inside the solid entity
			if(x + bbox.right  <  B.x - boxB.left) continue;
			if(x - bbox.left   >= B.x + boxB.right) continue;
			if(y + bbox.bottom <  B.y - boxB.top) continue;
			if(y - bbox.top    >= B.y + boxB.bottom) continue;

			if(platform_get(platform->getType())->getType() == PLATFORM_LIFT && ent)
			{
				// Lifts have more conditions
				int oldY = int(ent->getOldPosition().y);
				if(/*m_position.y > m_oldPosition.y && */
				        y + bbox.bottom >= B.y - boxB.top &&
				        oldY + bbox.bottom <= B.y - boxB.top &&
				        ent->getVelocity().y >= 0)
				//if(y > oldY && oldY + bbox.bottom < B.y - bbox.top && ent->getVelocity().y > 0)
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

/// Accessors

/// Experimental member functions
bool c_gamearea::set_tileset(const char *fname, uchar r, uchar g, uchar b)
{
	uchar color[3] = {r, g, b};
	m_pTileset = mm_loadImageAmbient(fname, color);
	if(m_pTileset == NULL)
	{
		cout << "Failed to load tileset \'" << fname << "\'!\n";
		return false;
	}
	return true;
}
bool c_gamearea::set_bitmask(const char *fname)
{
	if(m_pBitmask == NULL) m_pBitmask = new c_bitmask();
	if( m_pBitmask->load(fname) == false )
	{
		cout << "Failed to load bitmask \'" << fname << "\'!\n";
		return false;
	}
	return true;
}
void c_gamearea::set_tile(uint x, uint y, int i, uchar ix, uchar iy)
{
	if(x < m_width && y < m_height)
	{
		m_tiles[x][y].x[i] = ix;
		m_tiles[x][y].y[i] = iy;
	}
}
void c_gamearea::set_tileflag(uint x, uint y, uchar flag)
{
	if(x < m_width && y < m_height)
	{
		//m_tiles[x][y].flags = flag;
	}
}
void c_gamearea::create_tileAnimators(uint numNodes)
{
	m_numTileAnimations = numNodes;
	m_tileAnimation = new c_tile_animNode[numNodes];
}

void c_gamearea::create_backgrounds(uint n)
{
	m_numBackgrounds = n;
	m_background = new c_background[n];
}

bool c_gamearea::test_level()
{
	m_newLevel = true;
	if(!load("../test.lvl"))
	//if(!load("../d0l2.lvl"))
	{
		cerr << "Failed to load the level!" << endl;
		return false;
	}
	if(m_newLevel)
	{
		c_engine::getEngine()->setCheckPoint(m_pPlayer->getPosition().x, m_pPlayer->getPosition().y);
	}
	m_newLevel = false;

	// Create backgrounds manually
	create_backgrounds(1);
	m_startForeground = 1;

	if( m_background[0].init("data/bck_LDay.png", 0, 0, 0, 0) == false )
	{
		cout << "Failed to load a background!\n";
		return false;
	}

	// Level creation completed
	return true;
}

/** Implement global functions **/
void readBuffer(ifstream &ff, string &str, uint size)
{
	char *buffer = new char[size + 1];
	ff.read(buffer, size);
	buffer[size] = '\0';
	str = buffer;
	delete[] buffer;
}
