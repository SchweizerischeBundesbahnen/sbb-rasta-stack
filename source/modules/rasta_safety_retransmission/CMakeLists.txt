set(LIB_NAME rasta_safety_retransmission)

# Note that headers are optional, and do not affect add_library, but they will
# not show up in IDEs unless they are listed in add_library.
set(HEADER_LIST
	inc/rasta_safety_retransmission/sradin_sr_adapter_interface.h
	inc/rasta_safety_retransmission/sradno_sr_adapter_notifications.h
	inc/rasta_safety_retransmission/srapi_sr_api.h
	inc/rasta_safety_retransmission/sraty_sr_api_types.h
	inc/rasta_safety_retransmission/srcty_sr_config_types.h
	inc/rasta_safety_retransmission/srnot_sr_notifications.h
)

set(SRC_LIST
	src/sradno_sr_adapter_notifications.c
	src/srapi_sr_api.c
	src/srcor_sr_core.c
	src/srcor_sr_core.h
	src/srcty_sr_config_types.c
	src/srdia_sr_diagnostics.c
	src/srdia_sr_diagnostics.h
	src/srmd4_sr_md4.c
	src/srmd4_sr_md4.h
	src/srmsg_sr_messages.c
	src/srmsg_sr_messages.h
	src/srrece_sr_received_buffer.c
	src/srrece_sr_received_buffer.h
	src/srsend_sr_send_buffer.c
	src/srsend_sr_send_buffer.h
	src/srstm_sr_state_machine.c
	src/srstm_sr_state_machine.h
	src/srtyp_sr_types.h
)

# Make an automatic library - will be static or dynamic based on user setting
add_library(${LIB_NAME} ${HEADER_LIST} ${SRC_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(${LIB_NAME} PUBLIC inc)
target_include_directories(${LIB_NAME} PRIVATE src)

target_link_libraries(${LIB_NAME} PRIVATE
	rasta_common
	rasta_redundancy
)

INSTALL(TARGETS ${LIB_NAME} ARCHIVE DESTINATION lib)
INSTALL(FILES ${HEADER_LIST} DESTINATION inc/${LIB_NAME})

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME
	AND BUILD_TESTING)
	add_subdirectory(tests/unit_tests)
endif()
