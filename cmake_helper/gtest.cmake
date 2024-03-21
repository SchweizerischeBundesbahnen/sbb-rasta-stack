# Functionality to add a google test
#
# Macro ADD_GTEST Usage:
# ADD_GTEST(name source_files include_directories)
# Parameters:
# name - Name of the gtest should be unique
# -> example: gtest_temperature
# source files -  List of necessary source files
# -> example: "../service.cc;test_temperature.cc" include_directories
# include_directories - Necessary include directories
# -> example: ${PROJECT_SOURCE_DIR}/modules/
macro(ADD_GTEST name files include_dirs)
  set(TESTNAME gtest_${name})

  add_executable(${TESTNAME} ${files})
  include_directories(${TESTNAME} PUBLIC ${include_dirs})
  target_link_libraries(${TESTNAME} GTest::gtest_main GTest::gtest GTest::gmock)
  target_include_directories(${TESTNAME} PUBLIC ../inc)
  target_include_directories(${TESTNAME} PRIVATE ../src)

  add_test(${TESTNAME}_test ${TESTNAME})
endmacro()
