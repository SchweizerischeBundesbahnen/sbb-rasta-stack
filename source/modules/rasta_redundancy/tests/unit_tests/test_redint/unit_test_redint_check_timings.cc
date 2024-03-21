/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redint_check_timings.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the check timings function in redundancy layer interface.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redint.hh"

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
 * @brief Test class for CheckTimings test
 *
 * Parameter order:
 * - Amount of configured redundancy channels (uint32_t)
 * - Channel 0 State (redstm_RedundancyChannelStates)
 * - Channel 0 Defer queue timeout (bool)
 * - Channel 0 Pending messages (uint16_t)
 * - Channel 0 Free buffer entries (uint16_t)
 * - Channel 0 Used defer queue entries (uint32_t)
 * - Channel 1 State (redstm_RedundancyChannelStates)
 * - Channel 1 Defer queue timeout (bool)
 * - Channel 1 Pending messages (uint16_t)
 * - Channel 1 Free buffer entries (uint16_t)
 * - Channel 1 Used defer queue entries (uint32_t)
 * - Indicates if a message only is pending on one of the tr channels (bool)
 * - Indicates if a message was read but pending flag not cleared (bool)
 * .
 */
class CheckTimingsTestWithParam :   public redintTest, public testing::WithParamInterface<std::tuple<
                                      uint32_t,
                                      redstm_RedundancyChannelStates, bool, uint16_t, uint16_t, uint32_t,
                                      redstm_RedundancyChannelStates, bool, uint16_t, uint16_t, uint32_t,
                                      bool, bool>>
{
  public:
  uint32_t configured_redundancy_channels      = std::get<0>(GetParam());        ///< amount of redundancy channels
  redstm_RedundancyChannelStates ch_0_state    = std::get<1>(GetParam());        ///< channel 0 state
  bool ch_0_dfq_timeout                        = std::get<2>(GetParam());        ///< indicates if channel 0 defer queue is in timeout
  uint16_t ch_0_pending_messages               = std::get<3>(GetParam());        ///< amount of pending messages on channel 0
  uint16_t ch_0_free_bfr_entries               = std::get<4>(GetParam());        ///< amount of free buffer entries on channel 0
  uint32_t ch_0_used_dfq_entries               = std::get<5>(GetParam());        ///< amount of used buffer entries on channel 0
  redstm_RedundancyChannelStates ch_1_state    = std::get<6>(GetParam());        ///< channel 1 state
  bool ch_1_dfq_timeout                        = std::get<7>(GetParam());        ///< indicates if channel 1 defer queue is in timeout
  uint16_t ch_1_pending_messages               = std::get<8>(GetParam());        ///< amount of pending messages on channel 1
  uint16_t ch_1_free_bfr_entries               = std::get<9>(GetParam());        ///< amount of free buffer entries on channel 1
  uint32_t ch_1_used_dfq_entries               = std::get<10>(GetParam());       ///< amount of used buffer entries on channel 1
  bool msg_only_pending_on_one_test_channel    = std::get<11>(GetParam());       ///< indicates if pending message only on 1 channel
  bool message_read_but_still_pending          = std::get<12>(GetParam());       ///< indicates if a message was not read correctly
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup red_api
 * @{
 */

/**
 * @test        @ID{redintTest010} Verify the CheckTimings function.
 *
 * @details     This test verifies the CheckTimings function
 *
 * Test steps:
 * - call the init function with given redundancy channels in configuration
 * - set expectations for every redundancy channel configured
 * - set expectations if state up
 *   - set buffer and queue entries and pending messages according to parameter
 *   - set expectations to add message for every pending message as long as free entry available
 *   - set expectations if channel timeout
 * - set expectations if state closed
 *   - read every pending message and clear its flag
 * - init module
 * - call CheckTimings function and expect radef_kNoError as return value
 * .
 *
 * @testParameter
 * - Red channels: Number of configured redundancy channels
 * - Channel 0 State: Current state of channel 0
 * - Channel 0 Dfq timeout: Indicates if defer queue of channel 0 is in timeout
 * - Channel 0 Msgs pending: Number of messages pending on channel 0
 * - Channel 0 Free bfr entries: Number of free buffer entries (messages) on channel 0
 * - Channel 0 Used dfq entries: Number of used buffer entries (messages) on channel 0
 * - Channel 1 State: Current state of channel 1
 * - Channel 1 Dfq timeout: Indicates if defer queue of channel 1 is in timeout
 * - Channel 1 Msgs pending: Number of messages pending on channel 1
 * - Channel 1 Free bfr entries: Number of free buffer entries (messages) on channel 1
 * - Channel 1 Used dfq entries: Number of used buffer entries (messages) on channel 0
 * - Msg pending on one tr channel: Indicates if message pending only on one transport channel
 * - Msg still pending: Indicates if a message gets not read correctly
 * .
 * | Test Run | Test config                                                                                                                                                                                                  ||||||||||||| Expected values  |
 * |----------|--------------|--------|-------------|--------------|------------------|------------------|--------|-------------|--------------|------------------|------------------|-------------------------------|-------------------|------------------|
 * |          |              | Channel 0                                                             ||||| Channel 1                                                             |||||                               |                   | Test Result      |
 * |          | Red channels | State  | Dfq timeout | Msgs pending | Free bfr entries | Used dfq entries | State  | Dfq timeout | Msgs pending | Free bfr entries | Used dfq entries | Msg pending on one tr channel | Msg still pending | Normal operation |
 * | 0        |           1U | Closed | false       | 0U           | 0U               | 0U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 1        |           1U | Closed | false       | 1U           | 0U               | 0U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 2        |           1U | Closed | false       | 3U           | 0U               | 0U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 3        |           1U | Up     | false       | 0U           | 0U               | 0U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 4        |           1U | Up     | true        | 0U           | 0U               | 0U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 5        |           1U | Up     | false       | 1U           | 20U              | 0U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 6        |           1U | Up     | false       | 1U           | 20U              | 0U               | Closed | false       | 0U           | 0U               | 0U               | true                          | false             | Normal operation |
 * | 7        |           1U | Up     | false       | 3U           | 20U              | 0U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 8        |           1U | Up     | false       | 2U           | 1U               | 9U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 9        |           1U | Up     | false       | 1U           | 20U              | 0U               | Closed | false       | 0U           | 0U               | 0U               | false                         | true              | Normal operation |
 * | 10       |           1U | Up     | false       | 1U           | 19U              | 1U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 11       |           1U | Up     | false       | 1U           | 0U               | 10U              | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 12       |           1U | Up     | false       | 1U           | 1U               | 9U               | Closed | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * |          | vary state of channel 2                                                                                                                                                                                      |||||||||||||                  |
 * | 13       |           2U | Closed | false       | 1U           | 0U               | 0U               | Up     | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 14       |           2U | Up     | false       | 0U           | 0U               | 0U               | Up     | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 15       |           2U | Up     | true        | 0U           | 0U               | 0U               | Up     | false       | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 16       |           2U | Up     | true        | 0U           | 0U               | 0U               | Up     | true        | 0U           | 0U               | 0U               | false                         | false             | Normal operation |
 * | 17       |           1U | Up     | false       | 1U           | 20U              | 0U               | Closed | false       | 1U           | 20U              | 0U               | false                         | true              | Normal operation |
 * | 18       |           2U | Up     | false       | 1U           | 20U              | 0U               | Up     | false       | 1U           | 20U              | 0U               | false                         | false             | Normal operation |
 * | 19       |           2U | Up     | false       | 3U           | 20U              | 0U               | Up     | false       | 3U           | 20U              | 0U               | false                         | false             | Normal operation |
 * | 20       |           2U | Up     | false       | 3U           | 20U              | 0U               | Up     | false       | 2U           | 1U               | 9U               | false                         | false             | Normal operation |
 * | 21       |           2U | Up     | false       | 2U           | 1U               | 9U               | Up     | false       | 2U           | 1U               | 9U               | false                         | false             | Normal operation |
 * | 22       |           2U | Up     | false       | 1U           | 19U              | 1U               | Up     | false       | 1U           | 20U              | 0U               | false                         | false             | Normal operation |
 * | 23       |           2U | Up     | false       | 1U           | 19U              | 1U               | Up     | false       | 1U           | 19U              | 1U               | false                         | false             | Normal operation |
 * | 24       |           2U | Up     | false       | 1U           | 0U               | 10U              | Up     | false       | 1U           | 0U               | 10U              | false                         | false             | Normal operation |
 * | 25       |           2U | Up     | false       | 1U           | 1U               | 9U               | Up     | false       | 1U           | 0U               | 10U              | false                         | false             | Normal operation |
 * | 26       |           2U | Up     | false       | 1U           | 1U               | 9U               | Up     | false       | 1U           | 1U               | 9U               | false                         | false             | Normal operation |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(CheckTimingsTestWithParam, redintTest010VerifyCheckTimings)
{
  // test variables
  test_channel channels[UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX] = {
    {0, ch_0_state, ch_0_dfq_timeout, ch_0_pending_messages, ch_0_free_bfr_entries, ch_0_used_dfq_entries},
    {1, ch_1_state, ch_1_dfq_timeout, ch_1_pending_messages, ch_1_free_bfr_entries, ch_1_used_dfq_entries}
  };

  redcty_RedundancyLayerConfiguration config = CreateConfig(
    default_config.check_code_type,
    default_config.t_seq,
    default_config.n_diagnosis,
    default_config.n_defer_queue_size,
    configured_redundancy_channels,         // given parameter
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN,
    UT_REDINT_TRANSPORT_CHANNEL_NUM_MAX,
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN
  );

  // Set init call expectations
  EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(redcor_mock, redcor_Init(&config)).Times(1);
  EXPECT_CALL(redstm_mock, redstm_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(redrbf_mock, redrbf_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(reddfq_mock, reddfq_Init(config.number_of_redundancy_channels, config.n_defer_queue_size, config.t_seq)).Times(1);

  for(uint16_t i = 0; i < configured_redundancy_channels; i++ )
  {
    // set messages pending according to parameter
    uint16_t pending_messages = channels[i].pending_messages;

    // return states from parameter
    EXPECT_CALL(redstm_mock, redstm_GetChannelState(channels[i].channel_id)).WillOnce(Return(channels[i].state));

    if(channels[i].state == redstm_kRedundancyChannelStateUp)
    {
      // set buffer and queue entries according to parameter
      uint16_t free_buffer_entries = channels[i].free_bfr_entries;
      uint32_t used_defer_queue_entries = channels[i].used_dfq_entries;

      {
        InSequence s;
        EXPECT_CALL(redrbf_mock, redrbf_GetFreeBufferEntries(channels[i].channel_id)).WillOnce(Return(free_buffer_entries));
        EXPECT_CALL(reddfq_mock, reddfq_GetUsedEntries(channels[i].channel_id)).WillOnce(Return(used_defer_queue_entries));

        if(channels[i].pending_messages > 0)
        {
          EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).Times(2).WillRepeatedly(Return(true));
        }
        else
        {
          EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).Times(2).WillRepeatedly(Return(false));
        }

        while (free_buffer_entries > used_defer_queue_entries)
        {

          // expect calls for pending messages which gets successfully read and written to buffer
          // - except for last message if parameter is given (to simulate failed read operation which did not marked message as read)
          if(msg_only_pending_on_one_test_channel)
          {
            EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).WillOnce(Return(false));
            msg_only_pending_on_one_test_channel = false;
          }
          else
          {
            EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).WillOnce(Return(true));

            if(!message_read_but_still_pending && free_buffer_entries > 1)
            {
              EXPECT_CALL(redtri_mock, redtri_ReadMessage(_, _, _, _)).WillOnce(Return(radef_kNoError));
              EXPECT_CALL(redcor_mock, redcor_WriteReceivedMessageToInputBuffer(_, _, _));
              EXPECT_CALL(redstm_mock, redstm_ProcessChannelStateMachine(_, redstm_kRedundancyChannelEventReceiveData));
              EXPECT_CALL(redcor_mock, redcor_ClearInputBufferMessagePendingFlag(_));
            }
            else
            {
              EXPECT_CALL(redtri_mock, redtri_ReadMessage(_, _, _, _)).WillOnce(Return(radef_kNoMessageReceived));
              EXPECT_CALL(redcor_mock, redcor_ClearMessagePendingFlag(channels[i].channel_id, _));
            }
          }

          EXPECT_CALL(redrbf_mock, redrbf_GetFreeBufferEntries(channels[i].channel_id)).WillOnce(Return(free_buffer_entries));
          EXPECT_CALL(reddfq_mock, reddfq_GetUsedEntries(channels[i].channel_id)).WillOnce(Return(used_defer_queue_entries));

          if(pending_messages == 0)
          {
            EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).Times(2).WillRepeatedly(Return(false));
            break;
          }
          else
          {
            EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).Times(2).WillRepeatedly(Return(true));
          }

          pending_messages--;
        }

        used_defer_queue_entries++;
        free_buffer_entries--;
      }

      // expect calls for channel timeout
      EXPECT_CALL(reddfq_mock, reddfq_IsTimeout(channels[i].channel_id)).WillRepeatedly(Return(channels[i].dfq_timeout));
      if (channels[i].dfq_timeout)
      {
        EXPECT_CALL(redstm_mock, redstm_ProcessChannelStateMachine(channels[i].channel_id, redstm_kRedundancyChannelEventDeferTimeout)).Times(1);
      }
    }
    else
    {
      if(pending_messages == 0)
      {
        EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).Times(2).WillRepeatedly(Return(false));
      }
      else
      {
        uint16_t tr_channel_index = 0;
        InSequence s;
        while(pending_messages >= 0)
        {
          if(pending_messages == 0)
          {
            EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).Times(2).WillRepeatedly(Return(false));
            break;
          }

          EXPECT_CALL(redcor_mock, redcor_GetMessagePendingFlag(channels[i].channel_id, _)).Times(2).WillRepeatedly(Return(true));

          if(tr_channel_index > 1)
          {
            // while loop ends if transport channel index overflows
            break;
          }

          EXPECT_CALL(redtri_mock, redtri_ReadMessage(_, _, _, _)).WillOnce(Return(radef_kNoError));
          EXPECT_CALL(redtri_mock, redtri_ReadMessage(_, _, _, _)).WillOnce(Return(radef_kNoMessageReceived));
          EXPECT_CALL(redcor_mock, redcor_ClearMessagePendingFlag(channels[i].channel_id, _));

          pending_messages--;
          tr_channel_index ++;
        }
      }
    }
  }

  // init module
  redint_Init(&config);

  // perform test
  EXPECT_EQ(radef_kNoError, redint_CheckTimings());
}

INSTANTIATE_TEST_SUITE_P(
  verifyCheckTimings,
  CheckTimingsTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateClosed, false, 1, 0, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateClosed, false, 3, 0, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 0, 0, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, true, 0, 0, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 1, UT_REDINT_BUFFER_ENTRIES_MAX, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 1, UT_REDINT_BUFFER_ENTRIES_MAX, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, true, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 3, UT_REDINT_BUFFER_ENTRIES_MAX, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 2, 1, (UT_REDINT_DEFER_QUEUE_ENTRIES_MAX - 1), redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 1, UT_REDINT_BUFFER_ENTRIES_MAX, 0, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, true),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 1, (UT_REDINT_BUFFER_ENTRIES_MAX - 1), 1, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 1, 0, UT_REDINT_DEFER_QUEUE_ENTRIES_MAX, redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 1, 1, (UT_REDINT_DEFER_QUEUE_ENTRIES_MAX - 1), redstm_kRedundancyChannelStateClosed, false, 0, 0, 0, false, false),
    // vary state of channel 2
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateClosed, false, 1, 0, 0, redstm_kRedundancyChannelStateUp, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 0, 0, 0, redstm_kRedundancyChannelStateUp, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, true, 0, 0, 0, redstm_kRedundancyChannelStateUp, false, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, true, 0, 0, 0, redstm_kRedundancyChannelStateUp, true, 0, 0, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, redstm_kRedundancyChannelStateUp, false, 1, UT_REDINT_BUFFER_ENTRIES_MAX, 0, redstm_kRedundancyChannelStateClosed, false, 1, UT_REDINT_BUFFER_ENTRIES_MAX, 0, false, true),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 1, UT_REDINT_BUFFER_ENTRIES_MAX, 0, redstm_kRedundancyChannelStateUp, false, 1, UT_REDINT_BUFFER_ENTRIES_MAX, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 3, UT_REDINT_BUFFER_ENTRIES_MAX, 0, redstm_kRedundancyChannelStateUp, false, 3, UT_REDINT_BUFFER_ENTRIES_MAX, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 3, UT_REDINT_BUFFER_ENTRIES_MAX, 0, redstm_kRedundancyChannelStateUp, false, 2, 1, (UT_REDINT_DEFER_QUEUE_ENTRIES_MAX - 1), false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 2, 1, (UT_REDINT_DEFER_QUEUE_ENTRIES_MAX - 1), redstm_kRedundancyChannelStateUp, false, 2, 1, (UT_REDINT_DEFER_QUEUE_ENTRIES_MAX - 1), false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 1, (UT_REDINT_BUFFER_ENTRIES_MAX - 1), 1, redstm_kRedundancyChannelStateUp, false, 1, UT_REDINT_BUFFER_ENTRIES_MAX, 0, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 1, (UT_REDINT_BUFFER_ENTRIES_MAX - 1), 1, redstm_kRedundancyChannelStateUp, false, 1, (UT_REDINT_BUFFER_ENTRIES_MAX - 1), 1, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 1, 0, UT_REDINT_DEFER_QUEUE_ENTRIES_MAX, redstm_kRedundancyChannelStateUp, false, 1, 0, UT_REDINT_DEFER_QUEUE_ENTRIES_MAX, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 1, 1, (UT_REDINT_DEFER_QUEUE_ENTRIES_MAX - 1), redstm_kRedundancyChannelStateUp, false, 1, 0, UT_REDINT_DEFER_QUEUE_ENTRIES_MAX, false, false),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, redstm_kRedundancyChannelStateUp, false, 1, 1, (UT_REDINT_DEFER_QUEUE_ENTRIES_MAX - 1), redstm_kRedundancyChannelStateUp, false, 1, 1, (UT_REDINT_DEFER_QUEUE_ENTRIES_MAX - 1), false, false)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
