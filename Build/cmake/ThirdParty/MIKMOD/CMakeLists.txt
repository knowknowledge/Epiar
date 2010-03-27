# MIKMOD CMake build file
cmake_minimum_required(VERSION 2.6)
project(MIKMOD)

if (NOT MIKMOD_SOURCE_NAME)
	set(MIKMOD_SOURCE_NAME libmikmod-3.1.12)
endif (NOT MIKMOD_SOURCE_NAME)

set(MIKMOD_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${MIKMOD_SOURCE_NAME}")
# Create variables for include and library
set(MIKMOD_INCLUDE_DIR ${MIKMOD_SRC_FOLDER}/include CACHE STRING
	"MIKMOD include directory" FORCE)
set(MIKMOD_LIBRARIES MIKMOD CACHE STRING
	"MIKMOD libraries" FORCE)
set(MIKMOD_src
	${MIKMOD_SRC_FOLDER}/drivers/drv_ds.c
	${MIKMOD_SRC_FOLDER}/drivers/drv_nos.c
	${MIKMOD_SRC_FOLDER}/drivers/drv_raw.c
	${MIKMOD_SRC_FOLDER}/drivers/drv_stdout.c
	${MIKMOD_SRC_FOLDER}/drivers/drv_wav.c
	${MIKMOD_SRC_FOLDER}/drivers/drv_win.c
	${MIKMOD_SRC_FOLDER}/loaders/load_669.c
	${MIKMOD_SRC_FOLDER}/loaders/load_amf.c
	${MIKMOD_SRC_FOLDER}/loaders/load_dsm.c
	${MIKMOD_SRC_FOLDER}/loaders/load_far.c
	${MIKMOD_SRC_FOLDER}/loaders/load_gdm.c
	${MIKMOD_SRC_FOLDER}/loaders/load_it.c
	${MIKMOD_SRC_FOLDER}/loaders/load_imf.c
	${MIKMOD_SRC_FOLDER}/loaders/load_m15.c
	${MIKMOD_SRC_FOLDER}/loaders/load_med.c
	${MIKMOD_SRC_FOLDER}/loaders/load_mod.c
	${MIKMOD_SRC_FOLDER}/loaders/load_mtm.c
	${MIKMOD_SRC_FOLDER}/loaders/load_okt.c
	${MIKMOD_SRC_FOLDER}/loaders/load_s3m.c
	${MIKMOD_SRC_FOLDER}/loaders/load_stm.c
	${MIKMOD_SRC_FOLDER}/loaders/load_stx.c
	${MIKMOD_SRC_FOLDER}/loaders/load_ult.c
	${MIKMOD_SRC_FOLDER}/loaders/load_uni.c
	${MIKMOD_SRC_FOLDER}/loaders/load_xm.c
	${MIKMOD_SRC_FOLDER}/mmio/mmalloc.c
	${MIKMOD_SRC_FOLDER}/mmio/mmerror.c
	${MIKMOD_SRC_FOLDER}/mmio/mmio.c
	${MIKMOD_SRC_FOLDER}/playercode/mdriver.c
	${MIKMOD_SRC_FOLDER}/playercode/mdreg.c
	${MIKMOD_SRC_FOLDER}/playercode/mloader.c
	${MIKMOD_SRC_FOLDER}/playercode/mlreg.c
	${MIKMOD_SRC_FOLDER}/playercode/mlutil.c
	${MIKMOD_SRC_FOLDER}/playercode/mplayer.c
	${MIKMOD_SRC_FOLDER}/playercode/munitrk.c
	${MIKMOD_SRC_FOLDER}/playercode/mwav.c
	${MIKMOD_SRC_FOLDER}/playercode/npertab.c
	${MIKMOD_SRC_FOLDER}/playercode/sloader.c
	${MIKMOD_SRC_FOLDER}/playercode/virtch.c 
	${MIKMOD_SRC_FOLDER}/playercode/virtch_common.c
	${MIKMOD_SRC_FOLDER}/playercode/virtch2.c
	)

include_directories("${MIKMOD_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}")
add_library(${MIKMOD_LIBRARIES} STATIC ${MIKMOD_src})
# Need to disable VS internal malloc
set(MIKMOD_DEF "WIN32;DRV_WIN;HAVE_FCNTL_H;HAVE_MALLOC_H;HAVE_LIMITS_H;")
if (MSVC)
	set(MIKMOD_DEF ${MIKMOD_DEF}
		"DRV_DS;_INC_MALLOC;_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_WARNINGS")
	set(MIKMOD_FLAG "/wd4244 /wd4018")
endif (MSVC)
set_target_properties(${MIKMOD_LIBRARIES} PROPERTIES
	COMPILE_DEFINITIONS "${MIKMOD_DEF}"
	COMPILE_FLAGS "${MIKMOD_FLAG}")

if (NOT MIKMOD_STATICLIB_OUTPUT_PATH)
	set(MIKMOD_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"MIKMOD static lib output directory")
endif (NOT MIKMOD_STATICLIB_OUTPUT_PATH)
set_target_properties(${MIKMOD_LIBRARIES} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${MIKMOD_STATICLIB_OUTPUT_PATH}")

# Need to copy mikmod header
configure_file(${PROJECT_SOURCE_DIR}/mikmod_build.h ${MIKMOD_INCLUDE_DIR}/mikmod.h
	COPYONLY)

target_link_libraries(${MIKMOD_LIBRARIES}
	dsound
	)
