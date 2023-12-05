set(LIB_NAME rasta_safety_retransmission_config)

# Note that headers are optional, and do not affect add_library, but they will
# not show up in IDEs unless they are listed in add_library.
set(HEADER_LIST
	inc/rasta_safety_retransmission_config/srcfg_sr_config.h
)

set(SRC_LIST
	src/srcfg_sr_config.c
)

# Make an automatic library - will be static or dynamic based on user setting
add_library(${LIB_NAME} ${HEADER_LIST} ${SRC_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(${LIB_NAME} PUBLIC inc)
target_include_directories(${LIB_NAME} PRIVATE src)

target_link_libraries(${LIB_NAME} PRIVATE
	rasta_common
	rasta_safety_retransmission
)

INSTALL(FILES ${HEADER_LIST} DESTINATION inc/${LIB_NAME})
