/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srdia.cc
 *
 * @author Patrick Haldi, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the diagnostics module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srdia.hh"

#include "srdia_sr_diagnostics.h"

using ::testing::_;
using ::testing::SaveArg;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test Class Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized IncXyzErrorCounter tests
 *
 * Parameter order:
 * - Do initialise (bool)
 * - Configured connection ID (uint32_t)
 * - Expected error count (uint32_t)
 * - Expect fatal error (bool)
 * .
 */
class IncXyzErrorCounterTestWithParam : public srdiaTest,
    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>> {
public:
  const uint32_t configured_connection_id = std::get<0>(GetParam()); ///< The connection index to use
  const uint32_t expected_error_count = std::get<1>(GetParam()); ///< How high the according error counter should be after the test
  const bool expect_fatal_error = std::get<2>(GetParam()); ///< Indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_diagnostics
 * @{
 */

/**
 * @test        @ID{srdiaTest005} Verify IncSafetyCodeErrorCounter function
 *
 * @details     This test verifies that the safety code error counter is
 *              incremented correctly.
 *
 * Test steps:
 * - Call the init function
 * - Increase the SafetyCodeError counter N times
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Trigger a diagnostic notification
 * - Verify that the SafetyCodeError counter is N and other counters are 0
 * .
 *
 * @testParameter
 * - Configured connection Id: RaSTA connection identification
 * - Expected error count: How high the SafetyCodeError counter should be after the test
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter          | Expected values                                              |||
 * |----------|--------------------------|----------------------|--------------------|--------------------|
 * |          | Configured connection Id | Expected error count | Expect Fatal Error | Test Result        |
 * | 0        | 0                        | 1                    | False              | Normal operation   |
 * | 1        | 1                        | 1                    | False              | Normal operation   |
 * | 2        | 2                        | 1                    | True               | Fatal error raised |
 * | 3        | 0                        | 1                    | False              | Normal operation   |
 * | 4        | 0                        | 2                    | False              | Normal operation   |
 * | 5        | 0                        | 3                    | False              | Normal operation   |
 * | 6        | 0                        | 4                    | False              | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-639} Inc Safety Code Error Counter Function
 * @verifyReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @verifyReq{RASW-326} Connection Identification
 */
TEST_P(IncXyzErrorCounterTestWithParam, srdiaTest005VerifyIncSafetyCodeErrorCounterFunction) {
  // set expectations /////////////////////////////
  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .WillOnce(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }
  else {
    EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(configured_connection_id, _))
        .WillOnce(SaveArg<1>(&diag_noti))
    ;
  }

  // perform test /////////////////////////////////
  srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals);

  memset(&diag_noti, 0U, sizeof(diag_noti));

  if (expect_fatal_error) {
    EXPECT_THROW(srdia_IncSafetyCodeErrorCounter(configured_connection_id), std::invalid_argument);
  }
  else {
    for (uint32_t error_index = 0U; error_index < expected_error_count; error_index++) {
      EXPECT_NO_THROW(srdia_IncSafetyCodeErrorCounter(configured_connection_id));
    }
    EXPECT_NO_THROW(srdia_SendDiagnosticNotification(configured_connection_id));
    EXPECT_EQ(diag_noti.ec_safety, expected_error_count);
    EXPECT_EQ(diag_noti.ec_address, 0);
    EXPECT_EQ(diag_noti.ec_csn, 0);
    EXPECT_EQ(diag_noti.ec_sn, 0);
    EXPECT_EQ(diag_noti.ec_type, 0);
  }
}

/**
 * @test        @ID{srdiaTest006} Verify IncAddressErrorCounter function
 *
 * @details     This test verifies that the address error counter is
 *              incremented correctly.
 *
 * Test steps:
 * - Call the init function
 * - Increase the AddressError counter N times
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Trigger a diagnostic notification
 * - Verify that the AddressError counter is N and other counters are 0
 * .
 *
 * @testParameter
 * - Configured connection Id: RaSTA connection identification
 * - Expected error count: How high the AddressError counter should be after the test
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter          | Expected values                                              |||
 * |----------|--------------------------|----------------------|--------------------|--------------------|
 * |          | Configured connection Id | Expected error count | Expect Fatal Error | Test Result        |
 * | 0        | 0                        | 1                    | False              | Normal operation   |
 * | 1        | 1                        | 1                    | False              | Normal operation   |
 * | 2        | 2                        | 1                    | True               | Fatal error raised |
 * | 3        | 0                        | 1                    | False              | Normal operation   |
 * | 4        | 0                        | 2                    | False              | Normal operation   |
 * | 5        | 0                        | 3                    | False              | Normal operation   |
 * | 6        | 0                        | 4                    | False              | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-637} Inc Address Error Counter Function
 * @verifyReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @verifyReq{RASW-326} Connection Identification
 */
TEST_P(IncXyzErrorCounterTestWithParam, srdiaTest006VerifyIncAddressErrorCounterFunction) {
  // set expectations /////////////////////////////
  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .WillOnce(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }
  else {
    EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(configured_connection_id, _))
        .WillOnce(SaveArg<1>(&diag_noti))
    ;
  }

  // perform test /////////////////////////////////
  srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals);

  memset(&diag_noti, 0U, sizeof(diag_noti));

  if (expect_fatal_error) {
    EXPECT_THROW(srdia_IncAddressErrorCounter(configured_connection_id), std::invalid_argument);
  }
  else {
    for (uint32_t error_index = 0U; error_index < expected_error_count; error_index++) {
      EXPECT_NO_THROW(srdia_IncAddressErrorCounter(configured_connection_id));
    }
    EXPECT_NO_THROW(srdia_SendDiagnosticNotification(configured_connection_id));
    EXPECT_EQ(diag_noti.ec_address, expected_error_count);
    EXPECT_EQ(diag_noti.ec_safety, 0);
    EXPECT_EQ(diag_noti.ec_csn, 0);
    EXPECT_EQ(diag_noti.ec_sn, 0);
    EXPECT_EQ(diag_noti.ec_type, 0);
  }
}

/**
 * @test        @ID{srdiaTest007} Verify IncTypeErrorCounter function
 *
 * @details     This test verifies that the type error counter is
 *              incremented correctly.
 *
 * Test steps:
 * - Call the init function
 * - Increase the TypeError counter N times
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Trigger a diagnostic notification
 * - Verify that the TypeError counter is N and other counters are 0
 * .
 *
 * @testParameter
 * - Configured connection Id: RaSTA connection identification
 * - Expected error count: How high the TypeError counter should be after the test
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter          | Expected values                                              |||
 * |----------|--------------------------|----------------------|--------------------|--------------------|
 * |          | Configured connection ID | Expected error count | Expect Fatal Error | Test Result        |
 * | 0        | 0                        | 1                    | False              | Normal operation   |
 * | 1        | 1                        | 1                    | False              | Normal operation   |
 * | 2        | 2                        | 1                    | True               | Fatal error raised |
 * | 3        | 0                        | 1                    | False              | Normal operation   |
 * | 4        | 0                        | 2                    | False              | Normal operation   |
 * | 5        | 0                        | 3                    | False              | Normal operation   |
 * | 6        | 0                        | 4                    | False              | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-641} Inc Type Error Counter Function
 * @verifyReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @verifyReq{RASW-326} Connection Identification
 */
TEST_P(IncXyzErrorCounterTestWithParam, srdiaTest007VerifyIncTypeErrorCounterFunction) {
  // set expectations /////////////////////////////
  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .WillOnce(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }
  else {
    EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(configured_connection_id, _))
        .WillOnce(SaveArg<1>(&diag_noti))
    ;
  }

  // perform test /////////////////////////////////
  srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals);

  memset(&diag_noti, 0U, sizeof(diag_noti));

  if (expect_fatal_error) {
    EXPECT_THROW(srdia_IncTypeErrorCounter(configured_connection_id), std::invalid_argument);
  }
  else {
    for (uint32_t error_index = 0U; error_index < expected_error_count; error_index++) {
      EXPECT_NO_THROW(srdia_IncTypeErrorCounter(configured_connection_id));
    }
    EXPECT_NO_THROW(srdia_SendDiagnosticNotification(configured_connection_id));
    EXPECT_EQ(diag_noti.ec_type, expected_error_count);
    EXPECT_EQ(diag_noti.ec_safety, 0);
    EXPECT_EQ(diag_noti.ec_csn, 0);
    EXPECT_EQ(diag_noti.ec_sn, 0);
    EXPECT_EQ(diag_noti.ec_address, 0);
  }
}

/**
 * @test        @ID{srdiaTest008} Verify IncSequenceNumberErrorCounter function
 *
 * @details     This test verifies that the sequence number error counter is
 *              incremented correctly.
 *
 * Test steps:
 * - Call the init function
 * - Increase the SequenceNumberError counter N times
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Trigger a diagnostic notification
 * - Verify that the SequenceNumberError counter is N and other counters are 0
 * .
 *
 * @testParameter
 * - Configured connection Id: RaSTA connection identification
 * - Expected error count: How high the SequenceNumberError counter should be after the test
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter          | Expected values                                              |||
 * |----------|--------------------------|----------------------|--------------------|--------------------|
 * |          | Configured connection ID | Expected error count | Expect Fatal Error | Test Result        |
 * | 0        | 0                        | 1                    | False              | Normal operation   |
 * | 1        | 1                        | 1                    | False              | Normal operation   |
 * | 2        | 2                        | 1                    | True               | Fatal error raised |
 * | 3        | 0                        | 1                    | False              | Normal operation   |
 * | 4        | 0                        | 2                    | False              | Normal operation   |
 * | 5        | 0                        | 3                    | False              | Normal operation   |
 * | 6        | 0                        | 4                    | False              | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-640} Inc Sequence Number Error Counter Function
 * @verifyReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @verifyReq{RASW-326} Connection Identification
 */
TEST_P(IncXyzErrorCounterTestWithParam, srdiaTest008VerifyIncSequenceNumberErrorCounterFunction) {
  // set expectations /////////////////////////////
  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .WillOnce(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }
  else {
    EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(configured_connection_id, _))
        .WillOnce(SaveArg<1>(&diag_noti))
    ;
  }

  // perform test /////////////////////////////////
  srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals);

  memset(&diag_noti, 0U, sizeof(diag_noti));

  if (expect_fatal_error) {
    EXPECT_THROW(srdia_IncSequenceNumberErrorCounter(configured_connection_id), std::invalid_argument);
  }
  else {
    for (uint32_t error_index = 0U; error_index < expected_error_count; error_index++) {
      EXPECT_NO_THROW(srdia_IncSequenceNumberErrorCounter(configured_connection_id));
    }
    EXPECT_NO_THROW(srdia_SendDiagnosticNotification(configured_connection_id));
    EXPECT_EQ(diag_noti.ec_sn, expected_error_count);
    EXPECT_EQ(diag_noti.ec_address, 0);
    EXPECT_EQ(diag_noti.ec_csn, 0);
    EXPECT_EQ(diag_noti.ec_safety, 0);
    EXPECT_EQ(diag_noti.ec_type, 0);
  }
}

/**
 * @test        @ID{srdiaTest009} Verify IncConfirmedSequenceNumberErrorCounterErrorCounter function
 *
 * @details     This test verifies that the confirmed sequence number error counter is
 *              incremented correctly.
 *
 * Test steps:
 * - Call the init function
 * - Increase the ConfirmedSequenceNumberError counter N times
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Trigger a diagnostic notification
 * - Verify that the ConfirmedSequenceNumberError counter is N and other counters are 0
 * .
 *
 * @testParameter
 * - Configured connection Id: RaSTA connection identification
 * - Expected error count: How high the ConfirmedSequenceNumberError counter should be after the test
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter          | Expected values                                              |||
 * |----------|--------------------------|----------------------|--------------------|--------------------|
 * |          | Configured connection ID | Expected error count | Expect Fatal Error | Test Result        |
 * | 0        | 0                        | 1                    | False              | Normal operation   |
 * | 1        | 1                        | 1                    | False              | Normal operation   |
 * | 2        | 2                        | 1                    | True               | Fatal error raised |
 * | 3        | 0                        | 1                    | False              | Normal operation   |
 * | 4        | 0                        | 2                    | False              | Normal operation   |
 * | 5        | 0                        | 3                    | False              | Normal operation   |
 * | 6        | 0                        | 4                    | False              | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-638} Inc Confirmed Sequence Number Error Counter Function
 * @verifyReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @verifyReq{RASW-326} Connection Identification
 */
TEST_P(IncXyzErrorCounterTestWithParam, srdiaTest009VerifyIncConfirmedSequenceNumberErrorCounterFunction) {
  // set expectations /////////////////////////////
  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .WillOnce(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }
  else {
    EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(configured_connection_id, _))
        .WillOnce(SaveArg<1>(&diag_noti))
    ;
  }

  // perform test /////////////////////////////////
  srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals);

  memset(&diag_noti, 0U, sizeof(diag_noti));

  if (expect_fatal_error) {
    EXPECT_THROW(srdia_IncConfirmedSequenceNumberErrorCounter(configured_connection_id), std::invalid_argument);
  }
  else {
    for (uint32_t error_index = 0U; error_index < expected_error_count; error_index++) {
      EXPECT_NO_THROW(srdia_IncConfirmedSequenceNumberErrorCounter(configured_connection_id));
    }
    EXPECT_NO_THROW(srdia_SendDiagnosticNotification(configured_connection_id));
    EXPECT_EQ(diag_noti.ec_csn, expected_error_count);
    EXPECT_EQ(diag_noti.ec_address, 0);
    EXPECT_EQ(diag_noti.ec_safety, 0);
    EXPECT_EQ(diag_noti.ec_sn, 0);
    EXPECT_EQ(diag_noti.ec_type, 0);
  }
}

INSTANTIATE_TEST_SUITE_P(
  ErrorCounterTests,
  IncXyzErrorCounterTestWithParam,
  ::testing::Values(
      std::make_tuple(0U, 1U, false),
      std::make_tuple(1U, 1U, false),
      std::make_tuple(2U, 1U, true),
      std::make_tuple(0U, 1U, false),
      std::make_tuple(0U, 2U, false),
      std::make_tuple(0U, 3U, false),
      std::make_tuple(0U, 4U, false)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

