/**\file		defines.h
 * \author		Chris Thielen
 * \author		and others
 * \date		Saturday, November 21, 2009
 * \date		Saturday, November 21, 2009
 * \brief		Defines some common symbols
 * \details
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#ifndef SUCCESS
#define SUCCESS 0
#endif /*SUCCESS*/

#ifndef FAILURE
#define FAILURE 1
#endif /*SUCCESS*/

#ifndef u_byte
#define u_byte unsigned char
#endif // byte

#ifndef TO_BOOL
#define TO_BOOL(X) (X != 0)
#endif // TO_BOOL

#ifndef TO_FLOAT
#define TO_FLOAT(X) (static_cast<float>(X))
#endif // TO_FLOAT

#ifndef TO_INT
#define TO_INT(X) (static_cast<int>(X))
#endif // TO_INT

#endif /*DEFINES_H_*/
