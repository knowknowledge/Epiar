/**\file			ui_common.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Thursday, May 5, 2011
 * \date			Modified: Thursday, May 5, 2011
 * \brief
 * \details
 */

#ifndef __H_DIALOGS__
#define __H_DIALOGS__

#include "Graphics/image.h"
#include "UI/ui.h"

class Dialogs {
	public:
		static bool Confirm( const char *message );
		static void Alert( const char *message );
		static void Options();
};

#endif // __H_DIALOGS__
