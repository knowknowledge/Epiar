/*
 * Filename      : trig.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_trig__
#define __h_trig__

class Trig {
	public:
		static Trig *Instance();

		double DegToRad( int i );
		double DegToRad( double i );
		int RadToDeg( double i );

		double GetCos( int ang );
		double GetCos( double ang );
		double GetSin( int ang );
		double GetSin( double ang );

		void RotatePoint( float x, float y, float ax, float ay, float *nx, float *ny, float ang );

	protected:
		Trig();
		Trig( const Trig & );
  		Trig& operator= (const Trig&);

	private:
		static Trig *pInstance;

		double convdr; // conversion factor from degrees to radians
		double convrd; // conversion factor from radians to degrees

		double sinTable[360];
		double cosTable[360];
		double tanTable[360];
};

// Turns an arbitrary angle into an angle between -180 and 180 degrees
inline float normalizeAngle(float angle){
	float result = fmod(angle,540.0f);
	if(result>=180.0f)result-=360.0f;
	return result;
}

#endif // __h_trig__
