/*
 * Filename      : font.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         : Code taken from NeHe's OpenGL tutorial, lesson #43. No license could be found and is believed to be
 *                 free for use in all purposes without requiments.
 */

#include "Graphics/font.h"
#include "includes.h"
#include "Utilities/log.h"

Font::Font() {
	filename = NULL;

	r = 1.0f;
	g = 1.0f;
	b = 1.0f;
}

Font::Font( char *filename, float h ) {
	this->filename = NULL;

	r = 1.0f;
	g = 1.0f;
	b = 1.0f;

	SetFont( filename, h );
}

Font::~Font() {
	glDeleteLists(list_base, 128);
	glDeleteTextures(128, textures);
	delete [] textures;

	//Log::Message( "Font '%s' freed.", filename );

	free( filename );
}

// Generates OpenGL display lists from FreeType glyph information
bool Font::SetFont( char *filename, float h ) {

	this->filename = strdup( filename );

	//Allocate some memory to store the texture ids.
	textures = new GLuint[128];

	this->h=h;

	//Create and initilize a freetype font library.
	FT_Library library;
	if (FT_Init_FreeType( &library )) {
		Log::Error("FT_Init_FreeType failed");
		return(false);
	}

	//The object in which Freetype holds information on a given
	//font is called a "face".
	FT_Face face;

	//This is where we load in the font information from the file.
	//Of all the places where the code might die, this is the most likely,
	//as FT_New_Face will die if the font file does not exist or is somehow broken.
	if (FT_New_Face( library, filename, 0, &face )) {
		Log::Error("FT_New_Face failed (there is probably a problem with your font file)");
		return(false);
	}

	//For some twisted reason, Freetype measures font size
	//in terms of 1/64ths of pixels.  Thus, to make a font
	//h pixels high, we need to request a size of h*64.
	//(h << 6 is just a prettier way of writting h*64)
	FT_Set_Char_Size( face, h * 64, h * 64, 96, 96);

	//Here we ask opengl to allocate resources for
	//all the textures and displays lists which we
	//are about to create.  
	list_base = glGenLists(128);
	glGenTextures( 128, textures );

	//This is where we actually create each of the fonts display lists.
	for(unsigned char i=0;i<128;i++)
		make_dlist(face,i,list_base,textures);

	//We don't need the face information now that the display
	//lists have been created, so we free the assosiated resources.
	FT_Done_Face(face);

	//Ditto for the library.
	FT_Done_FreeType(library);

	return(true);
}

void Font::Render( int x, int y, char *text ) {
	glColor3f( r, g, b );

	//glRasterPos2i( x, y );

	// set up screen coords
	glPushAttrib(GL_TRANSFORM_BIT);
	GLint	viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
	glPopAttrib();

	//afont_gl_render_text( font, text );

	GLuint font = list_base;
	float h = this->h /.63f;						//We make the height about 1.5* that of
	
	//char		text[256];								// Holds Our String
	//va_list		ap;										// Pointer To List Of Arguments

	//if (fmt == NULL)									// If There's No Text
	//	*text=0;											// Do Nothing

	//else {
	//va_start(ap, fmt);									// Parses The String For Variables
	 //   vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	//va_end(ap);											// Results Are Stored In Text
	//}


	//Here is some code to split the text that we have been
	//given into a set of lines.
	//This could be made much neater by using
	//a regular expression library such as the one avliable from
	//boost.org (I've only done it out by hand to avoid complicating
	//this tutorial with unnecessary library dependencies).
	const char *start_line=text;
	vector<string> lines;

	const char * c = text;;

	//for(const char *c=text;*c;c++) {
	for(;*c;c++) {
		if(*c=='\n') {
			string line;
			for(const char *n=start_line;n<c;n++) line.append(1,*n);
			lines.push_back(line);
			start_line=c+1;
		}
	}
	if(start_line) {
		string line;
		for(const char *n=start_line;n<c;n++) line.append(1,*n);
		lines.push_back(line);
	}

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);	
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	glListBase(font);

	float modelview_matrix[16];	
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	//This is where the text display actually happens.
	//For each line of text we reset the modelview matrix
	//so that the line's text will start in the correct position.
	//Notice that we need to reset the matrix, rather than just translating
	//down by h. This is because when each character is
	//draw it modifies the current matrix so that the next character
	//will be drawn immediatly after it.  
	for(unsigned int i=0;i<lines.size();i++) {
		

		glPushMatrix();
		glLoadIdentity();
		glTranslatef(x,y-h*i,0);
		glMultMatrixf(modelview_matrix);

	//  The commented out raster position stuff can be useful if you need to
	//  know the length of the text that you are creating.
	//  If you decide to use it make sure to also uncomment the glBitmap command
	//  in make_dlist().
	//	glRasterPos2f(0,0);
		glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
	//	float rpos[4];
	//	glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
	//	float len=x-rpos[0];

		glPopMatrix();

		

	}


	glPopAttrib();
	
	// undo screen coord damage
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

// Renders text centered squarely on (x,y), taking the bounding box into account
void Font::RenderCentered( int x, int y, char *text ) {
	//int w, h, base;

	// determine size of text
	//afont_size_text( font, text, &w, &h, &base );

	//Render( x - (w / 2), y + (h / 2) - base, text );
	Render(x, y, text);
}

void Font::SetColor( float r, float g, float b ) {
	this->r = r;
	this->g = g;
	this->b = b;
}

void Font::make_dlist(FT_Face face, char ch, GLuint list_base, GLuint *tex_base) {
	if(FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT)) {
		Log::Error("FT_Load_Glyph failed");
		return;
	}

	FT_Glyph glyph;
	if(FT_Get_Glyph(face->glyph, &glyph)) {
		Log::Error("FT_Get_Glyph failed");
		return;
	}

	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	FT_Bitmap& bitmap = bitmap_glyph->bitmap;

	int width = next_p2( bitmap.width );
	int height = next_p2( bitmap.rows );

	GLubyte *expanded_data = new GLubyte[2 * width * height];

	for(int j = 0; j < height; j++) {
		for(int i = 0; i < width; i++) {
			expanded_data[2 * (i + j * width)] = expanded_data[2 * (i + j * width) + 1] = (i >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[i + bitmap.width * j];
		}
	}

	glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);

	delete [] expanded_data;

	// now create display list
	glNewList(list_base + ch, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, tex_base[ch]);

	glTranslatef(bitmap_glyph->left, 0, 0);

	glPushMatrix();
	glTranslatef(0, bitmap_glyph->top-bitmap.rows, 0);

	float x = (float)bitmap.width / (float)width, y = (float)bitmap.rows / (float)height;

	glBegin(GL_QUADS);
	glTexCoord2d(0,0); glVertex2f(0,bitmap.rows);
	glTexCoord2d(0,y); glVertex2f(0,0);
	glTexCoord2d(x,y); glVertex2f(bitmap.width,0);
	glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
	glEnd();
	glPopMatrix();
	glTranslatef(face->glyph->advance.x >> 6 ,0,0);

	glEndList();
}

int Font::next_p2(int a) {
	int rval = 1;
	while(rval < a) rval <<= 1;
	return rval;
}

