add_gtest(raas
"\
test_raas/unit_test_raas.cc;\
../mocks/rasys_mock.cc;\
../../src/raas_rasta_assert.c;\
../../src/rahlp_rasta_helper.c;\
"

"\
test_raas/;\
../mocks/;
../../inc/;\
../../../;\
"
)

add_gtest(rahlp
"\
test_rahlp/unit_test_rahlp.cc;\
../mocks/rasys_mock.cc;\
../../src/rahlp_rasta_helper.c;\
"

"\
test_rahlp/;\
../mocks/;
../../inc/;\
../../../;\
"
)

add_gtest(ralog
"\
test_ralog/unit_test_ralog.cc;\
../mocks/rasys_mock.cc;\
../../src/rahlp_rasta_helper.c;\
"

"\
test_ralog/;\
../mocks/;
../../inc/;\
../../../;\
"
)

add_gtest(radef
"\
test_radef/unit_test_radef.cc;\
../mocks/rasys_mock.cc;\
../../src/rahlp_rasta_helper.c;\
"

"\
test_radef/;\
../mocks/;
../../inc/;\
../../../;\
"
)
