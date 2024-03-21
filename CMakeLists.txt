cmake_minimum_required(VERSION 3.10)

# =========== options ===================
set(GOOGLE_TEST_PATH "" CACHE PATH "Path to GoogleTest installation folder")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# =========== helper module path  ===================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}; ${CMAKE_CURRENT_SOURCE_DIR}/cmake_helper)

project(
  RastaProtocolReferenceStack
  VERSION 1.1.0
  DESCRIPTION "Software Libary RaSTA Protocol Reference Stack"
  LANGUAGES CXX C)
include(CMakePrintHelpers) # use cmake_print_variables(<VAR>)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(_GENERATORS_LIST
  ""
  CACHE INTERNAL "List of code generators")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
  add_compile_definitions(RELEASE)
else()
  message([FATAL_ERROR] Invalid build type!)
endif()

# =========== unit tests ===================
if(GOOGLE_TEST_PATH)
  message(STATUS "GOOGLE_TEST_PATH is set to: ${GOOGLE_TEST_PATH}")
  find_package(GTest REQUIRED PATHS ${GOOGLE_TEST_PATH} NO_SYSTEM_ENVIRONMENT_PATH)
else()
  find_package(GTest REQUIRED)
endif()

include(GoogleTest)
include(CTest)
include(gtest)

# =========== modules ===================
add_subdirectory(source/modules)

# =========== cpack ===================
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "SBB AG")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RaSTA_Protocol_Reference_Stack")
set(CPACK_PACKAGE_VERSION "1.1.0")
SET(CPACK_COMPONENTS_ALL applications)
set(CPACK_GENERATOR "ZIP")

include(CPack)
