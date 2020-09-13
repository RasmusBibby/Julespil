/** Main file for game **/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "base.h"
#include "engine.h"
#include "event_handler.h"

#include "level.h"
#include "gamearea.h"

#include "player.h"
#include "checkpoint.h"
#include "platform.h"

#include "tile.h"
#include "background.h"

#include <cstdio>

using namespace std;

/** Declare global Variables, Pointers and Functions **/
SDL_Window		*g_pMainWindow = NULL;
SDL_Renderer	*g_pMainRenderer = NULL;
bool		g_bRunGame = true;
const int	g_iMaxFrames = 30;

/** Declare main.cpp Funtions **/
bool main_initialise()
{
	/// Initialize SDL and other important libraries
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return false;
	}

	// Setup the window and renderer
	if( (g_pMainWindow = SDL_CreateWindow("X-mas game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL /**| SDL_WINDOW_FULLSCREEN_DESKTOP*/)) == NULL)
	{
		printf("Failed to create the window: %s\n", SDL_GetError());
		return false;
	}
	g_pMainRenderer = SDL_CreateRenderer(g_pMainWindow, -1, 0);
	if(g_pMainRenderer == NULL)
	{
		printf("Failed to create a renderer: %s\n", SDL_GetError());
		return false;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(g_pMainRenderer, 640, 480);
	SDL_SetRenderDrawColor(g_pMainRenderer, 0, 0, 0, 0xFF);

	// Activate the SDL image library
	if( !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) )
	{
		printf("Failed to initialize SDL_image: %s\n", IMG_GetError());
		return false;
	}

	// Initialize keyboard
	keyboard_init();

	return true;
}
void main_quit()
{
	// Free and shutdown SDL
	if(g_pMainRenderer) SDL_DestroyRenderer(g_pMainRenderer);
	if(g_pMainWindow) SDL_DestroyWindow(g_pMainWindow);

	g_pMainRenderer = NULL;
	g_pMainWindow = NULL;

	keyboard_free();

	IMG_Quit();
	SDL_Quit();
}

/** Implement the main function **/
int main(int argc, char *argv[])
{
	// Initialise SDL and other systems
	if(main_initialise() == false)
	{
		main_quit();
		return 1;
	}

	// Add a level to the game
	c_engine engine;

	if(engine.loadMedia() == false)
	{
		main_quit();
		return 1;
	}

	c_gamearea *gamearea = new c_gamearea();
	engine.addLevel(gamearea);
	engine.gotoLevel(0);

	// Add a entity object types
	c_playertype *pt = new c_playertype();
	if(pt->createImage("data/player_sprite/spr_player_14a.png", 14))
	{
		pt->setBBox(11, 11, 31, 30);
		pt->initAnimations(9);
		pt->setAnimation(0,  0,  0); // Standing
		pt->setAnimation(1,  0,  3); // Move
		pt->setAnimation(2,  4,  5); // Climb ladder
		pt->setAnimation(3,  6,  7); // Climb rope
		pt->setAnimation(4,  8,  8); // Jump/Fall
		pt->setAnimation(5,  9,  9); // Aiming snowball
		pt->setAnimation(6,  9, 11); // Throw snowball
		pt->setAnimation(7, 11, 12); // Throw object
		pt->setAnimation(8, 13, 13); // Remote control
		player_add(pt);
	}
	else
	{
		printf("Failed to initialize the player!\n");
		g_bRunGame = false;
	}
	platform_add(new c_platformtype("data/images/platform_sm1Fac.png", 1, 1, PLATFORM_LIFT));
	platform_add(new c_platformtype("data/images/platform_lr1Fac.png", 3, 1, PLATFORM_LIFT));
	platform_add(new c_platformtype("data/images/door_1Fac.png", 1, 3, PLATFORM_DOOR));
	platform_add(new c_platformtype("data/images/box_1Fac.png", 1, 1, PLATFORM_PUSHABLE));

	// Prepare a test level
	if(gamearea->test_level() == false)
	{
		printf("Failed to initialize the level!\n");
		g_bRunGame = false;
	}

	/// Run through the main loop
	SDL_Event	ev;
	uint		uiGameticks;

	while(g_bRunGame)
	{
		// Seed the random generator
		uiGameticks = SDL_GetTicks();
		srand(uiGameticks);

		while(SDL_PollEvent(&ev))
		{
			keyboard_update(ev);

			// End the game
			if(ev.type == SDL_QUIT) g_bRunGame = false;
			else if(ev.type == SDL_KEYDOWN)
			{
				if(ev.key.keysym.sym == SDLK_ESCAPE) g_bRunGame = false;
			}

			// Handle level events
			if(engine.currentLevel()) engine.currentLevel()->ev_input(ev);
		}

		// Update the game cycle
		if(engine.currentLevel()) engine.currentLevel()->ev_update();
		engine.changeLevel();

		// Clear the screen and then render everything until putting it on the screen
		SDL_RenderClear(g_pMainRenderer);

		// Render the level
		if(engine.currentLevel()) engine.currentLevel()->ev_render();
		engine.drawTransistion();

		SDL_RenderPresent(g_pMainRenderer);

		// Keep track of the frames
		if(1000 / 30 > SDL_GetTicks() - uiGameticks)
			SDL_Delay((1000 / g_iMaxFrames) - (SDL_GetTicks() - uiGameticks));
	}
	/// Free the memory for any game content used by the game
	player_free();
	platform_free();

	/// Exit the libraries used by the program before close-fown
	main_quit();
	return 0;
}
