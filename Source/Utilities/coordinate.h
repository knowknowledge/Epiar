/**\file			coordinate.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */


#ifndef __h_coordinate__
#define __h_coordinate__

#include "includes.h"

class Coordinate {
	public:
		Coordinate();
		Coordinate( double x, double y );
	
		bool ViolatesBoundary( double top, double right, double bottom, double left );
	 	void EnforceBoundaries( double top, double right, double bottom, double left );
		void EnforceMagnitude( double radius);
		Coordinate RotateBy( float angle );
		Coordinate RotateTo( float angle );
	
		~Coordinate();
	
		double  GetX () const;
		double  GetY () const;
		void  SetX ( double x );
		void  SetY ( double y );
	
		/* Returns coords converted to screen universe by Camera class */
	 	int GetScreenX();
	 	int GetScreenY();

		float GetAngle();
		float GetMagnitude();
		inline float GetMagnitudeSquared() {
			return static_cast<float>(m_y*m_y + m_x*m_x );
		}
	
		// Make an SDL Rectangle from coordinates
		// Use width/height of 0
		SDL_Rect  getRect ();
		// Use given dimensions
		SDL_Rect  getRectWithDim ( int w, int h );
	
		Coordinate operator=(double a) {
			m_x = a;
			m_y = a;
	
			return *this;
		}// end operator=
	
		bool operator==(double a) {
			if( m_x == a )
				if( m_y == a )
					return( true );
	
			return( false );
		}//end opterator==

		const bool operator==(const Coordinate other) const {
			return ( m_x == other.m_x ) && ( m_x == other.m_x );
		}//end opterator==

		const bool operator<(const Coordinate other) const {
			if( m_x == other.m_x )
				return( m_y < other.m_y );
			return( m_x < other.m_x );
		}//end opterator<
	
		Coordinate operator+=(Coordinate a){
			m_x += a.m_x;
			m_y += a.m_y;
			return *this;
		}//end operator+=
	
		Coordinate operator-=(Coordinate a){
			m_x -= a.m_x;
			m_y -= a.m_y;
			return *this;
		}//end operator-=
	
		Coordinate operator*=(Coordinate a) {
			m_x *= a.m_x;
			m_y *= a.m_y;
			return *this;
		}// end opterator*=
	
		Coordinate operator-(Coordinate a) {
			return Coordinate(m_x-a.m_x,m_y-a.m_y);
		}
	
		Coordinate operator+(Coordinate a){
			return Coordinate(m_x+a.m_x,m_y+a.m_y);
		}
	
		Coordinate operator*(double r){
			return Coordinate(m_x*r,m_y*r);
		}
		Coordinate operator*=(double r){
			m_x*=r;
			m_y*=r;
			return *this;
		}
		Coordinate operator/(double r){
			return Coordinate(m_x/r,m_y/r);
		}
		Coordinate operator/=(double r){
			m_x/=r;
			m_y/=r;
			return *this;
		}
	
		friend ostream& operator<<(ostream & out, const Coordinate &c );
	
	//// end math operators
	
	private:
	 
		double  m_x;
		double  m_y;
};

#endif // __h_coordinates__
