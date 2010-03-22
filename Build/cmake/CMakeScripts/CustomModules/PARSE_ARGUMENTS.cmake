# This macro parses a string of arguments.
#   Here is a simple, albeit impractical, example of using PARSE_ARGUMENTS that demonstrates its behavior. 
#   SET(arguments
#     hello OPTION3 world
#     LIST3 foo bar
#     OPTION2
#     LIST1 fuz baz
#     )
#   PARSE_ARGUMENTS(ARG "LIST1;LIST2;LIST3" "OPTION1;OPTION2;OPTION3" ${arguments})
#
#   PARSE_ARGUMENTS creates 7 variables and sets them as follows: 
#   ARG_DEFAULT_ARGS: hello;world 
#   ARG_LIST1: fuz;baz 
#   ARG_LIST2: 
#   ARG_LIST3: foo;bar 
#   ARG_OPTION1: FALSE 
#   ARG_OPTION2: TRUE 
#   ARG_OPTION3: TRUE 
#
#   If you don't have any options, use an empty string in its place. 
#   PARSE_ARGUMENTS(ARG "LIST1;LIST2;LIST3" "" ${arguments})
#
#   Likewise if you have no lists. 
#   PARSE_ARGUMENTS(ARG "" "OPTION1;OPTION2;OPTION3" ${arguments})

MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})
    SET(larg_names ${arg_names})
    LIST(FIND larg_names "${arg}" is_arg_name)
    IF (is_arg_name GREATER -1)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name GREATER -1)
      SET(loption_names ${option_names})
      LIST(FIND loption_names "${arg}" is_option)
      IF (is_option GREATER -1)
	     SET(${prefix}_${arg} TRUE)
      ELSE (is_option GREATER -1)
	     SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option GREATER -1)
    ENDIF (is_arg_name GREATER -1)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

# vim:ft=cmake
