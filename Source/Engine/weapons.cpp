/**\file			weapons.cpp
 * \author			Shawn Reynolds (eb0s@yahoo.com)
 * \date			Created: Friday, November 21, 2009
 * \date			Modified: Friday, November 21, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/weapons.h"
#include "Utilities/log.h"
#include "Utilities/parser.h"
#include "Utilities/components.h"

Weapons *Weapons::pInstance = 0; // initialize pointer

/**\brief Gets or returns Weapons instance
 * \return Pointer to a Weapons object
 */
Weapons *Weapons::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Weapons; // create the sold instance
		pInstance->rootName = "weapons";
		pInstance->componentName = "weapon";
	}
	return( pInstance );
}

