set(LIB_NAME rasta_common)

# Note that headers are optional, and do not affect add_library, but they will
# not show up in IDEs unless they are listed in add_library.
set(HEADER_LIST
	inc/rasta_common/raas_rasta_assert.h
	inc/rasta_common/radef_rasta_definitions.h
	inc/rasta_common/rahlp_rasta_helper.h
	inc/rasta_common/ralog_rasta_logger.h
	inc/rasta_common/rasys_rasta_system_adapter.h
)

set(SRC_LIST
	src/raas_rasta_assert.c
	src/rahlp_rasta_helper.c
	src/ralog_rasta_logger.c
)

# Make an automatic library - will be static or dynamic based on user setting
add_library(${LIB_NAME} ${HEADER_LIST} ${SRC_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(${LIB_NAME} PUBLIC inc)
target_include_directories(${LIB_NAME} PRIVATE src)

INSTALL(TARGETS ${LIB_NAME} ARCHIVE DESTINATION lib)
INSTALL(FILES ${HEADER_LIST} DESTINATION inc/${LIB_NAME})

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME
	AND BUILD_TESTING)
	add_subdirectory(tests/unit_tests)
endif()
