/**\file			coordinate.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Utilities/camera.h"
#include "Utilities/coordinate.h"
#include "Utilities/trig.h"

/**\class Coordinate
 * \brief Coordinates. */

Coordinate::Coordinate() {
	m_x = m_y = 0;
	boundary.top = boundary.right = boundary.bottom = boundary.left = 0.;
}

Coordinate::Coordinate( double x, double y ) {
	m_x = x;
	m_y = y;
	boundary.top = boundary.right = boundary.bottom = boundary.left = 0.;
}

Coordinate::Coordinate( double top, double right, double bottom, double left ) {
	Coordinate();
	boundary.top = top;
	boundary.right = right;
	boundary.bottom = bottom;
	boundary.left = left;
}

Coordinate::~Coordinate () {

}

double  Coordinate::GetX () const {
	return ( m_x );
}

void  Coordinate::SetX ( double x ) {
	m_x = x;
}

double  Coordinate::GetY () const {
	return ( m_y );
}

void  Coordinate::SetY ( double y ) {
	m_y = y;
}

// Returns a SDL Rectangle using width/height of 0
SDL_Rect  Coordinate::getRect () {
	SDL_Rect rect;
	rect.x = (int) m_x;
	rect.y = (int) m_y;
	rect.w = rect.h = 0;
	return ( rect );
}

// Returns SDL Rectangle with given width and height
SDL_Rect  Coordinate::getRectWithDim ( int w, int h ) {
	SDL_Rect rect;
	rect.x = (int) m_x;
	rect.y = (int) m_y;
	rect.w = w;
	rect.h = h;
	return ( rect );
}

void VelocityAdjCoordinate(Coordinate& cor,Vector v, double t){
	Vector delta;
	delta = v*t;
	
	cor.m_x += delta.ipart();
	cor.m_y += delta.jpart();
}

int Coordinate::GetScreenX() {
	Camera *camera = Camera::Instance();
	Coordinate screen;
	
	camera->Translate( *this, screen );

	return( (int)screen.GetX() );
}

int Coordinate::GetScreenY() {
	Camera *camera = Camera::Instance();
	Coordinate screen;
	
	camera->Translate( *this, screen );
	
	return( (int)screen.GetY() );
}

float Coordinate::GetAngle() {
	// 0 is right, 90 is up
	// Due to the way coordinates are displayed, use negative Y
	Trig *trig = Trig::Instance();
	return static_cast<float>(trig->RadToDeg( static_cast<float>(atan2( static_cast<float>(-m_y)
					,static_cast<float>( m_x ))) ));
}

float Coordinate::GetMagnitude() {
	return static_cast<float>(sqrt( m_y*m_y + m_x*m_x ));
}

ostream& operator<<(ostream &out, const Coordinate &c) {
	out << "(" << c.m_x << ", " << c.m_y << ")";
	return out;
}

// Sets bounadry caps on coordinates - used for momentum capping as well
// Must be positive
// Warning, boundaries are not enforced in subtraction and addition, but
// are enforced on += and -=
void Coordinate::SetBoundaries( double top, double right, double bottom, double left ) {
	boundary.top = top;
	boundary.right = right;
	boundary.bottom = bottom;
	boundary.left = left;
}

// Ensures coordinates are within boundaries, if set
void Coordinate::EnforceBoundaries( void ) {
	if( boundary.top != 0. ) {
		if( m_y > 0. ) {
			if( m_y > boundary.top ) m_y = boundary.top;
		}
	}
	if( boundary.right != 0. ) {
		if( m_x > 0. ) {
			if( m_x > boundary.right ) m_x = boundary.right;
		}
	}
	if( boundary.bottom != 0. ) {
		if( m_y < 0. ) {
			if( (-1 * m_y) > boundary.bottom ) m_y = -1 * boundary.bottom;
		}
	}
	if( boundary.left != 0. ) {
		if( m_x < 0. ) {
			if( (-1 * m_x) > boundary.left ) m_x = -1 * boundary.left;
		}
	}	
}

bool Coordinate::ViolatesBoundary() {
	if( m_x < boundary.left ) return( true );
	if( m_x > boundary.right ) return( true );
	if( m_y < boundary.top ) return( true );
	if( m_y > boundary.bottom ) return( true );
	
	return( false );
}
