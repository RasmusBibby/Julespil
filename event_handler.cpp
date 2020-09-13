/** event_handler source file **/
#include "event_handler.h"

/**  Declare global variables  **/
static bool	*g_bKeyState;
static bool	*g_bKeyScan;

/** Implement global functions **/
void keyboard_init()
{
	int numKeys;
	SDL_GetKeyboardState(&numKeys);
	g_bKeyState = new bool[numKeys];
	g_bKeyScan = new bool[numKeys];
	for(int i = 0; i < numKeys; i++)
	{
		g_bKeyState[i] = false;
		g_bKeyScan[i] = false;
	}
}
void keyboard_free()
{
	if(g_bKeyState) delete[] g_bKeyState;
	if(g_bKeyScan) delete[] g_bKeyScan;

	g_bKeyState = NULL;
	g_bKeyScan = NULL;
}
void keyboard_update(SDL_Event ev)
{
	if(ev.type == SDL_KEYDOWN)
		g_bKeyState[ev.key.keysym.scancode] = true;
	else if(ev.type == SDL_KEYUP)
		g_bKeyState[ev.key.keysym.scancode] = false;
}
bool key_pressed(int key)
{
	if(g_bKeyState[key] != g_bKeyScan[key])
	{
		g_bKeyScan[key] = !g_bKeyScan[key];
		return true;
	}
	return false;
}
bool key_released(int key)
{
	if(g_bKeyState[key] != g_bKeyScan[key])
	{
		g_bKeyScan[key] = !g_bKeyScan[key];
		return true;
	}
	return false;
}
