/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srcor_config.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the core module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srcor.hh"

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

const uint32_t t_max_min = 750;           ///< max. value for t_max
const uint32_t t_max_max = 2000;          ///< min. value for t_max

const uint32_t t_h_min = 300;             ///< max. value for t_h
const uint32_t t_h_max = 750;             ///< min. value for t_h

const uint32_t n_send_max_min = 2;        ///< max. value for n_send_max
const uint32_t n_send_max_max = 20;       ///< min. value for n_send_max

const uint32_t m_w_a_min = 1;             ///< max. value for m_w_a
const uint32_t m_w_a_max = 19;            ///< min. value for m_w_a

const uint32_t n_max_packet_min = 1;      ///< max. value for n_max_packet
const uint32_t n_max_packet_max = 1;      ///< min. value for n_max_packet

const uint32_t n_diag_win_min = 100;      ///< max. value for n_diag_window
const uint32_t n_diag_win_max = 10000;    ///< min. value for n_diag_window

const uint32_t n_conn_min = 1;            ///< max. value for number_of_connections
const uint32_t n_conn_max = 2;            ///< min. value for number_of_connections

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
 * @brief Test class for value-parametrized IsConfigurationValid test
 *
 * Parameter order:
 * - t_max (uint32_t)
 * - t_h (uint32_t)
 * - safety_code (srcty_SafetyCodeType)
 * - m_w_a (uint16_t)
 * - n_send_max (uint16_t)
 * - n_max_packet (uint32_t)
 * - n_diag_window (uint32_t)
 * - number_of_connections (uint32_t)
 * - connection ID 1 (uint32_t)
 * - receiver ID 1 (uint32_t)
 * - sender ID 1 (uint32_t)
 * - connection ID 2 (uint32_t)
 * - receiver ID 2 (uint32_t)
 * - sender ID 2 (uint32_t)
 * - timing diagnostics intervals valid (bool)
 * - expected return value (bool)
 * .
 */
class IsConfigValidTestWithParam :  public srcorTest,
                                    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, srcty_SafetyCodeType, uint16_t, uint16_t, uint32_t, uint32_t,
                                                                                  uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                                                                                  bool, bool>>
{
    public:
    uint32_t t_max =                    std::get<0>(GetParam());        ///< max. accepted age of message
    uint32_t t_h =                      std::get<1>(GetParam());        ///< heartbeat period
    srcty_SafetyCodeType safety_code =  std::get<2>(GetParam());        ///< safety code type
    uint16_t m_w_a =                    std::get<3>(GetParam());        ///< max. number of received, unconfirmed messages
    uint16_t n_send_max =               std::get<4>(GetParam());        ///< max. number of send messages, unconfirmed messages
    uint32_t n_max_packet =             std::get<5>(GetParam());        ///< packetization factor
    uint32_t n_diag_window =            std::get<6>(GetParam());        ///< diagnosis window size
    uint32_t n_connections =            std::get<7>(GetParam());        ///< number of configured connections
    uint32_t conn_id_1 =                std::get<8>(GetParam());        ///< connection ID 1
    uint32_t rx_id_1 =                  std::get<9>(GetParam());        ///< receiver ID 1
    uint32_t tx_id_1 =                  std::get<10>(GetParam());       ///< sender ID 1
    uint32_t conn_id_2 =                std::get<11>(GetParam());       ///< connection ID 2
    uint32_t rx_id_2 =                  std::get<12>(GetParam());       ///< receiver ID 2
    uint32_t tx_id_2 =                  std::get<13>(GetParam());       ///< sender ID 2
    bool timing_diag_intervals_valid =  std::get<14>(GetParam());       ///< indicates if the timing diagnostics intervals are valid
    bool return_value =                 std::get<15>(GetParam());       ///< expected return value
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_core
 * @{
 */

/**
 * @test        @ID{srcorTest001} Verify the IsConfigurationValid function
 *
 * @details     This test verifies the validity check of the rasta safety and
 *              retransmission layer configuration.
 *              The timing diagnostics intervals are not checked within this
 *              function, only the function call of srdia_AreDiagnosticTimingIntervalsValid
 *              is verified and its return value is simulated.
 *
 * Test steps:
 * - create a safety and retransmission layer configuration according to the test parameter
 * - expect call to srdia_AreDiagnosticTimingIntervalsValid and return true or false according to test parameter
 * - call the isConfigurationValid function and verify the return value
 * .
 *
 * @testParameter
 * - t_max: Maximum accepted age of a message in miliseconds (valid range: 750 - 2000)
 * - t_h: Heartbeat period in miliseconds (valid range: 300 - 750)
 * - safety code: Type of safety code
 * - m_w_a: Maximum number of received, unconfirmed messages (valid range: 1 - 19)
 * - nSendMax: Maximum number of send messages without receiving a confirmation (valid range: 2 - 20)
 * - nMaxPacket: Packetization factor (valid range: 1 - 1)
 * - nDiagWindow: Diagnosis window size (valid range: 100 - 10000)
 * - n_conn: Number of configured connections (valid range: 1 -2)
 * - ConnID, RxID, TxID 1: Id of the connection (valid range: 0 - n_conn), Id of the sender and Id of the receiver on connection 1
 * - ConnID, RxID, TxID 2: Id of the connection (valid range: 0 - n_conn), Id of the sender and Id of the receiver on connection 2
 * - TimDiagInt valid: Return value of srdia_AreDiagnosticTimingIntervalsValid function call
 * - Exp. return value: Expected return value of srcor_IsConfigurationValid function call
 * .
 * | Test Run | Input parameter                                                                                                            |||||||||| Test config      | Expected values                     ||
 * |----------|-------|-----|-------------|-------|----------|------------|-------------|--------|----------------------|---------------------------|------------------|-------------------|------------------|
 * |          | t_max | t_h | safety code | m_w_a | nSendMax | nMaxPacket | nDiagWindow | n_conn | ConnID, RxID, TxID 1 | ConnID, RxID, TxID 2      | TimDiagInt valid | Exp. return value | Test Result      |
 * | 0        | 750   | 300 | LowerMd4    | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 1        | 749   | 300 | LowerMd4    | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 2        | 2001  | 300 | LowerMd4    | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 3        | 2000  | 300 | LowerMd4    | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 4        | 2000  | 299 | LowerMd4    | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 5        | 2000  | 751 | LowerMd4    | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 6        | 2000  | 750 | LowerMd4    | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 7        | 2000  | 750 | None        | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 8        | 2000  | 750 | FullMd4     | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 9        | 2000  | 750 | FullMd4 + 1 | 1     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 10       | 2000  | 750 | FullMd4     | 0     | 20       | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 11       | 2000  | 750 | FullMd4     | 20    | 20       | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 12       | 2000  | 750 | FullMd4     | 19    | 20       | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 13       | 2000  | 750 | FullMd4     | 1     | 0        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 14       | 2000  | 750 | FullMd4     | 1     | 21       | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 15       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 16       | 2000  | 750 | FullMd4     | 2     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 17       | 2000  | 750 | FullMd4     | 3     | 2        | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 18       | 2000  | 750 | FullMd4     | 1     | 20       | 0          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 19       | 2000  | 750 | FullMd4     | 1     | 20       | 2          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 20       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 100         | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 21       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 99          | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 22       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10001       | 1      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 23       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 1      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 24       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 0      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 25       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 3      | 0, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 26       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 2      | 0, 0, 1              | 1, 0, 1                   | True             | True              | Normal Operation |
 * | 27       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 2      | 1, 0, 1              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 28       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 2      | 0, 0, 1              | 2, 0, 1                   | True             | False             | Normal Operation |
 * | 29       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 2      | 0, 0, 0              | 1, 0, 1                   | True             | False             | Normal Operation |
 * | 30       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 2      | 0, 0, 1              | 1, 0, 0                   | True             | False             | Normal Operation |
 * | 31       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 2      | 0, 0, 1              | 1, 0xfffffffe, 0xffffffff | True             | True              | Normal Operation |
 * | 32       | 2000  | 750 | FullMd4     | 1     | 20       | 1          | 10000       | 2      | 0, 0, 1              | 1, 0, 1                   | False            | False             | Normal Operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-427} Struct SafetyRetransmissionConfiguration Structure
 * @verifyReq{RASW-431} Connection Configurations
 * @verifyReq{RASW-423} Struct Connection Configuration Structure
 * @verifyReq{RASW-426} Connection Id
 * @verifyReq{RASW-425} Sender Id
 * @verifyReq{RASW-435} Receiver Id
 * @verifyReq{RASW-436} Number of Connections
 * @verifyReq{RASW-438} N diagWindow
 * @verifyReq{RASW-440} N maxPacket
 * @verifyReq{RASW-441} N sendmax
 * @verifyReq{RASW-442} MWA
 * @verifyReq{RASW-428} MD4 Initial Value
 * @verifyReq{RASW-437} Struct MD4 Initial Value Structure
 * @verifyReq{RASW-432} Init Value A, B, C, D
 * @verifyReq{RASW-443} Safety Code Type
 * @verifyReq{RASW-444} T h
 * @verifyReq{RASW-445} T max
 * @verifyReq{RASW-446} RaSTA Network Id
 * @verifyReq{RASW-573} Is Configuration Valid Function
 */
TEST_P(IsConfigValidTestWithParam, srcorTest001VerifyIsConfigurationValidFunction) {
  // create rasta safety and retransmission layer configuration
  const srcty_SafetyRetransmissionConfiguration config = {
    .rasta_network_id = 0, // full range is valid - therefore not tested
    .t_max = t_max,
    .t_h = t_h,
    .safety_code_type = safety_code,
    .m_w_a = m_w_a,
    .n_send_max = n_send_max,
    .n_max_packet = n_max_packet,
    .n_diag_window = n_diag_window,
    .number_of_connections = n_connections,
    .connection_configurations = {{conn_id_1, rx_id_1, tx_id_1}, {conn_id_2, rx_id_2, tx_id_2}},
    .md4_initial_value = {0, 0, 0, 0}, // full range is valid, therefore not tested
    .diag_timing_distr_intervals = {111, 222, 333, 444} // values are not checked in srcor, only the function call of srdia_AreDiagnosticTimingIntervalsValid
  };

  // expect call to srdia_AreDiagnosticTimingIntervalsValid if preceding tests are ok
  if (  ((t_max_min <= t_max) && (t_max <= t_max_max)) &&
        ((t_h_min <= t_h) && (t_h <= t_h_max)) &&
        ((srcty_kSafetyCodeTypeMin <= safety_code) && (safety_code < srcty_kSafetyCodeTypeMax)) &&
        ((m_w_a_min <= m_w_a) && (m_w_a <= m_w_a_max)) &&
        ((n_send_max_min <= n_send_max) && (n_send_max <= n_send_max_max)) &&
        (m_w_a < n_send_max) &&
        ((n_max_packet_min <= n_max_packet) && (n_max_packet <= n_max_packet_max)) &&
        ((n_diag_win_min <= n_diag_window) && (n_diag_window <= n_diag_win_max)) &&
        ((n_conn_min <= n_connections) && (n_connections <= n_conn_max)) )
  {
    EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(config.t_max, config.diag_timing_distr_intervals))
      .WillOnce(Return(timing_diag_intervals_valid));
  }

  // call the IsConfigurationValid function and verify the return value
  EXPECT_EQ(return_value, srcor_IsConfigurationValid(&config));
}

INSTANTIATE_TEST_SUITE_P(
    verifyIsConfigurationValidFunction,
    IsConfigValidTestWithParam,
    ::testing::Values(
        std::make_tuple(t_max_min, t_h_min, srcty_kSafetyCodeTypeLowerMd4, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        // check t_max
        std::make_tuple((t_max_min - 1), t_h_min, srcty_kSafetyCodeTypeLowerMd4, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple((t_max_max + 1), t_h_min, srcty_kSafetyCodeTypeLowerMd4, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_min, srcty_kSafetyCodeTypeLowerMd4, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        // check t_h
        std::make_tuple(t_max_max, (t_h_min - 1), srcty_kSafetyCodeTypeLowerMd4, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, (t_h_max + 1), srcty_kSafetyCodeTypeLowerMd4, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeLowerMd4, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        // check safety_code
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeNone, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeMax, m_w_a_min, n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        // check m_w_a
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, (m_w_a_min - 1), n_send_max_max, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, (m_w_a_max + 1), n_send_max_max, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        // check n_send_max
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, (n_send_max_min - 1), n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, (n_send_max_max + 1), n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        // check m_w_a < n_send_max
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, (m_w_a_min + 1), n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, (m_w_a_min + 2), n_send_max_min, n_max_packet_min, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        // check n_max_packet
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, (n_max_packet_min - 1), n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, (n_max_packet_max + 1), n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_min,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        // check n_diag_win
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, (n_diag_win_min - 1),
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, (n_diag_win_max + 1),
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        n_conn_min, 0, 0, 1, 1, 0, 1, true, true),
        // check number_of_connections
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        (n_conn_min - 1), 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        (n_conn_max + 1), 0, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        n_conn_max, 0, 0, 1, 1, 0, 1, true, true),
        // check connection ID, receiver ID and sender ID
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        n_conn_max, 1, 0, 1, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        n_conn_max, 0, 0, 1, 2, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        n_conn_max, 0, 0, 0, 1, 0, 1, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        n_conn_max, 0, 0, 1, 1, 0, 0, true, false),
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        n_conn_max, 0, 0, 1, 1, (UINT32_MAX - 1), UINT32_MAX, true, true),
        // check diag_timing_distr_intervals
        std::make_tuple(t_max_max, t_h_max, srcty_kSafetyCodeTypeFullMd4, m_w_a_max, n_send_max_max, n_max_packet_max, n_diag_win_max,
                        n_conn_max, 0, 0, 1, 1, 0, 1, false, false)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
