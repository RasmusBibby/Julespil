/** event_handler header file **/
#ifndef _EVENTHANDLER_H
#define _EVENTHANDLER_H
/** Define enumerations **/
#include <SDL2/SDL_events.h>

/** Define global functions **/
void	keyboard_init();
void	keyboard_free();
void	keyboard_update(SDL_Event ev);
bool	key_pressed(int key);
bool	key_released(int key);

/** endif **/
#endif
