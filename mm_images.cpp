/** Source file for image and graphical handling **/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "multimedia.h"

#include <cstdio>

using namespace std;

/** Declare global variables and pointers, external and internal **/
extern SDL_Renderer *g_pMainRenderer;

uint get_pixel(SDL_Surface *image, int x, int y);
void set_pixel(SDL_Surface *image, int x, int y, uint color);

/*************************************************************/
/**          Implement the c_image class functions          **/
/*************************************************************/
c_image::c_image()
{
	m_width = 0;
	m_height = 0;
	m_numFrames = 0;
	m_image = NULL;
}
c_image::~c_image()
{
	free();
}

/// General functions
void c_image::createBlank(uint width, uint height, uchar r, uchar g, uchar b, uchar a)
{
	free();
	SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, r, g, b, a);
	if(surface == NULL)
	{
		return;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(g_pMainRenderer, surface);
	SDL_FreeSurface(surface);
	if(texture == NULL)
	{
		return;
	}

	m_numFrames = 1;
	m_image = new SDL_Texture*[1];
	m_width = width;
	m_height = height;
	m_image[0] = texture;

	SDL_SetTextureBlendMode(m_image[0], SDL_BLENDMODE_BLEND);
}
bool c_image::load(const char *fname)
{
	// Load a surface to later create the texture
	free();

	SDL_Surface *surface = IMG_Load(fname);
	if(surface == NULL)
	{
		printf("Failed load %s: %s\n", fname, IMG_GetError());
		return false;
	}
	uint w = surface->w;
	uint h = surface->h;

	// Create the texture from the loaded surface
	SDL_Texture *texture = SDL_CreateTextureFromSurface(g_pMainRenderer, surface);
	SDL_FreeSurface(surface);
	if(texture == NULL)
	{
		printf("Failed to convert the surface into a texture: %s\n", SDL_GetError());
		return false;
	}

	m_numFrames = 1;
	m_image = new SDL_Texture*[1];

	m_width = w;
	m_height = h;
	m_image[0] = texture;

	return true;
}
bool c_image::load(const char *fname, uint frames)
{
	if(frames <= 1) return load(fname);

	// Load a surface to later create the texture
	free();

	SDL_Surface *surface = IMG_Load(fname);
	if(surface == NULL)
	{
		printf("Failed load %s: %s\n", fname, IMG_GetError());
		return false;
	}

	m_numFrames = frames;
	m_image = new SDL_Texture*[frames];
	for(uint i = 0; i < m_numFrames; i++) m_image[i] = NULL;
	m_width = surface->w / frames;
	m_height = surface->h;

	// Split the SDL_Surface into smaller surfaces
	SDL_Surface *split = NULL;
	bool success = true;

	for(uint i = 0; i < m_numFrames; i++)
	{
		// Create a part of the image
		split = SDL_CreateRGBSurface(SDL_SWSURFACE, m_width, m_height, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

		// Take those pixels inside (x,y,w,h)
		if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
		uint pixel;
		for(uint xx = m_width * i, nx = 0; xx < m_width * i + m_width; xx++, nx++)
		{
			for(uint yy = 0, ny = 0; yy < m_height; yy++, ny++)
			{
				pixel = get_pixel(surface, (int)xx, (int)yy);
				set_pixel(split, (int)nx, (int)ny, pixel);
			}
		}
		if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

		m_image[i] = SDL_CreateTextureFromSurface(g_pMainRenderer, split);
		SDL_FreeSurface(split);
		if(!m_image[i])
		{
			printf("Failed to convert the surface into a texture: %s\n", SDL_GetError());
			success = false;
		}
	}
	SDL_FreeSurface(surface);
	return success;
}
bool c_image::loadFlipped(const char *fname, uint frames)
{
	if(frames <= 1) frames = 1;

	// Load a surface to later create the texture
	free();

	SDL_Surface *surface = IMG_Load(fname);
	if(surface == NULL)
	{
		printf("Failed load %s: %s\n", fname, IMG_GetError());
		return false;
	}

	m_numFrames = frames;
	m_image = new SDL_Texture*[frames];
	for(uint i = 0; i < m_numFrames; i++) m_image[i] = NULL;
	m_width = surface->w / frames;
	m_height = surface->h;

	// Split the SDL_Surface into smaller surfaces
	SDL_Surface *split = NULL;
	bool success = true;

	for(uint i = 0; i < m_numFrames; i++)
	{
		// Create a part of the image
		split = SDL_CreateRGBSurface(SDL_SWSURFACE, m_width, m_height, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

		// Take those pixels inside (x,y,w,h)
		if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
		uint pixel;
		for(uint xx = m_width * i, nx = m_width - 1; xx < m_width * i + m_width; xx++, nx--)
		{
			for(uint yy = 0, ny = 0; yy < m_height; yy++, ny++)
			{
				pixel = get_pixel(surface, (int)xx, (int)yy);
				set_pixel(split, (int)nx, (int)ny, pixel);
			}
		}
		if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

		m_image[i] = SDL_CreateTextureFromSurface(g_pMainRenderer, split);
		SDL_FreeSurface(split);
		if(!m_image[i])
		{
			printf("Failed to convert the surface into a texture: %s\n", SDL_GetError());
			success = false;
		}
	}
	SDL_FreeSurface(surface);
	return success;
}
void c_image::free()
{
	if(m_image)
	{
		for(uint i = 0; i < m_numFrames; i++)
		{
			if(m_image[i]) SDL_DestroyTexture(m_image[i]);
			m_image[i] = NULL;
		}
		delete[] m_image;
		m_numFrames = 0;

		m_width = 0;
		m_height = 0;
	}
}
void c_image::draw(int x, int y, uint frame)
{
	if(frame >= m_numFrames) frame = m_numFrames - 1;
	SDL_Rect pos = {x, y, (int)m_width, (int)m_height};
	SDL_RenderCopy(g_pMainRenderer, m_image[frame], NULL, &pos);
}

void c_image::setAlpha(uchar alpha)
{
	SDL_SetTextureAlphaMod(m_image[0], alpha);
}

/**************************************/
/**    Implement global functions    **/
/**************************************/
uint get_pixel(SDL_Surface *image, int x, int y)
{
	int bpp = image->format->BytesPerPixel;
	Uint8 *p = (Uint8*)image->pixels + y * image->pitch + x * bpp;
	switch(bpp)
	{
		case 1: return *p;
		case 2: return *(Uint16*)p;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else return p[0] | p[1] << 8 | p[2] << 16;
		case 4: return *(Uint32*)p;
		default: return 0;
	}
	return 0;
}
void set_pixel(SDL_Surface *image, int x, int y, uint color)
{
	int bpp = image->format->BytesPerPixel;
	Uint8 *p = (Uint8*)image->pixels + y * image->pitch + x * bpp;
	switch(bpp)
	{
		case 1:
			*p = color;
			break;
		case 2:
			*(Uint16*)p = color;
			break;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (color >> 16) & 0xFF;
				p[1] = (color >> 8) & 0xFF;
				p[2] = color & 0xFF;
			}
			else
			{
				p[0] = color & 0xFF;
				p[1] = (color >> 8) & 0xFF;
				p[2] = (color >> 16) & 0xFF;
			}
			break;
		case 4:
			*(Uint32*)p = color;
			break;
	}
}

/// Loading functions
SDL_Texture *mm_loadImage(const char *fname)
{
	// Load a surface to later create the texture
	SDL_Surface *surface = IMG_Load(fname);
	if(surface == NULL)
	{
		printf("Failed load %s: %s\n", fname, IMG_GetError());
		return NULL;
	}

	// Create the texture from the loaded surface
	SDL_Texture *texture = SDL_CreateTextureFromSurface(g_pMainRenderer, surface);
	SDL_FreeSurface(surface);
	if(texture == NULL)
	{
		printf("Failed to convert the surface to a texture: %s\n", SDL_GetError());
		return NULL;
	}

	return texture;
}
SDL_Texture *mm_loadImageAmbient(const char *fname, const uchar color[3])
{
	// Load an image from a file with changed colors
	SDL_Surface *image = IMG_Load(fname);
	if(image == NULL)
	{
		printf("Failed to load %s: %s\n", fname, IMG_GetError());
		return NULL;
	}

	// Set the color to floating values
	float fColor[3];
	for(int i = 0; i < 3; i++)
	{
		fColor[i] = (float)color[i] / 255.0f;
	}

	// Manipulate each pixel's color in the image
	uchar r, g, b, a;
	if(SDL_MUSTLOCK(image)) SDL_LockSurface(image);
	for(int x = 0; x < image->w; x++)
	{
		for(int y = 0; y < image->h; y++)
		{
			SDL_GetRGBA(get_pixel(image, x, y), image->format, &r, &g, &b, &a);
			r *= fColor[0]; g *= fColor[1]; b *= fColor[2];
			set_pixel(image, x, y, SDL_MapRGBA(image->format, r, g, b, a));
		}
	}

	if(SDL_MUSTLOCK(image)) SDL_UnlockSurface(image);

	// Convert "image" to a texture
	SDL_Texture *texture = SDL_CreateTextureFromSurface(g_pMainRenderer, image);
	SDL_FreeSurface(image);
	if(texture == NULL)
	{
		printf("Failed to create an ambient texture: %s\n", SDL_GetError());
	}
	return texture;
}

/// Rendering functions
void draw_rect(SDL_Rect rect, uchar r, uchar g, uchar b, uchar a)
{
	SDL_SetRenderDrawColor(g_pMainRenderer, r, g, b, a);
	SDL_RenderFillRect(g_pMainRenderer, &rect);
}
void draw_rect(int x, int y, uint w, uint h, uchar r, uchar g, uchar b, uchar a)
{
	SDL_Rect rect;
	rect.x = x; rect.y = y;
	rect.w = w; rect.h = h;
	draw_rect(rect, r, g, b, a);
}
void draw_image(SDL_Texture *image, SDL_Rect *pos, SDL_Rect *clip)
{
	SDL_RenderCopy(g_pMainRenderer, image, clip, pos);
}
