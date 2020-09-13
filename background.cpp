/** Source file for the background class implementations **/
#include "background.h"
#include "multimedia.h"

#include <SDL2/SDL.h>

/** Implement c_background class functions **/
c_background::c_background()
{
	m_image = NULL;

	m_x = m_y = 0;
	m_spdX = m_spdY = 0;
	m_camX = m_camY = 0;
}
c_background::~c_background()
{
	if(m_image)
	{
		delete m_image;
		m_image = NULL;
	}
}

/// General functions
bool c_background::init(const char *fname, short spdX, short spdY, short camX, short camY)
{
	if(m_image == NULL) m_image = new c_image();
	if( m_image->load(fname) == false)
	{
		return false;
	}

	// Set the movement
	m_spdX = spdX;
	m_spdY = spdY;

	m_camX = camX;
	m_camY = camY;

	// Creation of the background succeeded
	return true;
}
void c_background::update()
{
	// Move the background
	if(m_spdX != 0)
	{
		// Move the background horizontally and check if it has gone out of borders
		m_x += m_spdX / (m_image->width() * 100);
		if(m_x > (int)m_image->width()) m_x -= m_image->width();
		else if(m_x < 0) m_x += m_image->width();
	}
	if(m_spdY != 0)
	{
		// Do the same, just vertically
		m_y += m_spdY / (m_image->height() * 100);
		if(m_y > (int)m_image->height()) m_y -= m_image->height();
		else if(m_y < 0) m_y += m_image->height();
	}
}
void c_background::draw(int view_x, int view_y)
{
	// Get the size of the viewport
	//const uint WIDTH = 640;
	//const uint HEIGHT = 480;

	// Set the camera position
	//int cam_x = 0, cam_y = 0;
	//if(m_camX != 0) cam_x = m_camX / (m_image->width()  * 100);
	//if(m_camY != 0) cam_y = m_camY / (m_image->height() * 100);

	// Calculate the position and how many times the image should be rendered
	m_image->draw(0, 0);
}
