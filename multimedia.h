/** Header file for the multimedia classes **/
#ifndef _MULTIMEDIA_H
#define _MULTIMEDIA_H
/** Included files **/
#include <SDL2/SDL.h>
#include "base.h"

/** Declare classes **/
struct s_animation
{
	uint begframe, endframe;
};

class c_image
{
	private:
		uint	m_width, m_height;
		uint	m_numFrames;
		SDL_Texture	**m_image;

	public:
		c_image();
		~c_image();

		// General functions
		void	createBlank(uint width, uint height, uchar r, uchar g, uchar b, uchar a = 0xFF);
		bool	load(const char *fname);
		bool	load(const char *fname, uint frames);
		bool	loadFlipped(const char *fname, uint frames);
		void	free();
		void	draw(int x, int y, uint frame = 0);

		//Accessor functions
		uint	width()const		{ return m_width; };
		uint	height()const		{ return m_height; };
		uint	numFrames()const	{ return m_numFrames; };
		SDL_Texture	*image(uint frame = 0)const	{ return m_image[frame]; };

		void	setAlpha(uchar alpha);
};

/** Declare global functions **/

// Images and graphics
SDL_Texture	*mm_loadImage(const char*);
SDL_Texture	*mm_loadImageAmbient(const char*, const uchar[3]);

void	draw_rect(SDL_Rect rect, uchar r, uchar g, uchar b, uchar a);
void	draw_rect(int x, int y, uint w, uint h, uchar r, uchar g, uchar b, uchar a = 0xFF);
void	draw_image(SDL_Texture *image, SDL_Rect *pos, SDL_Rect *clip = NULL);

// Fonts

// Sounds, audio and music

/** Endif **/
#endif
