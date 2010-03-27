# LibXml2 CMake build file
cmake_minimum_required(VERSION 2.6)
project (LibXml2)

if (NOT LibXml2_SOURCE_NAME)
	set(LibXml2_SOURCE_NAME libxml2-2.7.6)
endif (NOT LibXml2_SOURCE_NAME)

set(LIBXML_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${LibXml2_SOURCE_NAME}")
# Create variables for include and library
set(LIBXML2_INCLUDE_DIR ${LIBXML_SRC_FOLDER}/include CACHE STRING
	"LibXml2 include directory" FORCE)
set(LIBXML2_LIBRARIES libxml2 CACHE STRING
	"LibXml2 libraries" FORCE)
set(libxml_src
${LIBXML_SRC_FOLDER}/c14n.c
${LIBXML_SRC_FOLDER}/catalog.c
${LIBXML_SRC_FOLDER}/chvalid.c
${LIBXML_SRC_FOLDER}/debugXML.c
${LIBXML_SRC_FOLDER}/dict.c
${LIBXML_SRC_FOLDER}/DOCBparser.c
${LIBXML_SRC_FOLDER}/encoding.c
${LIBXML_SRC_FOLDER}/entities.c
${LIBXML_SRC_FOLDER}/error.c
${LIBXML_SRC_FOLDER}/globals.c
${LIBXML_SRC_FOLDER}/hash.c
${LIBXML_SRC_FOLDER}/HTMLparser.c
${LIBXML_SRC_FOLDER}/HTMLtree.c
${LIBXML_SRC_FOLDER}/legacy.c
${LIBXML_SRC_FOLDER}/list.c
${LIBXML_SRC_FOLDER}/nanoftp.c
${LIBXML_SRC_FOLDER}/nanohttp.c
${LIBXML_SRC_FOLDER}/parser.c
${LIBXML_SRC_FOLDER}/parserInternals.c
${LIBXML_SRC_FOLDER}/pattern.c
${LIBXML_SRC_FOLDER}/relaxng.c
${LIBXML_SRC_FOLDER}/SAX2.c
${LIBXML_SRC_FOLDER}/SAX.c
${LIBXML_SRC_FOLDER}/schematron.c
${LIBXML_SRC_FOLDER}/threads.c
${LIBXML_SRC_FOLDER}/tree.c
${LIBXML_SRC_FOLDER}/uri.c
${LIBXML_SRC_FOLDER}/valid.c
${LIBXML_SRC_FOLDER}/xinclude.c
${LIBXML_SRC_FOLDER}/xlink.c
${LIBXML_SRC_FOLDER}/xmlIO.c
${LIBXML_SRC_FOLDER}/xmlmemory.c
${LIBXML_SRC_FOLDER}/xmlreader.c
${LIBXML_SRC_FOLDER}/xmlregexp.c
${LIBXML_SRC_FOLDER}/xmlmodule.c
${LIBXML_SRC_FOLDER}/xmlsave.c
${LIBXML_SRC_FOLDER}/xmlschemas.c
${LIBXML_SRC_FOLDER}/xmlschemastypes.c
${LIBXML_SRC_FOLDER}/xmlunicode.c
${LIBXML_SRC_FOLDER}/xmlwriter.c
${LIBXML_SRC_FOLDER}/xpath.c
${LIBXML_SRC_FOLDER}/xpointer.c
${LIBXML_SRC_FOLDER}/xmlstring.c
)

option(LIBXML2_CONFIG_COMPILER "Reconfigure libxml2?" true)
# Use the builtin .js script to configure build
if (LIBXML2_CONFIG_COMPILER)
	message(STATUS "Configuring LibXml2...")
	if (WIN32)
		if (MSVC)
			set(libxml_compiler msvc)
		elseif (BORLAND)
			set(libxml_compiler bcb)
		else (MSVC)
			set(libxml_compiler mingw)
		endif (MSVC)
		execute_process(
			COMMAND cscript.exe configure.js iconv=no compiler=${libxml_compiler}
			WORKING_DIRECTORY "${LIBXML_SRC_FOLDER}/win32"
			OUTPUT_VARIABLE libxml_configure_output
			ERROR_VARIABLE libxml_configure_output)
		message(STATUS "${libxml_configure_output}")
	endif (WIN32)
	message(STATUS "Configuration complete...")
	set(LIBXML2_CONFIG_COMPILER false CACHE BOOL "Reconfigure libxml2?" FORCE)
endif (LIBXML2_CONFIG_COMPILER)

include_directories(${LIBXML2_INCLUDE_DIR})

# Creates the project
add_library(${LIBXML2_LIBRARIES} STATIC ${libxml_src})
if (NOT LIBXML2_STATICLIB_OUTPUT_PATH)
	set(LIBXML2_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"LibXml2 static lib output directory")
endif (NOT LIBXML2_STATICLIB_OUTPUT_PATH)
set_target_properties (${LIBXML2_LIBRARIES} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${LIBXML2_STATICLIB_OUTPUT_PATH}")

if (WIN32)
	target_link_libraries(${LIBXML2_LIBRARIES} ws2_32)
endif (WIN32)

# Some defines needed
if (MSVC)
	set(libxml_compile_def
		_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
endif (MSVC)
set_target_properties(${LIBXML2_LIBRARIES} PROPERTIES
	COMPILE_DEFINITIONS "${libxml_compile_def}")
# vim:ft=cmake
