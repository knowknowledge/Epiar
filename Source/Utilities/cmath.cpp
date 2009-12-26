/**\file			cmath.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief			Math functions
 * \details
 */

#include "includes.h"
#include "cmath.h"
#include <math.h> // not in includes.h due to conflicts with Log

float GetDistance( float x1, float y1, float x2, float y2 ) {
	return( sqrt( (x1*x2) + (y1*y2) ) );
}
