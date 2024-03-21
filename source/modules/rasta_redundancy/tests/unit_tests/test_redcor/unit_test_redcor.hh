/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redcor.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version b4d4bd413ce5fa54d373eb00890116763cac504a
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 *
 * @brief Header file for unit tests of the core module of the redundancy layer.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDCOR_UNIT_TEST_REDCOR_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDCOR_UNIT_TEST_REDCOR_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/redcor_red_core.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "reddfq_mock.hh"
#include "reddia_mock.hh"
#include "redrbf_mock.hh"
#include "redmsg_mock.hh"
#include "redtri_mock.hh"
#include "test_helper.hh"
// clang-format on

using testing::_;
using testing::StrictMock;
using testing::InSequence;
using testing::Return;
using testing::SetArgPointee;
using testing::Invoke;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// Message
#define UT_REDCOR_DATA_LEN_MIN                      28U        ///< minimum value for payload data length (min sr message size)
#define UT_REDCOR_DATA_LEN_MAX                      1101U      ///< maximum value for payload data length (max sr message size)
#define UT_REDCOR_MSG_LEN_MAX                       1113U      ///< maximum value for message length
#define UT_REDCOR_MSG_LEN_MIN                       36U        ///< maximum value for message length

// Redundancy channel
#define UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MIN        1U         ///< minimum number of redundancy channels
#define UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MAX        2U         ///< maximum number of redundancy channels
#define UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN         0U         ///< minimum redundancy channel id
#define UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX         1U         ///< maximum redundancy channel id

// Transport channels
#define UT_REDCOR_TRANSPORT_CHANNEL_NUM_MIN         1U         ///< minimum number of transport channels
#define UT_REDCOR_TRANSPORT_CHANNEL_NUM_MAX         2U         ///< maximum number of transport channels
#define UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN          0U         ///< minimum transport channel id
#define UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX          3U         ///< maximum transport channel id

// Defer Queue
#define UT_REDCOR_T_SEQ_MIN                         50U        ///< minimum time period (in ms) how long a message is buffered
#define UT_REDCOR_T_SEQ_MAX                         500U       ///< maximum time period (in ms) how long a message is buffered
#define UT_REDCOR_QUEUE_SIZE_MIN                    4U         ///< minimum size of the defer queue
#define UT_REDCOR_QUEUE_SIZE_MAX                    10U        ///< maximum size of the defer queue

// Diagnosis
#define UT_REDCOR_N_DIAGNOSIS_MIN                   10U        ///< Minimum diagnosis window size
#define UT_REDCOR_N_DIAGNOSIS_MAX                   1000U      ///< Maximum diagnosis window size

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

extern uint8_t test_red_message_data_u8[UT_REDCOR_MSG_LEN_MAX];
extern uint8_t test_red_payload_data_u8[UT_REDCOR_DATA_LEN_MAX];
extern bool redcor_initialized;                                                                         ///< initialization state (local variable)
extern const redcty_RedundancyLayerConfiguration *redcor_redundancy_configuration;                      ///< redundancy channel configuration (in red_core)
extern redcor_RedundancyChannelData redcor_redundancy_channels[UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MAX];   ///< redundancy channels (in red_core)

/**
 * @brief default config for tests
 *
 * This configuration is used when no other specific configuration is used or tested. (Taken from redcfg_red_config)
 */
const redcty_RedundancyLayerConfiguration default_config = {
    .check_code_type = redcty_kCheckCodeA,
    .t_seq = 50U,
    .n_diagnosis = 200U,
    .n_defer_queue_size = 4U,
    .number_of_redundancy_channels = 2U,
    {
        {
            .red_channel_id = 0U,
            .num_transport_channels = 2U,
            .transport_channel_ids = {0U, 1U},
        },
        {
            .red_channel_id = 1U,
            .num_transport_channels = 2U,
            .transport_channel_ids = {2U, 3U},
        },
    },
};

// -----------------------------------------------------------------------------
// Test class definiton
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the core module
 *
 */
class redcorTest : public ::testing::Test
{
public:
  redcorTest() {
    // reset initialization state
    redcor_initialized = false;
  }
  ~redcorTest() {
  }

  /**
   * @brief Function to comapre if a sequence number is older than another
   * Taken from reddfq_IsSequenceNumberOlder function
   */
  bool IsSequenceNumberOlder(const uint32_t sequence_number_to_compare, const uint32_t sequence_number_reference) {
    bool is_sequence_number_older;

    // Unsigned integer wrap around allowed in this calculation
    // + 1U is added to get the "older" functionality and not the "older or equal"
    if (((sequence_number_reference - (sequence_number_to_compare + 1U)) < (UINT32_MAX / 2U))) {
      is_sequence_number_older = true;
    } else {
      is_sequence_number_older = false;
    }

    return is_sequence_number_older;
  }

  /**
   * @brief Function to verify message data
   * This functions verifies that the message data is equal to the test payload data (which gets used in the tests)
   */
  void VerifySendMessageData(const uint32_t transport_channel_id, const uint16_t message_size, const uint8_t* const message_data) {
    for (uint16_t i = 0; i < message_size; i++)
    {
      ASSERT_EQ(message_data[i], test_red_message_data_u8[i]);
    }
  }

  /**
   * @brief creates a redundancy layer configuration
   *
   * This function returns a config with values given from parameters for the redundancy layer
   *
   * @param check_code_type                 Check code type
   * @param t_seq                           Time to buffer seq messages
   * @param n_diagnosis                     Diagnosis window size
   * @param n_defer_queue_size              Defer queue size
   * @param num_redundancy_channels         Number of redundancy channels
   * @param red_channel_id_start            Redundancy channel start id (will be increased by 1)
   * @param num_transport_channels          Number of transport channels per redundancy channel
   * @param transport_channel_id_start      Transport channel start id (will be increased by 1)
   *
   */
  static redcty_RedundancyLayerConfiguration CreateConfig(redcty_CheckCodeType check_code_type , uint32_t t_seq, uint32_t n_diagnosis, uint32_t n_defer_queue_size, uint32_t num_redundancy_channels,
                                                        uint32_t red_channel_id_start, uint32_t num_transport_channels, uint32_t transport_channel_id_start)
  {
    redcty_RedundancyLayerConfiguration config = {
      check_code_type,
      t_seq,
      n_diagnosis,
      n_defer_queue_size,
      num_redundancy_channels
    };

    // create config for redundancy channels
    for(uint16_t i = 0U; i < num_redundancy_channels; i++)
    {
      redcty_RedundancyChannelConfiguration red_channel_config = {red_channel_id_start, num_transport_channels};

      // create config for transport channels in redundancy channel
      for(uint16_t t = 0U; t < num_transport_channels; t++){
        if(t < UT_REDCOR_TRANSPORT_CHANNEL_NUM_MAX){
          red_channel_config.transport_channel_ids[t] = transport_channel_id_start;
          transport_channel_id_start++;
        }
      }

      config.redundancy_channel_configurations[i] = red_channel_config;
      red_channel_id_start++;

      // reset transport channel id for next redundancy channel
      transport_channel_id_start = 0;
    };

    return config;
  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  static const uint8_t expected_sequence_nbr_pos      = 4;                ///< expected Byteposition of the Sequence Number (according to RASW-202)

  StrictMock<rasysMock> rasys_mock;       ///< mock for the rasta system adapter class
  reddfqMock reddfq_mock;                 ///< mock for the redundancy defer queue class
  reddiaMock reddia_mock;                 ///< mock for the redundancy diagnostics class
  redmsgMock redmsg_mock;                 ///< mock for the redundancy messages class
  redrbfMock redrbf_mock;                 ///< mock for the redundancy received buffer class
  redtriMock redtri_mock;                 ///< mock for the redundancy transport interface
};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDCOR_UNIT_TEST_REDCOR_HH_
