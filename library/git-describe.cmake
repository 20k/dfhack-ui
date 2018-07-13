execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=8 --long
    WORKING_DIRECTORY "${dfhack_SOURCE_DIR}"
    OUTPUT_VARIABLE DFHACK_GIT_DESCRIPTION)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY "${dfhack_SOURCE_DIR}"
    OUTPUT_VARIABLE DFHACK_GIT_COMMIT)
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=8 --exact-match
    WORKING_DIRECTORY "${dfhack_SOURCE_DIR}"
    RESULT_VARIABLE DFHACK_GIT_TAGGED_RESULT
    OUTPUT_QUIET ERROR_QUIET)

# Check library/xml for matching commits
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD:library/xml
    WORKING_DIRECTORY "${dfhack_SOURCE_DIR}"
    OUTPUT_VARIABLE DFHACK_GIT_XML_EXPECTED_COMMIT)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY "${dfhack_SOURCE_DIR}/library/xml"
    OUTPUT_VARIABLE DFHACK_GIT_XML_COMMIT)

file(WRITE ${git_describe_tmp_h} "")

macro(git_describe_definition var)
    string(STRIP "${${var}}" GIT_DEF_VALUE)
    file(APPEND ${git_describe_tmp_h} "#define ${var} \"${GIT_DEF_VALUE}\"\n")
endmacro()

git_describe_definition(DFHACK_GIT_DESCRIPTION)
git_describe_definition(DFHACK_GIT_COMMIT)
git_describe_definition(DFHACK_GIT_XML_EXPECTED_COMMIT)
git_describe_definition(DFHACK_GIT_XML_COMMIT)
git_describe_definition(DFHACK_BUILD_ID)
if(${DFHACK_GIT_TAGGED_RESULT} EQUAL 0)
    file(APPEND ${git_describe_tmp_h} "#define DFHACK_GIT_TAGGED\n")
endif()
if(${DFHACK_GIT_XML_COMMIT} STREQUAL ${DFHACK_GIT_XML_EXPECTED_COMMIT})
    file(APPEND ${git_describe_tmp_h} "#define DFHACK_GIT_XML_MATCH\n")
endif()
