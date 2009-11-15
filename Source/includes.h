#ifndef __H_INCLUDES__
#define __H_INCLUDES__

#ifdef _MSC_VER
// The Microsoft GL header files require windows.h to be included first.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Microsoft's math.h requires this define so that it defines M_PI etc.
#define _USE_MATH_DEFINES

#endif // _MSC_VER

// System includes
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <time.h>
#include <assert.h>
#include <queue>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// Enhancements to STL
#include "Utilities/string_convert.h"

// Library includes
#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_image.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <zlib.h>

#include "GL/gl.h"
#include "GL/glu.h"

// Local includes
#include "defines.h"
#include "version.h"

#include <algorithm>

using namespace std;

#endif // __H_INCLUDES__