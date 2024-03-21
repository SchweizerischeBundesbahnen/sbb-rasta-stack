/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redtrn.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the transport notifications module of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redtrn.hh"

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
// Test class definitons
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - Transport channel ID (uint32_t)
 * - Associated Redundancy channel from transport channel (uint32_t)
 * - Redundancy channel state (redstm_RedundancyChannelStates)
 * - Free buffer entries of redundancy channel (uint16_t)
 * - Used defer queue entries of redundancy channel (uint32_t)
 * - Return code of redtri_ReadMessage function call (radef_RaStaReturnCode)
 * - Expect fatal error (bool)
 * .
 */
class MessageReceivedNotificationTestWithParam :  public redtrnTest,
                                                  public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, redstm_RedundancyChannelStates, uint16_t, uint32_t, radef_RaStaReturnCode, bool>>
{
  public:
  uint32_t transport_channel_id =                               std::get<0>(GetParam());     ///< transport channel id
  uint32_t associated_red_channel =                             std::get<1>(GetParam());     ///< redundancy channel of the tr channel
  redstm_RedundancyChannelStates redundancy_channel_state =     std::get<2>(GetParam());     ///< redundancy channel state
  uint16_t free_buffer_entries =                                std::get<3>(GetParam());     ///< free buffer entries
  uint32_t used_defer_queue_entries =                           std::get<4>(GetParam());     ///< used defer queue entries
  radef_RaStaReturnCode read_message_function_call_result =     std::get<5>(GetParam());     ///< return code of read message call
  bool expect_fatal_error =                                     std::get<6>(GetParam());     ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup red_transport_notifications
 * @{
 */

/**
 * @test        @ID{redtrnTest001} Verify the MessageReceivedNotification function.
 *
 * @details     This test verifies the correct behaviour of the MessageReceivedNotification function
 *
 * Test steps:
 * - set expectations for function calls according to parameter when no error expected
 *   - Set associated redundancy channel id
 *   - Set return value for state machine call
 *   - Call for core module clear message pending flag when no message received (given from parameter)
 *   - When state up return values of free buffer and used queue entries
 *     - set transport interface read message function call return value on expected call
 *     - If free entries available set expectations for writing received message to input buffer, state machine call and clearing pending flag
 *     - If no free entries expect set message pending flag call
 *   - When state closed or notInitialized message gets read and discarded
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call function
 * .
 *
 * @testParameter
 * - Transport CH Id: Transport channel identification
 * - Associated red CH Id: Associated Redundancy identification of Transport channel
 * - Channel state: State of redundancy channel
 * - Free buffer entries: Free buffer entries on redundancy channel (messages)
 * - Used defer queue entries: Used defer queue entries on redundancy channel (messages)
 * - Readmessage call return code: Return value of redtri_ReadMessage function call
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config                                                                                                      ||||| Expected values                        ||
 * |----------|-----------------|----------------------|----------------|--------------------|--------------------------|------------------------------|--------------------|--------------------|
 * |          | Transport CH Id | Associated red CH Id | Channel state  | Free buffer entries| Used defer queue entries | Readmessage call return code | Expect Fatal Error | Test Result        |
 * | 0        | 0U              | 0U                   | Closed         | 20U                | 0U                       | radef_kNoMessageReceived     | false              | normal operation   |
 * | 1        | 0U              | 0U                   | Closed         | 20U                | 0U                       | radef_kNoError               | false              | normal operation   |
 * | 2        | 0U              | 0U                   | Up             | 20U                | 0U                       | radef_kNoMessageReceived     | false              | normal operation   |
 * | 3        | 0U              | 0U                   | Up             | 20U                | 0U                       | radef_kNoError               | false              | normal operation   |
 * | 4        | 1U              | 0U                   | Up             | 20U                | 0U                       | radef_kNoError               | false              | normal operation   |
 * | 5        | 2U              | 1U                   | Up             | 20U                | 0U                       | radef_kNoError               | false              | normal operation   |
 * | 6        | 3U              | 1U                   | Up             | 20U                | 0U                       | radef_kNoError               | false              | normal operation   |
 * | 7        | 0U              | 0U                   | Up             | 19U                | 1U                       | radef_kNoError               | false              | normal operation   |
 * | 8        | 0U              | 0U                   | Up             | 0U                 | 10U                      | radef_kNoError               | false              | normal operation   |
 * | 9        | 4U              | 1U                   | Up             | 20U                | 0U                       | radef_kNoError               | true               | fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(MessageReceivedNotificationTestWithParam, redtrnTest001VerifyMessageReceivedNotificationFunction)
{
  if(!expect_fatal_error)
  {
    // set associated redundancy channel to given parameter
    EXPECT_CALL(redcor_mock, redcor_GetAssociatedRedundancyChannel(transport_channel_id, _)).WillOnce(SetArgPointee<1>(associated_red_channel));

    // return configured channel state
    EXPECT_CALL(redstm_mock, redstm_GetChannelState(associated_red_channel)).WillOnce(Return(redundancy_channel_state));

    // If there is no message to read, the received message pending flag of this transport channel is cleared.
    if(read_message_function_call_result == radef_kNoMessageReceived)
    {
      EXPECT_CALL(redcor_mock, redcor_ClearMessagePendingFlag(associated_red_channel, transport_channel_id)).Times(1);
    }

    if(redundancy_channel_state == redstm_kRedundancyChannelStateUp)
    {
      EXPECT_CALL(redrbf_mock, redrbf_GetFreeBufferEntries(associated_red_channel)).WillOnce(Return(free_buffer_entries));
      EXPECT_CALL(reddfq_mock, reddfq_GetUsedEntries(associated_red_channel)).WillOnce(Return(used_defer_queue_entries));

      if(free_buffer_entries > used_defer_queue_entries)
      {
        EXPECT_CALL(redtri_mock, redtri_ReadMessage(transport_channel_id, _, _, _)).WillOnce(Return(read_message_function_call_result));

        // if no error occurred while reading message, message gets written to input buffer, state machine gets updated and pending flag will be cleared
        if(read_message_function_call_result == radef_kNoError)
        {
          EXPECT_CALL(redcor_mock, redcor_WriteReceivedMessageToInputBuffer(associated_red_channel, transport_channel_id, _)).Times(1);
          EXPECT_CALL(redstm_mock, redstm_ProcessChannelStateMachine(associated_red_channel, redstm_kRedundancyChannelEventReceiveData)).Times(1);
          EXPECT_CALL(redcor_mock, redcor_ClearInputBufferMessagePendingFlag(associated_red_channel)).Times(1);
        }
      }
      else
      {
        // if there are not enough free buffer entries in the received buffer, the message is not read and the received message pending flag of this transport channel is set.
        EXPECT_CALL(redcor_mock, redcor_SetMessagePendingFlag(associated_red_channel, transport_channel_id)).Times(1);
      }

    }
    else
    {
      // if the associated redundancy is not in the up state, the message is read and discarded.
      EXPECT_CALL(redtri_mock, redtri_ReadMessage(transport_channel_id, _, _, _)).WillOnce(Return(read_message_function_call_result));
    }
  }

  // perform test
  if(expect_fatal_error)
  {
    // call the MessageReceivedNotification function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redtrnTest::invalidArgumentException));
    EXPECT_THROW(redtrn_MessageReceivedNotification(transport_channel_id), std::invalid_argument);
  }
  else
  {
    // call the MessageReceivedNotification function and verify if parameter are set correctly
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redtrn_MessageReceivedNotification(transport_channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyMessageReceivedNotificationFunction,
  MessageReceivedNotificationTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDTRN_TR_CHANNEL_ID_MIN, UT_REDTRN_RED_CHANNEL_ID_MIN, redstm_kRedundancyChannelStateClosed, UT_REDTRN_BFR_ENTRIES_MAX, UT_REDTRN_DFQ_ENTRIES_MIN, radef_kNoMessageReceived, false),
    std::make_tuple(UT_REDTRN_TR_CHANNEL_ID_MIN, UT_REDTRN_RED_CHANNEL_ID_MIN, redstm_kRedundancyChannelStateClosed, UT_REDTRN_BFR_ENTRIES_MAX, UT_REDTRN_DFQ_ENTRIES_MIN, radef_kNoError, false),
    std::make_tuple(UT_REDTRN_TR_CHANNEL_ID_MIN, UT_REDTRN_RED_CHANNEL_ID_MIN, redstm_kRedundancyChannelStateUp, UT_REDTRN_BFR_ENTRIES_MAX, UT_REDTRN_DFQ_ENTRIES_MIN, radef_kNoMessageReceived, false),
    std::make_tuple(UT_REDTRN_TR_CHANNEL_ID_MIN, UT_REDTRN_RED_CHANNEL_ID_MIN, redstm_kRedundancyChannelStateUp, UT_REDTRN_BFR_ENTRIES_MAX, UT_REDTRN_DFQ_ENTRIES_MIN, radef_kNoError, false),
    std::make_tuple(1, UT_REDTRN_RED_CHANNEL_ID_MIN, redstm_kRedundancyChannelStateUp, UT_REDTRN_BFR_ENTRIES_MAX, UT_REDTRN_DFQ_ENTRIES_MIN, radef_kNoError, false),
    std::make_tuple(2, UT_REDTRN_RED_CHANNEL_ID_MAX, redstm_kRedundancyChannelStateUp, UT_REDTRN_BFR_ENTRIES_MAX, UT_REDTRN_DFQ_ENTRIES_MIN, radef_kNoError, false),
    std::make_tuple(UT_REDTRN_TR_CHANNEL_ID_MAX, UT_REDTRN_RED_CHANNEL_ID_MAX, redstm_kRedundancyChannelStateUp, UT_REDTRN_BFR_ENTRIES_MAX, UT_REDTRN_DFQ_ENTRIES_MIN, radef_kNoError, false),
    std::make_tuple(UT_REDTRN_TR_CHANNEL_ID_MIN, UT_REDTRN_RED_CHANNEL_ID_MIN, redstm_kRedundancyChannelStateUp, (UT_REDTRN_BFR_ENTRIES_MAX - 1), (UT_REDTRN_DFQ_ENTRIES_MIN + 1), radef_kNoError, false),
    std::make_tuple(UT_REDTRN_TR_CHANNEL_ID_MIN, UT_REDTRN_RED_CHANNEL_ID_MIN, redstm_kRedundancyChannelStateUp, UT_REDTRN_BFR_ENTRIES_MIN, UT_REDTRN_DFQ_ENTRIES_MAX, radef_kNoError, false),
    std::make_tuple((UT_REDTRN_TR_CHANNEL_ID_MAX + 1), UT_REDTRN_RED_CHANNEL_ID_MAX, redstm_kRedundancyChannelStateUp, UT_REDTRN_BFR_ENTRIES_MAX, UT_REDTRN_DFQ_ENTRIES_MIN, radef_kNoError, true)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
