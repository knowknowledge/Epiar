# FTGL CMake build file
cmake_minimum_required(VERSION 2.6)
project (FTGL)

if (NOT FTGL_SOURCE_NAME)
	set(FTGL_SOURCE_NAME ftgl-2.1.3~rc5)
endif (NOT FTGL_SOURCE_NAME)

set(FTGL_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${FTGL_SOURCE_NAME}/src")
# Create variables for include and library
set(FTGL_INCLUDE_DIR ${FTGL_SRC_FOLDER} CACHE STRING
	"FTGL include directory" FORCE)
set(FTGL_LIBRARIES FTGL CACHE STRING
	"FTGL libraries" FORCE)
set(FTGL_src
	${FTGL_SRC_FOLDER}/FTBuffer.cpp
	${FTGL_SRC_FOLDER}/FTCharmap.cpp
	${FTGL_SRC_FOLDER}/FTContour.cpp
	${FTGL_SRC_FOLDER}/FTFace.cpp
	${FTGL_SRC_FOLDER}/FTGlyphContainer.cpp
	${FTGL_SRC_FOLDER}/FTLibrary.cpp
	${FTGL_SRC_FOLDER}/FTPoint.cpp
	${FTGL_SRC_FOLDER}/FTSize.cpp
	${FTGL_SRC_FOLDER}/FTVectoriser.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTBitmapFont.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTBufferFont.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTExtrudeFont.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTFont.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTFontGlue.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTOutlineFont.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTPixmapFont.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTPolygonFont.cpp
	${FTGL_SRC_FOLDER}/FTFont/FTTextureFont.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTBitmapGlyph.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTBufferGlyph.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTExtrudeGlyph.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTGlyph.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTGlyphGlue.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTOutlineGlyph.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTPixmapGlyph.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTPolygonGlyph.cpp
	${FTGL_SRC_FOLDER}/FTGlyph/FTTextureGlyph.cpp
	${FTGL_SRC_FOLDER}/FTLayout/FTLayout.cpp
	${FTGL_SRC_FOLDER}/FTLayout/FTLayoutGlue.cpp
	${FTGL_SRC_FOLDER}/FTLayout/FTSimpleLayout.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/${FTGL_SOURCE_NAME}/msvc)
include_directories(${FTGL_INCLUDE_DIR})
include_directories(${FREETYPE_INCLUDE_DIRS})

add_library(${FTGL_LIBRARIES} STATIC ${FTGL_src})
set_target_properties(${FTGL_LIBRARIES} PROPERTIES
	COMPILE_DEFINITIONS "WIN32;_LIB;FTGL_LIBRARY_STATIC")
if (NOT FTGL_STATICLIB_OUTPUT_PATH)
	set(FTGL_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"FTGL static lib output directory")
endif (NOT FTGL_STATICLIB_OUTPUT_PATH)
set_target_properties(${FTGL_LIBRARIES} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${FTGL_STATICLIB_OUTPUT_PATH}")

target_link_libraries(${FTGL_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${OPENGL_gl_LIBRARY}
	${OPENGL_glu_LIBRARY}
	)

