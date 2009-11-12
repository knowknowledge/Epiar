/*
 * Filename      : cmath.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "cmath.h"
#include "includes.h"
#include <math.h> // not in includes.h due to conflicts with Log

float GetDistance( float x1, float y1, float x2, float y2 ) {
	return( sqrt( (x1*x2) + (y1*y2) ) );
}
