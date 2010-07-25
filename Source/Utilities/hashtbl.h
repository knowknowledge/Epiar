/**\file			hashtbl.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Friday, June 9, 2006
 * \date			Modified: Friday, June 9, 2006
 * \brief			Implementation of a quadratic probing hash table
 * \details
 * Mark Allen Weiss' quadratic probing hash table was used as a reference.
 */

#ifndef __H_HASH_TABLE__
#define __H_HASH_TABLE__

#include "includes.h"

int nextPrime( int n );

template <class HashedObj>
class HashTable {
	public:
		HashTable( int size = 101 ) : table( nextPrime( size ) ) {
			makeEmpty();
		}
		
		bool contains( HashedObj &x ) {
			int pos = findPos( x );
			
			if( isActive( pos ) ) {
				x = table[ pos ].element;
				return true;
			}
			
			return false;
		}
		
		void makeEmpty( void ) {
			currentSize = 0;
			for( int i = 0; i < (signed)table.size(); i++ ) {
				table[ i ].info = EMPTY;
			}
		}
		
		bool insert( HashedObj x ) {
			int currentPos = findPos( x );
			if( isActive( currentPos ) )
				return false;
			
			table[ currentPos ] = HashEntry( x, ACTIVE );
			
			if( ++currentSize > (signed)table.size() / 2 )
				rehash();
				
			return true;
		}
		
		bool remove( const HashedObj & x ) {
			int currentPos = findPos( x );
			if( !isActive( currentPos ) )
				return false;
			
			table[ currentPos ].info = DELETED;
			return true;
		}
		
		enum EntryType { ACTIVE, EMPTY, DELETED };
	
	private:
		struct HashEntry {
			HashedObj element;
			EntryType info;
			
			HashEntry( const HashedObj &e = HashedObj(), EntryType i = EMPTY )
				: element( e ), info( i ) {}
		};
		
		vector<HashEntry> table;
		int currentSize;
		
		bool isActive( int currentPos ) const
			{ return table[ currentPos ].info == ACTIVE; }
		
		int findPos( HashedObj x ) const {
			int offset = 1;
			int currentPos = myhash( x );
			
			while( table[ currentPos ].info != EMPTY && table[ currentPos ].element != x ) {
				currentPos += offset;
				offset += 2;
				if( currentPos >= (signed)table.size() )
					currentPos -= table.size();
			}
			
			return currentPos;
		}
		
		void rehash( void ) {
			vector<HashEntry> oldTable = table;
			
			table.resize( nextPrime( 2 * oldTable.size() ) );
			for( int j = 0; j < (signed)table.size(); j++ )
				table[ j ].info = EMPTY;
			
			// Copy table over
			currentSize = 0;
			for( int i = 0; i < (signed)oldTable.size(); i++ )
				if( oldTable[i].info == ACTIVE )
					insert( oldTable[i].element );
		}
		
		int myhash( HashedObj &x) const {
			int hashVal = x.hash();
			
			hashVal %= table.size();
			if( hashVal < 0 )
				hashVal += table.size();
				
			return hashVal;
		}
};

//int hash( const string &key );
//int hash( int key );

#endif // __H_HASH_TABLE__
