set(LIB_NAME rasta_redundancy)

# Note that headers are optional, and do not affect add_library, but they will
# not show up in IDEs unless they are listed in add_library.
set(HEADER_LIST
	inc/rasta_redundancy/redcty_red_config_types.h
	inc/rasta_redundancy/redint_red_interface.h
	inc/rasta_redundancy/rednot_red_notifications.h
	inc/rasta_redundancy/redtri_transport_interface.h
	inc/rasta_redundancy/redtrn_transport_notifications.h
)

set(SRC_LIST
	src/redcor_red_core.c
	src/redcor_red_core.h
	src/redcrc_red_crc.c
	src/redcrc_red_crc.h
	src/redcty_red_config_types.c
	src/reddfq_red_defer_queue.c
	src/reddfq_red_defer_queue.h
	src/reddia_red_diagnostics.c
	src/reddia_red_diagnostics.h
	src/redint_red_interface.c
	src/redmsg_red_messages.c
	src/redmsg_red_messages.h
	src/redrbf_red_received_buffer.c
	src/redrbf_red_received_buffer.h
	src/redstm_red_state_machine.c
	src/redstm_red_state_machine.h
	src/redtrn_transport_notifications.c
	src/redtyp_red_types.h
)

# Make an automatic library - will be static or dynamic based on user setting
add_library(${LIB_NAME} ${HEADER_LIST} ${SRC_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(${LIB_NAME} PUBLIC inc)
target_include_directories(${LIB_NAME} PRIVATE src)

target_link_libraries(${LIB_NAME} PRIVATE
	rasta_common
)

INSTALL(TARGETS ${LIB_NAME} ARCHIVE DESTINATION lib)
INSTALL(FILES ${HEADER_LIST} DESTINATION inc/${LIB_NAME})

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME
	AND BUILD_TESTING)
	add_subdirectory(tests/unit_tests)
endif()
