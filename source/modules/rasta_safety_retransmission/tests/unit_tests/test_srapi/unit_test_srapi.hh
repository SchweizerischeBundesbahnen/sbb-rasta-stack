/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srapi.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the api module of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRAPI_UNIT_TEST_SRAPI_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRAPI_UNIT_TEST_SRAPI_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "inc/rasta_safety_retransmission/srapi_sr_api.h"
#include "src/srstm_sr_state_machine.h"
#include "srcor_sr_core.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "srcor_mock.hh"
#include "srstm_mock.hh"
#include "srsend_mock.hh"
#include "srrece_mock.hh"
// clang-format on

using testing::NaggyMock;
using testing::StrictMock;
using testing::_;
using testing::SetArgPointee;
using testing::Return;
using testing::Invoke;
using testing::InSequence;
using testing::Throw;
using testing::DoAll;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_SRAPI_MIN_NUM_CONN 1         ///< minimum number of rasta connections
#define UT_SRAPI_MAX_NUM_CONN 2         ///< maximum number of rasta connections
#define UT_SRAPI_CONN_IDX_0 0           ///< minimum connection index 0
#define UT_SRAPI_CONN_IDX_1 1           ///< maximum connection index 1
#define UT_SRAPI_DATA_LEN_MIN 1U        ///< minimum value for payload data length
#define UT_SRAPI_DATA_LEN_MAX 1055U     ///< maximum value for payload data length
#define UT_SRAPI_NSENDMAX_MAX 20        ///< maximum value for nSendMax (buffer size)

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern uint8_t test_payload_data_u8[UT_SRAPI_DATA_LEN_MAX];

extern bool srapi_initialized;                            ///< initialization state (local variable)

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
 * @brief Test fixture class for the api module
 *
 */
class srapiTest : public ::testing::Test
{
public:
  srapiTest() {
    // reset initialization state
    srapi_initialized = false;
  }
  ~srapiTest() {
  }

  /**
   * @brief Check payload data
   * This function verifies the message payload data during a mocked call of the
   * srcor_WriteMessagePayloadToTemporaryBuffer function.
   */
  struct checkPayloadData {
    /**
     * @brief Mocked srcor_WriteMessagePayloadToTemporaryBuffer function
     *
     * @param connection_id         connection ID
     * @param message_payload_size  size of the input data
     * @param message_payload       pointer to the message data
     */
    void operator()(const uint32_t connection_id, const uint16_t message_payload_size, const uint8_t *const message_payload)
    {
      // verify the payload data
      for (uint16_t i = 0; i < message_payload_size; i++)
      {
        EXPECT_EQ(message_payload[i], expected_data[i]);
      }
    }

    /**
     * @brief Construct a new checkPayloadData object
     *
     * @param payload_data  pointer to the expected data
     */
    checkPayloadData(uint8_t* payload_data){
      expected_data = payload_data;
    }

    uint8_t * expected_data;                    ///< pointer to the expected payload data
  };

  /**
   * @brief Provide data and return
   * This function provides data and a return value during a mocked call of the
   * srrece_ReadFromBuffer function.
   */
  struct provideDataAndReturn {
    /**
     * @brief Mocked srrece_ReadFromBuffer function
     *
     * @param connection_id         connection ID
     * @param message_payload       pointer to the message payload structure
     */
    radef_RaStaReturnCode operator()(const uint32_t connection_id, srtyp_SrMessagePayload* const message_payload)
    {
      radef_RaStaReturnCode return_value = radef_kNoError;

      // set the payload size and data
      message_payload->payload_size = data_size;
      for (uint16_t i = 0; i < data_size; i++)
      {
        message_payload->payload[i] = read_data[i];
      }

      if (data_size == 0)
      {
        return_value = radef_kNoMessageReceived;
      }

      return return_value;
    }

    /**
     * @brief Construct a new provideDataAndReturn object
     *
     * @param payload_size  size of the payload data
     * @param payload_data  pointer to the read data
     */
    provideDataAndReturn(uint16_t payload_size, uint8_t* payload_data){
      data_size = payload_size;
      read_data = payload_data;
    }

    uint8_t * read_data;                  ///< pointer to the data to be read
    uint16_t data_size;                   ///< size of the data to be read
  };

  /**
   * @brief Set Received Messages Pending Flag
   * This function sets the Received Messages Pending Flag according to a counter.
   */
  struct setReceivedMessagePendingFlag {
    /**
     * @brief Mocked srrece_ReadFromBuffer function
     *
     * @param connection_id         connection ID
     */
    bool operator()(uint32_t connection_id)
    {
      bool return_value = (bool)*msg_available;

      if(*msg_available > 0){
        (*msg_available)--;
      }

      return return_value;
    }

    /**
     * @brief Construct a new setReceivedMessagePendingFlag object
     *
     * @param n_messages  number of available messages
     */
    setReceivedMessagePendingFlag(uint8_t * n_messages){
      msg_available = n_messages;
    }

    uint8_t * msg_available;              ///< pointer to the data to be read
  };

  /**
   * @brief Decrement Free Buffer Entries
   * This function decrements the number of free buffer entries.
   */
  struct decrementFreeBufferEntries {
    /**
     * @brief Mocked GetFreeBufferEntries function
     *
     * @param connection_id         connection ID
     */
    uint16_t operator()(const uint32_t connection_id)
    {
      uint16_t return_value = *free_elements;

      if(*free_elements > 0){
        (*free_elements)--;
      }

      return return_value;
    }

    /**
     * @brief Construct a new decrementFreeBufferEntries object
     *
     * @param free_buffer_entries   number of free buffer entries
     */
    decrementFreeBufferEntries(uint8_t * free_buffer_entries){
      free_elements = free_buffer_entries;
    }

    uint8_t * free_elements;              ///< pointer to the data to be read
  };

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  StrictMock<rasysMock>   rasys_mock;     ///< mock for the rasta system adapter class
  NaggyMock<srcorMock>    srcor_mock;     ///< mock for the rasta safety and retransmission core class
  NaggyMock<srstmMock>    srstm_mock;     ///< mock for the rasta safety and retransmission state machine class
  NaggyMock<srsendMock>   srsend_mock;    ///< mock for the rasta safety and retransmission send buffer class
  NaggyMock<srreceMock>   srrece_mock;    ///< mock for the rasta safety and retransmission received buffer class
};

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRAPI_UNIT_TEST_SRAPI_HH_
