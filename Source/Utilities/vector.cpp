/*
 * Filename      : vector.cpp
 * Author(s)     : Douglas Moyes (dbmoyes@gmail.com), Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

// Some additional functions for the Vector class that were
// too big to inline, or for whatever reason wound up in here. All
// functions are described fully in the header file.

#include "vector.h"

// double Vector::radangle()
// Polar form. Returns the angle in radians.
double Vector::radangle(){
#ifndef USEATAN2	
	register double angle;
	// if there is no imaginary part, the value is zero so
	// no need to go further.
	if(j==0)return 0; 
	
	// stuff to avoid dividing by zero before calling
	// atan().
	if(i==0 && j>0)return V_PI2;
	if(i==0 && j<0)return V_3PI2;

	// now it's safe to find the arctanget.
	angle=atan(j/i);

	//Fix the angle if we are in quadrant II or III
	if(i<0)angle+=V_PI;

	//Make the angle positive if we are in quadrant IV.
	if(j<0 && i>0)angle+=V_2PI;
	return angle
#else
	return atan2(j,i);
#endif
		
}//end radangle()

ostream& operator<<(ostream &out, const Vector &v ) {
	out << "(" << v.i << ", " << v.j << ")";
	return out;
}
