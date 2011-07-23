/**\file			font.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include <FTGL/ftgl.h>
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "Utilities/log.h"
#include "Utilities/file.h"

/**\class Font
 * \brief Font class takes care of initializing fonts. */

/**\brief Constructs new font (default color white).
 */
Font::Font():r(1.f),g(1.f),b(1.f),a(1.f),font(NULL) {}

/**\brief Construct new font based on file.
 * \param filename String containing file.
 */
Font::Font( string filename ):r(1.f),g(1.f),b(1.f),a(1.f),font(NULL) {
	bool success;
	success = Load( filename );
	assert( success );
}

/**\brief Lazy fetch an Font
 */
Font* Font::Get( string filename ) {
	Font* value;
	value = static_cast<Font*>(Resource::Get(filename));
	if( value == NULL ) {
		value = new Font();
		if(value->Load(filename)){
			Resource::Store(filename,(Resource*)value);
		} else {
			LogMsg(DEBUG1,"Couldn't Find Font '%s'",filename.c_str());
			delete value;
			return NULL;
		}
	}
	return value;
}

/**\brief Lazy fetch an Font
 */
Font* Font::GetSkin( string skinPath ) {
	string path = SKIN( skinPath );
	if( path == "" ) {
		LogMsg(DEBUG1,"Couldn't Find Font '%s'",skinPath.c_str());
		assert(0);
	}
	return Get( path );
}

/**\brief Destroys the font.*/
Font::~Font() {
	delete (FTTextureFont*)this->font;
	LogMsg(INFO, "Font '%s' freed.", fontname.c_str() );
}

/**\brief Sets the new color and alpha value.
 */
void Font::SetColor( Color c, float a ) {
	this->r = c.r;
	this->g = c.g;
	this->b = c.b;
	this->a = a;
}

/**\brief Sets the new color and alpha value.
 */
void Font::SetColor( float r, float g, float b, float a ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

/**\brief Loads the font (uses FTGL Texture Fonts).
 * \param filename Path to font file.
 */
bool Font::Load( string filename ) {
	File fontFile;
	if( fontFile.OpenRead( filename.c_str() ) == false) {
		LogMsg(ERR, "Font '%s' could not be loaded.", fontname.c_str() );
		return( false );
	}

	if( this->font != NULL) {
		LogMsg(ERR, "Deleting the old font '%s'.\n", fontname.c_str() );
		delete this->font;
	}

	fontname = fontFile.GetAbsolutePath();
	this->font = new FTTextureFont( fontname.c_str() );

	if( font == NULL ) {
		LogMsg(ERR, "Failed to load font '%s'.\n", fontname.c_str() );
		return( false );
	}

	font->FaceSize(12);

	LogMsg(INFO, "Font '%s' loaded.\n", fontname.c_str() );

	return( true );
}

/**\brief Set's the size of the font (default is 12).*/
void Font::SetSize( int size ){
	this->font->FaceSize( size );
}

/**\brief Retrieves the size of the font.*/
unsigned int Font::GetSize( void ){
	return this->font->FaceSize();
}

/**\brief Returns the width of the text (no padding).*/
int Font::TextWidth( const string& text ) {
	return TO_INT(this->font->Advance(text.c_str()));
}

/**\brief Returns the recommended line height of the font.
 * \details
 * It is recommended that you use the line height for rendering lines
 * of text as it inserts a natural padding between lines.
 */
int Font::LineHeight( void ){
	return TO_INT(ceil(this->font->LineHeight()));
}

/**\brief Returns the complete height of the font, including Ascend and Descend.
 * \details
 * Use Outer height to get the tight fitting height of the font.
 */
int Font::TightHeight( void ){
	float asc = this->font->Ascender();
	float dsc = this->font->Descender();
	int height = TO_INT(ceil(asc-dsc));

	return height;
}

/**\brief Renders a string with natural padding.
 * \return The consumed width (This includes a small bit of padding on the right)
 */
int Font::Render( int x, int y, const string& text,XPos xpos, YPos ypos ){
	int h= this->LineHeight();
	return this->RenderInternal(x,y,text,h,xpos,ypos);
}

/**\brief Renders a string with no padding.
 * \return The consumed width (This includes a small bit of padding on the right)
 */
int Font::RenderTight(int x, int y, const string& text,XPos xpos, YPos ypos ){
	int h = this->TightHeight();
	return this->RenderInternal(x,y,text,h,xpos,ypos);
}

/**\brief Renders a string wrapped to a given width.
 * \return The number of lines used ( multiply by LineHeight to get total height).
 */
int Font::RenderWrapped( int x, int y, const string& text, int w ){
	LogMsg(ERR, "This isn't implemented yet!!!");
	assert(0);
	return 0;
}

/**\brief Internal rendering function.*/
int Font::RenderInternal( int x, int y, const string& text, int h, XPos xpos, YPos ypos) {
	int xn = 0;
	int yn = 0;

	switch( xpos ) {
		case LEFT:
			xn = x;
			break;
		case CENTER:
			xn = x - this->TextWidth(text) / 2;
			break;
		case RIGHT:
			xn=x-this->TextWidth(text);
			break;
		default:
			LogMsg(ERR, "Invalid xpos");
			assert(0);
	}

	// Y coordinates are flipped
	switch( ypos ) {
		case TOP:
			yn = -y - h - TO_INT(floor(this->font->Descender()));
			break;
		case MIDDLE:
			yn = -y - h / 2 - TO_INT(floor(this->font->Descender()));
			break;
		case BOTTOM:
			yn = -y - TO_INT(floor(this->font->Descender()));
			break;
		default:
			LogMsg(ERR, "Invalid ypos");
			assert(0);
	}

	glColor4f( r, g, b, a );
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix(); // to save the current matrix
	glScalef(1, -1, 1);
	FTPoint newpoint = this->font->Render( text.c_str(), -1, FTPoint( xn, yn, 1) );
	glPopMatrix(); // restore the previous matrix
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	return TO_INT(ceil(newpoint.Xf())) - x;
}

