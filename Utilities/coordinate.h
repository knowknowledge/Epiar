/*
 * Filename      : coordinate.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_coordinate__
#define __h_coordinate__

#include "includes.h"
#include "vector.h"

class Coordinate {
	public:
		Coordinate();
		Coordinate( double x, double y );
		Coordinate( double top, double right, double bottom, double left );
	
		// Sets bounadry caps on coordinates - used for momentum capping as well
		// Warning, boundaries are not enforced in subtraction and addition, but
		// are enforced on += and -=
		void SetBoundaries( double top, double right, double bottom, double left );
		bool ViolatesBoundary();
	
		~Coordinate();
	
		double  GetX () const;
		double  GetY () const;
		void  SetX ( double x );
		void  SetY ( double y );
	
		/* Returns coords converted to screen universe by Camera class */
	 	int GetScreenX();
	 	int GetScreenY();

		float GetAngle();
	
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
	
		Coordinate operator+=(Coordinate a){
			m_x += a.m_x;
			m_y += a.m_y;
			
			EnforceBoundaries();

			return *this;
		}//end operator+=
	
		Coordinate operator-=(Coordinate a){
			m_x -= a.m_x;
			m_y -= a.m_y;
			
			EnforceBoundaries();
			
			return *this;
		}//end operator-=
	
		Coordinate operator*=(Coordinate a) {
			m_x *= a.m_x;
			m_y *= a.m_y;
	
			EnforceBoundaries();
	
			return *this;
		}// end opterator*=
	
		Coordinate operator-(Coordinate a) {
			return Coordinate(m_x-a.m_x,m_y-a.m_y);
		}
	
		Coordinate operator+(Coordinate a){
			return Coordinate(m_x+a.m_x,m_y+a.m_y);
		}
	
		Coordinate operator+=(Vector b) {
			m_x+=b.ipart();
			m_y+=b.jpart();
			
			EnforceBoundaries();
			
			return *this;
		}
	
		Coordinate operator+(Vector b){
			return Coordinate(m_x+b.ipart(),m_y+b.jpart());
		}
		
		Coordinate operator-=(Vector b) {
			m_x-=b.ipart();
			m_y-=b.jpart();
			
			EnforceBoundaries();
			
			return *this;
		}
	
		Coordinate operator-(Vector b){
			return Coordinate(m_x-b.ipart(),m_y-b.jpart());
		}//end operator- (Vector conversion)
	
		friend ostream& operator<<(ostream & out, const Coordinate &c );
	
	//// end math operators
	
	//// friend functions
	
		// VelocityAdjCoordinate(&cCoordiante,Vector, double)
	        // Will adjust the cCoodiante, for a given velocity (cVecotr) over some
	        // length of time (double).
		// (this code was moved out of the vector.cpp)
		// inputs:
		//	cCoordiante	The cCoordiante object to be adjusted
		//	Vector		The velocity vector
		//	double		The time the object has past
		//			since the last position update.
	
		friend void VelocityAdjCoordinate(Coordinate&,Vector, double);
	
	
	//The rest is Matt's problem :-D
	/////////////////////////////////////////////
	
	private:
	 	// Ensures coordinates are within boundaries, if set
	 	void EnforceBoundaries( void );
	 
		double  m_x;
		double  m_y;
		
		// used to set bounds on coordinates
		struct {
			double top, right, bottom, left;
		} boundary;
};

#endif // __h_coordinates__
