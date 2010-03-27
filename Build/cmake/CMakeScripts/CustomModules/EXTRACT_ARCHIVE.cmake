# function for extracting an archive
# This function requires the PARSE_ARGUMENTS macro
# Required Variable to be defined
#	1. G_UTIL_PATH - path containing tar/gzip
# Required Arguments
#	1. Archive
#	2. Output path
# Optional Arguments
#	NA
# Options
#	1. REQUIRED - define this to halt CMAKE progress on failed extraction

function(EXTRACT_ARCHIVE)
	PARSE_ARGUMENTS(EX "" "REQUIRED" ${ARGV})
	list(GET EX_DEFAULT_ARGS 0 ARC_PATH)
	list(GET EX_DEFAULT_ARGS 1 ARC_OUT)

	# Look for tar
	find_program(EX_TAR_PATH tar "${G_UTIL_PATH}")
	mark_as_advanced(EX_TAR_PATH)
	# Look for gzip
	find_program(EX_GZ_PATH gzip "${G_UTIL_PATH}")
	mark_as_advanced(EX_GZ_PATH)

	# If all conditions satisfied, perform extraction
	if (EXISTS "${ARC_PATH}")
		if (EX_TAR_PATH AND EX_GZ_PATH)
			message(STATUS "Extracting to ${ARC_OUT}")
			execute_process(COMMAND "${EX_GZ_PATH}" -cdv "${ARC_PATH}"
							COMMAND "${EX_TAR_PATH}" -x
							WORKING_DIRECTORY "${ARC_OUT}"
							RESULT_VARIABLE EX_RETURN_CODE)
		else (EX_TAR_PATH AND EX_GZ_PATH)
			message(WARNING "Either tar or gzip was not found in path.")
			set(EX_RETURN_CODE -1)
		endif(EX_TAR_PATH AND EX_GZ_PATH)
	else(EXISTS "${ARC_PATH}")
		message(WARNING "The archive does not exist: ${ARC_PATH}")
	endif(EXISTS "${ARC_PATH}")

	# Check if error occurred
	if (EX_RETURN_CODE)
		if (EX_REQUIRED)
			message(FATAL_ERROR "There was an error in the extraction to ${ARC_OUT}")
		else (EX_REQUIRED)
			message(WARNING "There was an error in the extraction to ${ARC_OUT}")
		endif (EX_REQUIRED)
	endif (EX_RETURN_CODE)
endfunction(EXTRACT_ARCHIVE)


# vim:ft=cmake
