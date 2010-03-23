/**\file			trig.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
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

// Turns an arbitrary angle into the smallest equivalent angle
inline float normalizeAngle(float angle){
	angle -= floor(angle / 360.0f) * 360.0f; // Normalize to 0 to 360
	if(angle>180.0f) return angle -360.0f; // Normalize to -180 to 180
	return angle;
}

#endif // __h_trig__
