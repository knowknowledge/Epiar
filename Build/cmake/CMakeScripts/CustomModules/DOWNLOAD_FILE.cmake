# function for downloading files
# This function requires the PARSE_ARGUMENTS macro
# Required Variable to be defined
#	1. G_UTIL_PATH - path containing wget/curl (if using external downloader)
# Required Arguments
#	1. URL
#	2. Output path
# Optional Arguments
#	NA
# Options
#	1. REQUIRED - define this to halt CMAKE progress on failed download

function(DOWNLOAD_FILE)
	# Parse Arguments
	PARSE_ARGUMENTS(DLOPT "" "REQUIRED" ${ARGV})
	list(GET DLOPT_DEFAULT_ARGS 0 DL_URL)
	list(GET DLOPT_DEFAULT_ARGS 1 DL_OUT)

	# Look for wget
	find_program(DL_WGET_PATH wget "${G_UTIL_PATH}")
	mark_as_advanced(DL_WGET_PATH)
	# Look for curl
	find_program(DL_CURL_PATH curl "${G_UTIL_PATH}")
	mark_as_advanced(DL_CURL_PATH)

	# Delete file if it already exists
	if (EXISTS "${DL_OUT}")
		file(REMOVE "${DL_OUT}")
	endif(EXISTS "${DL_OUT}")

	if (DL_WGET_PATH)
		message(STATUS "Downloading with wget: ${DL_URL}...")
		execute_process(COMMAND "${DL_WGET_PATH}" -o "${DL_OUT}" "${DL_URL}")
	elseif (DL_CURL_PATH)
		message(STATUS "Downloading with curl: ${DL_URL}...")
		execute_process(COMMAND "${DL_CURL_PATH}" -L -o "${DL_OUT}" "${DL_URL}")
	else (DL_WGET_PATH)
		message(STATUS "Downloading: ${DL_URL}...")
		file(DOWNLOAD "${DL_URL}" "${DL_OUT}")
	endif (DL_WGET_PATH)

	if (NOT EXISTS "${DL_OUT}")
		if (DL_REQUIRED)
			message(FATAL_ERROR "Unable to download to ${DL_OUT}")
		else (DL_REQUIRED)
			message(WARNING "Unable to download to ${DL_OUT}")
		endif (DL_REQUIRED)
	endif (NOT EXISTS "${DL_OUT}")

endfunction(DOWNLOAD_FILE)

# vim:ft=cmake
