/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redint.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the redundancy layer interface.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDINT_UNIT_TEST_REDINT_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDINT_UNIT_TEST_REDINT_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "inc/rasta_redundancy/redint_red_interface.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "redcor_mock.hh"
#include "reddfq_mock.hh"
#include "redrbf_mock.hh"
#include "redmsg_mock.hh"
#include "redtri_mock.hh"
#include "test_helper.hh"
#include "redstm_mock.hh"
// clang-format on

using testing::_;
using testing::StrictMock;
using testing::Return;
using testing::Invoke;
using testing::InSequence;
using testing::Throw;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// Message and buffer
#define UT_REDINT_MSG_LEN_MIN                       28U        ///< minimum value for message length
#define UT_REDINT_MSG_LEN_MAX                       1101U      ///< maximum value for message length
#define UT_REDINT_MSG_BUFFER_SIZE_MIN               28U        ///< minimum value for buffer size
#define UT_REDINT_MSG_BUFFER_SIZE_MAX               1101U      ///< maximum value for buffer size

// Redundancy channel
#define UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN        1U         ///< minimum number of redundancy channels
#define UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX        2U         ///< maximum number of redundancy channels
#define UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN         0U         ///< minimum redundancy channel id
#define UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX         1U         ///< maximum redundancy channel id

// Transport channels
#define UT_REDINT_TRANSPORT_CHANNEL_NUM_MAX         2U         ///< maximum number of transport channels

// Entries
#define UT_REDINT_BUFFER_ENTRIES_MAX                20U        ///< maximum buffer entries
#define UT_REDINT_DEFER_QUEUE_ENTRIES_MAX           10U        ///< maximum defer queue size

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief test config of channel
 *
 * This struct is used to get channel config and set expected values in tests
 */
typedef struct {
  uint16_t channel_id;                            ///< channel id
  redstm_RedundancyChannelStates state;           ///< channel state
  bool dfq_timeout;                               ///< is defer queue timeout
  uint16_t pending_messages;                      ///< pending messages
  uint16_t free_bfr_entries;                      ///< free buffer entries
  uint32_t used_dfq_entries;                      ///< used defer queue entries
} test_channel;

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern uint8_t test_red_message_data_u8[UT_REDINT_MSG_LEN_MAX];
extern radef_RaStaReturnCode redint_initialization_state;            ///< initialization state (local variable)

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
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test Class Definition
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the defer queue module
 *
 */
class redintTest : public ::testing::Test
{
public:
  redintTest()
  {
    // reset initialization state
    redint_initialization_state = radef_kNotInitialized;
  }

  ~redintTest()
  {

  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  /**
   * @brief initializes the module and sets the current state
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
  static redcty_RedundancyLayerConfiguration CreateConfig(redcty_CheckCodeType check_code_type , uint32_t t_seq, uint32_t n_diagnosis, uint32_t n_defer_queue_size,
  uint32_t num_redundancy_channels, uint32_t red_channel_id_start, uint32_t num_transport_channels, uint32_t transport_channel_id_start)
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
        if(t < UT_REDINT_TRANSPORT_CHANNEL_NUM_MAX){
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

  StrictMock<rasysMock> rasys_mock;  ///< mock for the rasta system adapter class
  reddfqMock reddfq_mock;            ///< mock for the redundancy defer queue class
  redcorMock redcor_mock;            ///< mock for the redundancy core class
  redrbfMock redrbf_mock;            ///< mock for the redundancy received buffer class
  redtriMock redtri_mock;            ///< mock for the redundancy transport interface
  redstmMock redstm_mock;            ///< mock for the redundancy state machine class
};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDINT_UNIT_TEST_REDINT_HH_
