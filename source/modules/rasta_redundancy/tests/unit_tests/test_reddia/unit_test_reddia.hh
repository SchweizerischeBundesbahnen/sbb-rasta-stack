/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_reddia.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 8b98e76f21adede1cf571b89227c3be9b13bb77f
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4936: Fixed IsConfigurationValid calls CreateConfig with invalid amount of transport channels,05.12.2022,N. Fricker}
 *
 *
 * @brief Header file for unit tests of the diagnostics module of the redundancy layer.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDDIA_UNIT_TEST_REDDIA_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDDIA_UNIT_TEST_REDDIA_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/reddia_red_diagnostics.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "redcor_mock.hh"
#include "rednot_mock.hh"
#include "test_helper.hh"
// clang-format on

using testing::_;
using testing::StrictMock;
using testing::Invoke;
using testing::Return;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define UT_REDDIA_NUM_OF_RED_CHANNELS_MIN 1U        ///< minimum number of redundancy channels
#define UT_REDDIA_NUM_OF_RED_CHANNELS_MAX 2U        ///< maximum number of redundancy channels
#define UT_REDDIA_NUM_OF_TR_CHANNELS_MIN 1U         ///< minimum number of transport channels per redundancy channel
#define UT_REDDIA_NUM_OF_TR_CHANNELS_MAX 2U         ///< maximum number of transport channels per redundancy channel
#define UT_REDDIA_RED_CHANNEL_ID_MIN 0U             ///< minimum id of redundancy channel
#define UT_REDDIA_RED_CHANNEL_ID_MAX 1U             ///< maximum id of redundancy channel
#define UT_REDDIA_TR_CHANNEL_ID_MIN 0U              ///< minimum id of transport channel
#define UT_REDDIA_TR_CHANNEL_ID_MAX 3U              ///< maximum id of transport channel
#define UT_REDDIA_DFQ_SIZE_MIN 4U                   ///< minimum defer queue size
#define UT_REDDIA_DFQ_SIZE_MAX 10U                  ///< maximum defer queue size
#define UT_REDDIA_N_DIAGNOSIS_MIN 10U               ///< minimum diagnosis window size
#define UT_REDDIA_N_DIAGNOSIS_MAX 1000U             ///< maximum diagnosis window size
#define UT_REDDIA_T_SEQ_MIN 50U                     ///< minimum time period (in ms) how long a message is buffered
#define UT_REDDIA_T_SEQ_MAX 500U                    ///< maximum time period (in ms) how long a message is buffered

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

extern bool reddia_initialized;                                                 ///< initialization state (local variable)
extern redcty_RedundancyLayerConfiguration *reddia_redundancy_configuration;    ///< redundancy layer configuration (local variable)
extern uint32_t reddia_current_n_diagnosis[UT_REDDIA_NUM_OF_RED_CHANNELS_MAX];  ///< current messages in diagnosis window
extern reddia_ReceivedMessageTimestamp reddia_received_messages_timestamps[RADEF_MAX_NUMBER_OF_RED_CHANNELS][RADEF_MAX_RED_LAYER_N_DIAGNOSIS];               ///< timestamps of received messages
extern radef_TransportChannelDiagnosticData reddia_transport_channel_diagnostic_data[UT_REDDIA_NUM_OF_RED_CHANNELS_MAX][UT_REDDIA_NUM_OF_TR_CHANNELS_MAX];   ///< transport channel diagnostic data

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
 * @brief Test fixture class for the diagnostics module
 *
 */
class reddiaTest : public ::testing::Test
{
public:
  reddiaTest() {
    // reset initialization state
    reddia_initialized = false;
  }
  ~reddiaTest() {
  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
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

    uint32_t red_channel_iterations = num_redundancy_channels;
    uint32_t tr_channel_iterations = num_transport_channels;

    if(num_redundancy_channels > UT_REDDIA_NUM_OF_RED_CHANNELS_MAX)
    {
      red_channel_iterations = UT_REDDIA_NUM_OF_RED_CHANNELS_MAX;
    }

    if(num_transport_channels > UT_REDDIA_NUM_OF_RED_CHANNELS_MAX)
    {
      tr_channel_iterations = UT_REDDIA_NUM_OF_RED_CHANNELS_MAX;
    }

    // create config for redundancy channels
    for(uint16_t i = 0U; i < red_channel_iterations; i++)
    {
      redcty_RedundancyChannelConfiguration red_channel_config = {red_channel_id_start, num_transport_channels};

      // create config for transport channels in redundancy channel
      for(uint16_t t = 0U; t < tr_channel_iterations; t++){
          red_channel_config.transport_channel_ids[t] = transport_channel_id_start;
          transport_channel_id_start++;
      }

      config.redundancy_channel_configurations[i] = red_channel_config;
      red_channel_id_start++;
    };

    return config;
  }

  StrictMock<rasysMock> rasys_mock;       ///< mock for the rasta system adapter class
  redcorMock redcor_mock;                 ///< mock for the redundancy core class
  rednotMock rednot_mock;                 ///< mock for the redundancy notification class
};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDDIA_UNIT_TEST_REDDIA_HH_
