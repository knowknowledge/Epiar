/**\file			vector.h
 * \author			Douglas Moyes (dbmoyes@gmail.com), Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h__vector__
#define __h__vector__

/*********************
Vector -- class for handling vector quantities.

Operator Overloading:

+,+=,-,-=,/,/=.*,*=. math operator are overloaded.
==.!= logical operator are overloaded.

The = operator is overloaded to only duplicate relevant information.

Friend functions:

Other class functions:

Rectangular notation (internally stored this way):

double Vector::ipart()	gets the i part of a vector (real)
double Vector::jpart()	gets the j part of a vector (imaginary)

Polar Notation:

double Vector::degangle()	gets the vector angle in degrees
double Vector::radangle()	gets the vector angle in radians
double Vector::magnitude()	get the magnitude of the vector

Constructor can be called in two forms:

Vector(double ipart, double jpart)
	where ipart and jpart are the i and j parts of a vector.


Vector(double magnitude, double angle, bool radians)
	magnitude is the magnitude of the vector, and angle is the
	angle of the vector (obviously). radians is true if the
	angle is in radians, false if it is in degrees.  For code
	readability V_DEG and V_RAD have been defined for
	the boolean values associated with using degrees or radians.

Vector(char *) FOR INTERNAL EXCEPTION HANDLING ONLY


The public string "what" stores a description of the exception.
Normally the pointer is null.
*/

#include "includes.h"

//the following is manly for debugging.

#define USEATAN2	//use atan2() rather than atan to determine angle.

// undefined when compiling when testing the math and  building the test
// program not tied to the Epiar CVS


#define V_RAD true
#define V_DEG false
#include <math.h>


// Some definitions for pi in case they aren't defined in math.h
// Since all of these definitions are specific to Vector, they are defined
// in the header file associated with the class, not some other header file.
// The form is given as v_xPIy where x is the numerator, and y
// is the denominator.  If a value for x or y is omitted, the value is
// one.

#ifndef M_PI
//hey, this might be running on a computer with unusually precise
//doubles.
#define V_PI   3.1415926535897932384626433832795028841971693993751058209
#define V_2PI  6.2831853071795864769252867665590057683943387987502116419
#else
#define V_PI M_PI
#define V_2PI (M_PI*2)
#endif


#ifndef M_PI_2
#define V_PI2  1.5707963267948966192313216916397514420985846996875529104
#define V_3PI2 4.7123889803846898576939650749192543262957540990626587314
#else
#define V_PI2 M_PI_2
#define V_3PI2 (M_PI_2*3)
#endif


#ifndef M_PI_4
#define V_PI4  .78539816339744830961566084581987572104929234984377645524
#else
#define V_PI4 M_PI_4
#endif

class Vector{

	double i,j; // i and j parts of vector in rectangular notation

	public:

	char *what; //used when exception is thrown.  Stores description
		    //of exception.

///////// Constructors ////////
	// Vector(double d)
	// Constructor that takes a scalar
	// inputs: d -- a scalar quantity
	Vector(double d){
		what=0;
		i=0; j=d;
	}// end constructor

	//Vector(char *s)
	//Used internally to assign the internal "what" string a vale
	//when Vector is thrown as an exception.
	//inputs: s -- a description of the exception
	Vector(const char *s){
		what=strdup(s);
		i=0; j=0;
	}//end constructor

	//Vector()
	//Default constructor
	//inputs: none
	Vector(){
		what=0;
		i=0; j=0;
	}// end constructor

	//Vector(double ip, double jp)
	//Constructor allows assignment of the vector in rectangular
	//form first the real part, followed by the imaginary part.
	Vector(const double ip,const double jp){
		what=0;
		i=ip; j=jp;
	}//end constructor

	//Vector(double m, double a, const bool r)
	//Constructor allows assignment of the vector in polar form.
	//inputs:
	//	m	magnitude of the vector
	//	a	angle of the vector in either radians or degrees
	//		as specified by r below
	//	r	true if angle a is given in radians, false if it
	//		is not. V_RAD and V_DEG should be used
	//		in place of true and false for readability.
	Vector(const double m, const double a, const bool r){
		double angle;

		what=0;
		angle=a;

		if(!r)angle*=V_PI/180; //convert to radians.
		j=m*sin(angle);
		i=m*cos(angle);
	}//end of polar constructor

	friend ostream& operator<<(ostream &out, const Vector &v);

	// All these functions are inlined for speed.

	// double Vector::ipart()
	// returns the value of the imaginary part of the vector
	double ipart(){return i;}

	// double Vector::jpart()
	// returns the real part of the vector
	double jpart(){return j;}

	// double Vector::radangle()
	// Polar form.  Returns the angle in radians.
	double radangle(); // too complex for inlining.

	// double Vector::degangle()
	// Polar form.  Returns the angle, in degrees, of the vector.
	double degangle(){return radangle()*180/V_PI;}

	// double Vector::magnitude()
	// Polar form.  Returns the the magnitude of the vector.
	double magnitude(){return sqrt(i*i + j*j);}

////////////////// Basic Math //////////////////
// Everyone should know what these operators do, so I see
// no real reason describe them :-)
/// binary operators ///

	Vector operator+(Vector a){ //addition
		register Vector ret;

		ret.i= i + a.i;
		ret.j= j + a.j;

		return ret;
	}//end operator+

	Vector operator+(double a){ //scalar addition
		return Vector(i+a,j);
	}//end operator+

	Vector operator-(Vector a){ //subtraction
		register Vector ret;

		ret.i= i - a.i;
		ret.j= j - a.j;

		return ret;
	}//end operator-

	Vector operator-(double a){ //scalar subtraction
		return Vector(i-a,j);
	}//end operator-


	Vector operator*(Vector a){ //multiplication
		double m; //magnitude
		double angle; //angle
		// convert the two vectors into polar form
		// do the math and return the result
		m=magnitude()*a.magnitude();
		angle=radangle()+a.radangle();


		return Vector(m*cos(angle),m*sin(angle));
	}//end operator*

	Vector operator*(double a){ //scalar multiplication
		return Vector(i*a,j*a);
	}

	Vector operator/(Vector a){ //division
		double m1,m2; //magnitudes
		double a1,a2; //angles
		// convert the two vectors into polar form
		m1=magnitude();
		m2=a.magnitude();
		a1=radangle();
		a2=a.radangle();

		//divide by zero exception
		if(m2==0)throw Vector("Divide by Zero");

		// do the math and return the result
		m1=m1/m2;
		a1=a1-a2;

		return Vector(m1*cos(a1),m1*sin(a1));
	}// end operator/

	Vector operator/(double a){ //scalar division
		if(a==0)throw Vector("Divide by Zero");
		return Vector(i/a,j/a);
	}
//// assignment operators ////

	Vector operator+=(Vector a){ //addition
		i+=a.i;
		j+=a.j;
		return *this;
	}//end operator+=

	Vector operator+=(double a){ //scalar addition
		i+=a;
		return *this;
	}//end operator+=

	Vector operator-=(Vector a){ //subtraction
		i-=a.i;
		j-=a.j;
		return *this;
	}//end operator-=

	Vector operator-=(double a){ //scalar subtraction
		i-=a;
		return *this;
	}//end operator-=

	Vector operator*=(Vector a){ //multiplication

		double m; //magnitude
		double angle; //angle
		// convert the two vectors into polar form
		m=magnitude()*a.magnitude();
		angle=radangle()+a.radangle();

		// do the math and return the result
		j=m*sin(angle);
		i=m*cos(angle);

		return *this;
	}//end operator*=

	Vector operator*=(double a){ //scalar multiplication
		i*=a;
		j*=a;
		return *this;
	}

	Vector operator/=(Vector a){ //division
		double m1,m2; //magnitudes
		double a1,a2; //angles
		// convert the two vectors into polar form
		m1=magnitude();
		m2=a.magnitude();
		a1=radangle();
		a2=a.radangle();

		//divide by zero exception
		if(m2==0)throw Vector("Divide by Zero");

		// do the math and return the result
		m1=m1/m2;
		a1=a1-a2;

		j=m1*sin(a1);
		i=m1*cos(a1);

		return *this;
	}// end operator/=

	Vector operator/=(double a){ //scalar division
		if(a==0)throw Vector("Divide by Zero");
		i/=a;
		j/=a;
		return *this;
	}

//// unary operators ////

//////////////// end of basic math ////////////////

/// Logical operators ///

	bool operator==(Vector a){
		if(i!=a.i)return false;
		if(j!=a.j)return false;
		return true;
	}// end operator==

	bool operator<(float a) {
		if (magnitude() < a)
			return true;
		return false;
	}

	bool operator>(float a) {
		if (magnitude() > a)
			return true;
		return false;
	}

	bool operator!=( Vector a){
		// we can do this because we overloaded this
		// operator above.
		return !(*this==a);
	}// end operator!=

//Copying and other operators.

	Vector operator=(Vector a){
		i=a.i; // only copy the stuff we really need.
		j=a.j;
		return *this;
	}// end operator=*/

	// this is a one-way conversion only!
};

#endif // __h_vector__
