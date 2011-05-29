/**\file		common.h
 * \author		Chris Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created:	Saturday, January 5, 2008
 * \date		Modified:	Thursday, November 19, 2009
 * \brief		Common variables and defines.
 * \details
 *	This file contains some global variables, defines,
 *	.
 */

#ifndef __H_COMMON__
#define __H_COMMON__

#include "Graphics/font.h"
#include "Utilities/xml.h"
#include "Utilities/options.h"

// FORPRODUCTION: switch this to 0
#define DEBUG 1

// defined in main.cpp. used through the codebase
extern XMLFile *skinfile;

extern Font *SansSerif;
extern Font *BitType;
extern Font *Serif;
extern Font *Mono;

#define SKIN(path) (skinfile->Get(path) )

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

#ifndef M_SQRT2
#define V_SQRT2  1.41421356237309504880168872420969807856967187537694807
#else
#define V_SQRT2 M_SQRT2
#endif

#endif // __H_COMMON__
