/**\file			ui_dialogs.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Thusday, May 5, 2011
 * \date			Modified: Thursday, May 5, 2011
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Dialogs
 * \brief Common UI dialogs, e.g. "Yes/No", "Ok". */

void CancelConfirmDialog( void* value ) {
	*(int *)value = 0;
	UI::ReleaseModality();
}
void OKConfirmDialog( void* value ) {
	*(int *)value = 1;
	UI::ReleaseModality();
}
void OKAlertDialog( void* value ) {
	UI::ReleaseModality();
}

/**\brief Presents a question with "Ok/Cancel" buttons.
 * \return Returns true on "Ok", false otherwise
 */
bool Dialogs::Confirm( const char *message )
{
	Window* win = new Window(325, 265, 325, 130, "Confirm");
	static int value = 0;

	win->AddChild( ( new Label( 45, 35, message ) ) )
	->AddChild( (new Button( 65, 90, 80, 30, "Cancel", CancelConfirmDialog, &value ) ) )
	->AddChild( (new Button( 190, 90, 80, 30, "OK", OKConfirmDialog, &value ) ) );

	UI::ModalDialog( win );

	return value;
}

/**\brief Presents a message with a single "Ok" button.
 */
void Dialogs::Alert( const char *message )
{
	Window* win = new Window(325, 265, 325, 130, "Alert");

	win->AddChild( ( new Label( 45, 35, message ) ) )
	->AddChild( (new Button( 130, 90, 80, 30, "OK", OKAlertDialog, NULL ) ) );

	UI::ModalDialog( win );
}

/** @} */
