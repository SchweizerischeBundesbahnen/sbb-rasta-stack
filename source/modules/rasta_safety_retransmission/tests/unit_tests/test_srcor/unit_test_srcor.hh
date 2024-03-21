/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srcor.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version c43d3a924e35983d1734435fa2bbfbffb1750d0c
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 *
 * @brief Header file for unit tests of the core module of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRCOR_UNIT_TEST_SRCOR_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRCOR_UNIT_TEST_SRCOR_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/srcor_sr_core.h"
#include "rasta_common/raas_rasta_assert.h"
#include "test_helper.hh"
#include "rasys_mock.hh"
#include "srmsg_mock.hh"
#include "srdia_mock.hh"
#include "srsend_mock.hh"
#include "srrece_mock.hh"
#include "sradin_mock.hh"
#include "srnot_mock.hh"
// clang-format on

using testing::NaggyMock;
using testing::StrictMock;
using testing::_;
using testing::SetArgPointee;
using testing::SetArgReferee;
using testing::SetArrayArgument;
using testing::SaveArg;
using testing::DoAll;
using testing::Return;
using testing::Invoke;
using testing::InSequence;
using testing::Throw;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_SRCOR_CONNECTION_NUM_MIN                     1U         ///< minimum number of connections
#define UT_SRCOR_CONNECTION_NUM_MAX                     2U         ///< maximum number of connections
#define UT_SRCOR_CONNECTION_ID_MIN                      0U         ///< minimum connection id
#define UT_SRCOR_CONNECTION_ID_MAX                      1U         ///< maximum connection id
#define UT_SRCOR_PAYLOAD_LEN_MIN                        1U         ///< minimum length of data message payload
#define UT_SRCOR_PAYLOAD_LEN_MAX                        1055U      ///< maximum length of data message payload
#define UT_SRCOR_MESSAGE_LEN_MAX                        1101U      ///< maximum message length
#define UT_SRCOR_SEND_BUFFER_SIZE_MAX                   20U        ///< maximum size of send buffer [messages]
#define UT_SRCOR_SEND_MIN_FREE_ENTRIES_TO_SEND          3          ///< minimum free send buffer entries to send [messages]
#define UT_SRCOR_RECEIVE_BUFFER_SIZE_MAX                20U        ///< maximum size of receive buffer [messages]
#define UT_SRCOR_RECEIVE_MIN_FREE_ENTRIES_TO_RECEIVE    1          ///< minimum free receive buffer entries for receive [messages]
#define UT_SRCOR_NSENDMAX_MAX                           20U        ///< maximum value for nSendMax (buffer size)
#define UT_SRCOR_HB_MIN                                 300U       ///< minimum heartbeat period
#define UT_SRCOR_HB_MAX                                 750U       ///< maximum heartbeat period
#define UT_SRCOR_TIMER_GRANULARITY                      1          ///< timer granularity

#define UT_SRCOR_SEQ_RANGE_MAX UT_SRCOR_NSENDMAX_MAX * 10U        ///< max range that the seq nbr still accepted

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern bool srcor_initialized;                                                      ///< initialization state (module local variable)
extern uint8_t test_payload_data_u8[UT_SRCOR_PAYLOAD_LEN_MAX];                      ///< test payload data
extern uint8_t test_message_data_u8[UT_SRCOR_MESSAGE_LEN_MAX];                      ///< test message data
extern srcor_RaStaConnectionData srcor_rasta_connections[UT_SRCOR_CONNECTION_NUM_MAX];    ///< rasta connection data

/**
 * @brief default config for tests
 *
 * This configuration is used when no other specific configuration is used or tested. (Taken from srcfg_sr_config)
 */
const srcty_SafetyRetransmissionConfiguration default_config = {
  .rasta_network_id = 123456U,
  .t_max = 750U,
  .t_h = 300U,
  .safety_code_type = srcty_kSafetyCodeTypeLowerMd4,
  .m_w_a = 10U,
  .n_send_max = 20U,
  .n_max_packet = 1U,
  .n_diag_window = 5000U,
  .number_of_connections = 2U,
  {
    {
      .connection_id = 0U,
      .sender_id = 0x61U,
      .receiver_id = 0x62U,
    },
    {
      .connection_id = 1U,
      .sender_id = 1U,
      .receiver_id = 3U,
    },
  },
  {
    .init_a = 0x67452301U,
    .init_b = 0xEFCDAB89U,
    .init_c = 0x98BADCFEU,
    .init_d = 0x10325476U,
  },  // MD4 initial value
  {
    150U,
    300U,
    450U,
    600U,
  }  // intervals for timing diagnostics
};

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test Class Definition
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the core module
 *
 */
class srcorTest : public ::testing::Test
{
public:
  srcorTest() {
    // reset initialization state
    srcor_initialized = false;
  }
  ~srcorTest() {
  }

  static const uint8_t expected_message_length_pos    = 0;    ///< expected byte position of the Message Length (according to RASW-160)
  static const uint8_t expected_message_type_pos      = 2;    ///< expected byte position of the Message Type (according to RASW-160)
  static const uint8_t expected_receiver_id_pos       = 4;    ///< expected byte position of the Receiver ID (according to RASW-160)
  static const uint8_t expected_sender_id_pos         = 8;    ///< expected byte position of the Sender ID (according to RASW-160)
  static const uint8_t expected_sequence_nbr_pos      = 12;   ///< expected byte position of the Sequence Number (according to RASW-160)
  static const uint8_t expected_conf_seq_nbr_pos      = 16;   ///< expected byte position of the Conf. Sequence Number (according to RASW-160)
  static const uint8_t expected_timestamp_pos         = 20;   ///< expected byte position of the Timestamp (according to RASW-160)
  static const uint8_t expected_conf_timestamp_pos    = 24;   ///< expected byte position of the Conf. Timestamp (according to RASW-160)
  srtyp_ProtocolVersion expected_protocol_version = {48, 51, 48, 51}; ///< expected protocol version (03.03)

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  /**
   * @brief Compare md4 values with default
   * This function is used to compare the md4 init values of a mocked function call with the default config.
   *
   * @param configured_safety_code_type configured safety code type
   * @param configured_md4_initial_value configured md4 initial value
   */
  void checkDefaultMd4InitValues(const srcty_SafetyCodeType configured_safety_code_type, const srcty_Md4InitValue configured_md4_initial_value){
    EXPECT_EQ(configured_md4_initial_value.init_a, default_config.md4_initial_value.init_a);
    EXPECT_EQ(configured_md4_initial_value.init_b, default_config.md4_initial_value.init_b);
    EXPECT_EQ(configured_md4_initial_value.init_c, default_config.md4_initial_value.init_c);
    EXPECT_EQ(configured_md4_initial_value.init_d, default_config.md4_initial_value.init_d);
  }

  /**
   * @brief Get the header of a message
   * This function is used to extract the header data in a message
   *
   * @param sr_message pointer to the message
   * @param message_header pointer to the header for storing data
   */
  void getMessageHeader(const srtyp_SrMessage *const sr_message, srtyp_SrMessageHeader *const message_header) {
    message_header->message_length = GetUint16FromMessage(sr_message, expected_message_length_pos);
    message_header->message_type = (srtyp_SrMessageType)GetUint16FromMessage(sr_message, expected_message_type_pos);
    message_header->receiver_id = GetUint32FromMessage(sr_message, expected_receiver_id_pos);
    message_header->sender_id = GetUint32FromMessage(sr_message, expected_sender_id_pos);
    message_header->sequence_number = GetUint32FromMessage(sr_message, expected_sequence_nbr_pos);
    message_header->confirmed_sequence_number = GetUint32FromMessage(sr_message, expected_conf_seq_nbr_pos);
    message_header->time_stamp = GetUint32FromMessage(sr_message, expected_timestamp_pos);
    message_header->confirmed_time_stamp = GetUint32FromMessage(sr_message, expected_conf_timestamp_pos);
  }

  /**
   * @brief Get Uint16 value from
   * This function is used to get a uint16 value from a message
   *
   * @param sr_message pointer to the message, from where a variable must be extracted
   * @param position Start position where the variable must be read in the message
   * @return uint16_t Read variable from the specified start position
   */
  static uint16_t GetUint16FromMessage(const srtyp_SrMessage *const sr_message, const uint16_t position) {
    // Get variable from message
    uint16_t data = 0U;
    for (uint16_t index = 0U; index < srcty_kByteCountUint16; ++index) {
      data += (uint16_t)sr_message->message[position + index] << (index * 8U);
    }
    return data;
  }

  /**
   * @brief Get Uint32 value from
   * This function is used to get a uint32 value from a message
   *
   * @param sr_message pointer to the message, from where a variable must be extracted
   * @param position Start position where the variable must be read in the message
   * @return uint32_t Read variable from the specified start position
   */
  static uint32_t GetUint32FromMessage(const srtyp_SrMessage *const sr_message, const uint16_t position) {
    // Get variable from message
    uint32_t data = 0U;
    for (uint16_t index = 0U; index < srcty_kByteCountUint32; ++index) {
      data += (uint32_t)sr_message->message[position + index] << (index * 8U);
    }
    return data;
  }

  StrictMock<rasysMock>   rasys_mock;     ///< mock for the rasta system adapter class
  NaggyMock<srmsgMock>    srmsg_mock;     ///< mock for the rasta safety and retransmission messages class
  NaggyMock<srdiaMock>    srdia_mock;     ///< mock for the rasta safety and retransmission diagnostics class
  NaggyMock<srsendMock>   srsend_mock;    ///< mock for the rasta safety and retransmission send buffer class
  NaggyMock<srreceMock>   srrece_mock;    ///< mock for the rasta safety and retransmission received buffer class
  NaggyMock<sradinMock>   sradin_mock;    ///< mock for the rasta safety and retransmission adapter interface class
  NaggyMock<srnotMock>    srnot_mock;     ///< mock for the rasta safety and retransmission notifications class
};

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRCOR_UNIT_TEST_SRCOR_HH_
