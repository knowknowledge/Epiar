/**\file			hashtbl.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Friday, June 9, 2006
 * \date			Modified: Friday, June 9, 2006
 * \brief			Implementation of a quadratic probing hash table
 * \details
 * Mark Allen Weiss' quadratic probing hash table was used as a reference.
 */

#include "includes.h"
#include "hashtbl.h"

// This algorithm could be more efficient.
bool isPrime( int n ) {
	if( n == 2 || n == 3 )
		return true;
	
	if( n == 1 || n % 2 == 0 )
		return false;
	
	for( int i = 3; i * i <= n; i+= 2 )
		if( n % i == 0 )
			return false;
	
	return true;
}

// Assumes n > 0.
int nextPrime( int n ) {
	if( n % 2 == 0 )
		n++;
	
	for( ; !isPrime( n ); n+= 2 )
		;
	
	return n;
}

//int hash( const string & key ) {
	//int hashVal = 0;
	
	//for( int i = 0; i < (signed)key.length(); i++ )
		//hashVal = 37 * hashVal + key[i];
	
	//return hashVal;
//}

//int hash( int key ) {
	//return key;
//}
