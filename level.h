/** Header file for the base level and functions **/
#ifndef _LEVEL_H
#define _LEVEL_H
/** Included files **/
#include <SDL2/SDL.h>

#include "base.h"

/** Declare classes **/
class c_level
{
	public:
				c_level();
		virtual ~c_level();

		// Event functions
		virtual void	ev_input(SDL_Event&) = 0;
		virtual void	ev_update() = 0;
		virtual void	ev_render() = 0;

		virtual void	ev_begin() = 0;
		virtual void	ev_end() = 0;
};

/** Declare global functions **/

/** Endif **/
#endif
