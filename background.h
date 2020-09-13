/** Header file for the background class **/
#ifndef _BACKGROUND_H
#define _BACKGROUND_H
/** Included files **/
#include "base.h"

/** Define macros **/

/** Declare classes **/
class c_image;

class c_background
{
	private:
		c_image	*m_image;

		// Movement
		int		m_x, m_y; // Current position of the background
		short	m_spdX, m_spdY; // A constant movement, independent of the movement of the camera
		short	m_camX, m_camY; // A movement dependent on the position of the camera
	public:
		c_background();
		~c_background();

		// General functions
		bool	init(const char *fname, short spdX, short spdY, short camX, short camY);
		void	update();
		void	draw(int view_x, int view_y);

		// Accessor functions
};

/** Endif **/
#endif
