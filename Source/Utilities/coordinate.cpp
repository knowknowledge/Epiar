/**\file			coordinate.cpp
 * \author			Chris Thielen (chris@epiar.net)
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
}

Coordinate::Coordinate( double x, double y ) {
	m_x = x;
	m_y = y;
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
	
	camera->TranslateWorldToScreen( *this, screen );

	return( (int)screen.GetX() );
}

int Coordinate::GetScreenY() {
	Camera *camera = Camera::Instance();
	Coordinate screen;
	
	camera->TranslateWorldToScreen( *this, screen );
	
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

// Ensures coordinates are within boundaries, if set
void Coordinate::EnforceBoundaries( double top, double right, double bottom, double left ) {
	if( top != 0. ) {
		if( m_y > 0. ) {
			if( m_y > top ) m_y = top;
		}
	}
	if( right != 0. ) {
		if( m_x > 0. ) {
			if( m_x > right ) m_x = right;
		}
	}
	if( bottom != 0. ) {
		if( m_y < 0. ) {
			if( (-1 * m_y) > bottom ) m_y = -1 * bottom;
		}
	}
	if( left != 0. ) {
		if( m_x < 0. ) {
			if( (-1 * m_x) > left ) m_x = -1 * left;
		}
	}	
}

bool Coordinate::ViolatesBoundary( double top, double right, double bottom, double left ) {
	if( m_x < left ) return( true );
	if( m_x > right ) return( true );
	if( m_y < top ) return( true );
	if( m_y > bottom ) return( true );
	
	return( false );
}

void Coordinate::EnforceMagnitude(double radius) {
	if( GetMagnitudeSquared() > radius*radius ) {
		Trig *trig = Trig::Instance();
		int angle = TO_INT(GetAngle());
		m_x = trig->GetCos( angle )*radius;
		m_y = -trig->GetSin( angle )*radius;
	}
}

Coordinate Coordinate::RotateBy( float angle ) {
	int newangle = TO_INT( normalizeAngle(angle + GetAngle()) );
	RotateTo( newangle );
	return *this;
}

Coordinate Coordinate::RotateTo( float newangle ) {
	Trig *trig = Trig::Instance();
	int angle = TO_INT( newangle );
	float radius = GetMagnitude();
	m_x = trig->GetCos( angle )*radius;
	m_y = -trig->GetSin( angle )*radius;
	return *this;
}

