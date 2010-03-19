/**\file		includes.h
 * \author		Chris Thielen
 * \author		and others
 * \date		Saturday, November 21, 2009
 * \date		Saturday, November 21, 2009
 * \brief		Contains common system libraries.
 * \details
 */

#ifndef __H_INCLUDES__
#define __H_INCLUDES__

#ifdef _MSC_VER
// The Microsoft GL header files require windows.h to be included first.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define snprintf _snprintf
// Microsoft's math.h requires this define so that it defines M_PI etc.
#define _USE_MATH_DEFINES

#endif // _MSC_VER

// System includes
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <time.h>
#include <assert.h>
#include <queue>
#include <errno.h>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Enhancements to STL
#include "Utilities/string_convert.h"

// Library includes
#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <zlib.h>

#if __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/glext.h"
#endif

// Local includes
#include "defines.h"
#include "version.h"

#include <algorithm>

using namespace std;

#endif // __H_INCLUDES__
