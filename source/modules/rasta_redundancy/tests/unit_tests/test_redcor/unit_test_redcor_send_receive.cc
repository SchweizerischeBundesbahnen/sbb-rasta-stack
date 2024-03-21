/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redcor_send_receive.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version b2539a325a88a1efa06194472c78a90d33bf93d6
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4937: Fixed out of boundary access for array in redcor unit tests,05.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4968: Added channel id check before setting sequence numbers,12.12.2022,N. Fricker}
 *
 * @brief Unit test file for the core module send and receive tests of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redcor.hh"

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
 * @brief Test class for value-parametrized WriteReceivedMessageToInputBuffer function test
 *
 * Parameter order:
 * - Redundancy Channel id (uint32_t)
 * - Transport channel id (uint32_t)
 * - Indicates if transport channel id is valid
 * - Message size (uint16_t)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class WriteReceivedMessageToInputBufferTestWithParam :    public redcorTest,
                                                          public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, uint16_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t redundancy_channel_id                   = std::get<0>(GetParam());        ///< Redundancy channel id
  uint32_t transport_channel_id                    = std::get<1>(GetParam());        ///< Transport channel id
  bool transport_channel_id_valid                  = std::get<2>(GetParam());        ///< Indicates if transport channel id is valid
  redtyp_RedundancyMessage received_message        = {std::get<3>(GetParam()),{}};   ///< Message
  radef_RaStaReturnCode expected_return_code       = std::get<4>(GetParam());        ///< Expected return code
};

/**
 * @brief Test class for value-parametrized ProcessReceivedMessageTestWithParam function test
 *
 * Parameter order:
 * - Redundancy Channel id (uint32_t)
 * - Message already in buffer (bool)
 * - Add message to buffer (bool)
 * - Set correct crc (bool)
 * - Next expected seq nbr for the receipt (uint32_t)
 * - Next sequence number to be sent (uint32_t)
 * - Seq nbr of the message to add (uint32_t)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class ProcessReceivedMessageTestWithParam :   public redcorTest,
                                              public testing::WithParamInterface<std::tuple<uint32_t, bool, bool, bool, uint32_t, uint32_t, uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t redundancy_channel_id                   = std::get<0>(GetParam());        ///< Redundancy channel id
  bool message_already_in_queue                    = std::get<1>(GetParam());        ///< indicates if a message is already in queue
  bool add_message_to_buffer                       = std::get<2>(GetParam());        ///< indicates if a message will be added to buffer
  bool set_correct_crc                             = std::get<3>(GetParam());        ///< indicates if the crc will be configured correct
  uint32_t rx_seq_nbr                              = std::get<4>(GetParam());        ///< next sequence number expected for the receipt
  uint32_t tx_seq_nbr                              = std::get<5>(GetParam());        ///< next sequence number to be sent
  uint32_t msg_seq_nbr                             = std::get<6>(GetParam());        ///< sequence number of the message to write
  radef_RaStaReturnCode expected_return_code       = std::get<7>(GetParam());        ///< expected return type
};

/**
 * @brief Test class for value-parametrized WriteMessagePayloadToSendBuffer function test
 *
 * Parameter order:
 * - Redundancy channel ID (uint32_t)
 * - Message payload size (uint16_t)
 * - Indicates if a message gets added before (bool)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class WriteMessagePayloadToSendBufferTestWithParam :  public redcorTest,
                                                      public testing::WithParamInterface<std::tuple<uint32_t, uint16_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t redundancy_channel_id                       = std::get<0>(GetParam());        ///< redundancy channel id to clear pending flag
  uint16_t payload_size                                = std::get<1>(GetParam());        ///< message payload size
  bool message_already_in_buffer                       = std::get<2>(GetParam());        ///< add message to buffer before writing payload
  radef_RaStaReturnCode expected_return_code           = std::get<3>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for value-parametrized SendMessage function test
 *
 * Parameter order:
 * - Redundancy channel ID (uint32_t)
 * - Add message to buffer (bool)
 * - Message sequence number (uint32_t)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class SendMessageTestWithParam :  public redcorTest,
                                  public testing::WithParamInterface<std::tuple<uint32_t, bool, uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t redundancy_channel_id                       = std::get<0>(GetParam());        ///< redundancy channel id to clear pending flag
  bool add_message_to_buffer                           = std::get<1>(GetParam());        ///< indicates if a message is added to buffer
  uint32_t message_seq_nbr                             = std::get<2>(GetParam());        ///< message sequence number
  radef_RaStaReturnCode expected_return_code           = std::get<3>(GetParam());        ///< expected return code
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup red_core
 * @{
 */

/**
 * @test        @ID{redcorTest008} Verify the WriteReceivedMessageToInputBuffer function.
 *
 * @details     This test verifies the correct behaviour of the WriteReceivedMessageToInputBuffer function.
 *
 * Test steps:
 * - Init module with default configuration
 * - Verify input buffer values before writing message
 * - Call WriteReceivedMessageToInputBuffer function
 * - Verify message added to input buffer
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Transport channel id: Transport channel identification
 * - Message size: Message size of received message in bytes (valid range: 36 - 1113)
 * - Expected return code: Expected return code when calling function
 * .
 * | Test Run | Input parameter                                           ||| Expected values                             ||
 * |----------|-----------------------|----------------------|--------------|-------------------------|--------------------|
 * |          | Redundancy channel id | Transport channel id | Message size | Expected return code    | Test Result        |
 * | 0        | 0U                    | 0U                   | 36U          | radef_kNoError          | Normal operation   |
 * | 1        | 0U                    | 1U                   | 1113U        | radef_kNoError          | Normal operation   |
 * | 2        | 1U                    | 2U                   | 36U          | radef_kNoError          | Normal operation   |
 * | 3        | 1U                    | 3U                   | 1113U        | radef_kNoError          | Normal operation   |
 * | 4        | 0U                    | 0U                   | 35U          | radef_kInvalidParameter | Fatal error raised |
 * | 5        | 2U                    | 3U                   | 1113U        | radef_kInvalidParameter | Fatal error raised |
 * | 6        | 1U                    | 4U                   | 1113U        | radef_kInvalidParameter | Fatal error raised |
 * | 7        | 1U                    | 3U                   | 1114U        | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(WriteReceivedMessageToInputBufferTestWithParam, redcorTest008VerifyWriteReceivedMessageToInputBuffer)
{
  // copy test message
  for (uint16_t i = 0; i < received_message.message_size; i++)
  {
    received_message.message[i] = test_red_message_data_u8[i];
  }

  // expect calls for configuration validation and init of both redundancy channel
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  if(redundancy_channel_id <= UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX)
  {
    // expect call for transport channel validation when no error thrown at redundancy channel validation
    EXPECT_CALL(reddia_mock, reddia_IsTransportChannelIdValid(redundancy_channel_id, transport_channel_id)).WillOnce(Return(transport_channel_id_valid));
  }

  // init the module
  redcor_Init(&default_config);

  // perform the test
  if(expected_return_code != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_return_code)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_WriteReceivedMessageToInputBuffer(redundancy_channel_id, transport_channel_id, &received_message), std::invalid_argument);
  }
  else
  {
    // Verify input buffer empty before write
    ASSERT_EQ(redcor_redundancy_channels[redundancy_channel_id].input_buffer.message_in_buffer, false);
    ASSERT_EQ(redcor_redundancy_channels[redundancy_channel_id].input_buffer.message_buffer.message_size, 0U);

    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_WriteReceivedMessageToInputBuffer(redundancy_channel_id, transport_channel_id, &received_message));

    // Verify input buffer is updated and contains message
    ASSERT_EQ(redcor_redundancy_channels[redundancy_channel_id].input_buffer.message_in_buffer, true);
    ASSERT_EQ(redcor_redundancy_channels[redundancy_channel_id].input_buffer.message_buffer.message_size, received_message.message_size);
    ASSERT_EQ(redcor_redundancy_channels[redundancy_channel_id].input_buffer.transport_channel_id, transport_channel_id);
    for (uint16_t copy_index = 0U; copy_index < received_message.message_size; ++copy_index)
    {
      ASSERT_EQ(redcor_redundancy_channels[redundancy_channel_id].input_buffer.message_buffer.message[copy_index], received_message.message[copy_index]);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyWriteReceivedMessageToInputBuffer,
  WriteReceivedMessageToInputBufferTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN, true, UT_REDCOR_MSG_LEN_MIN, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), true, UT_REDCOR_MSG_LEN_MAX, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), true, UT_REDCOR_MSG_LEN_MIN, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, true, UT_REDCOR_MSG_LEN_MAX, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN, true, (UT_REDCOR_MSG_LEN_MIN - 1), radef_kInvalidParameter),
    std::make_tuple((UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, false, UT_REDCOR_MSG_LEN_MAX, radef_kInvalidParameter),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX + 1), false, UT_REDCOR_MSG_LEN_MAX, radef_kInvalidParameter),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, true, (UT_REDCOR_MSG_LEN_MAX + 1), radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{redcorTest009} Verify the ProcessReceivedMessage function.
 *
 * @details     This test verifies the correct behaviour of the ProcessReceivedMessage function.
 *
 * Test steps:
 * - Init module with default configuration
 * - Set next sequence number expected for the receipt on the given channel
 * - Call ProcessReceivedMessage function with given test parameter
 * - Set expectations if add message to buffer parameter is set
 * - Return check crc result according set correct crc parameter
 * - Expect update redundancy channel diagnostics if message doesn't gets ignored
 * - Expect message to be added to buffer if its seq nbr is the expected
 * - Expect message to be added to defer queue if out of seq and not already in queue
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Msg already in buffer: Indicates if a message is already in queue
 * - Add msg to buffer: Indicates if a message will be added to buffer
 * - Set correct CRC: Indicates if CheckMessageCrc call passes
 * - Next exp seq nbr: Next expected seq nbr for the receipt
 * - Next seq nbr to be sent: Next sequence number to be sent
 * - Seq nbr of message: Seq nbr of the message to add
 * - Exp return code: Expected return code when calling function
 * .
 * | Test Run | Input parameter       | Test config                                                                                                              |||||| Expected values                              ||
 * |----------|-----------------------|-----------------------|-------------------|-----------------|------------------|-------------------------|--------------------|--------------------------|--------------------|
 * |          | Redundancy channel id | Msg already in buffer | Add msg to buffer | Set correct CRC | Next exp seq nbr | Next seq nbr to be sent | Seq nbr of message | Exp return code          | Test Result        |
 * | 0        | 0U                    | false                 | false             | false           | 0U               | 0U                      | 0U                 | radef_kNoMessageReceived | Fatal error raised |
 * | 1        | 0U                    | false                 | true              | true            | 0U               | 0U                      | 0U                 | radef_kNoError           | Normal operation   |
 * | 2        | 0U                    | false                 | true              | true            | 0U               | 1U                      | 0U                 | radef_kNoError           | Normal operation   |
 * | 3        | 0U                    | false                 | true              | true            | 0U               | 1U                      | 1U                 | radef_kNoError           | Normal operation   |
 * | 4        | 0U                    | false                 | true              | true            | 0U               | 0U                      | 1U                 | radef_kNoError           | Normal operation   |
 * | 5        | 0U                    | false                 | true              | false           | 0U               | 0U                      | 0U                 | radef_kNoError           | Normal operation   |
 * | 6        | 1U                    | false                 | true              | true            | 0U               | 0U                      | 0U                 | radef_kNoError           | Normal operation   |
 * | 7        | 0U                    | false                 | true              | true            | 1U               | 0U                      | 0U                 | radef_kNoError           | Normal operation   |
 * | 8        | 0U                    | false                 | true              | true            | 1U               | 0U                      | 0U                 | radef_kNoError           | Normal operation   |
 * | 9        | 0U                    | false                 | true              | true            | 1U               | 0U                      | 2U                 | radef_kNoError           | Normal operation   |
 * | 10       | 0U                    | false                 | true              | true            | 1U               | 0U                      | 40U                | radef_kNoError           | Normal operation   |
 * | 11       | 0U                    | true                  | true              | true            | 1U               | 0U                      | 40U                | radef_kNoError           | Normal operation   |
 * | 12       | 0U                    | false                 | true              | true            | 1U               | 0U                      | 41U                | radef_kNoError           | Normal operation   |
 * | 13       | 0U                    | false                 | true              | true            | 41U              | 0U                      | 1U                 | radef_kNoError           | Normal operation   |
 * | 14       | 0U                    | false                 | true              | true            | 1U               | 0U                      | 42U                | radef_kNoError           | Normal operation   |
 * | 15       | 2U                    | false                 | false             | false           | 0U               | 0U                      | 0U                 | radef_kInvalidParameter  | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(ProcessReceivedMessageTestWithParam, redcorTest009VerifyProcessReceivedMessage)
{
  // test variables
  uint32_t valid_transport_channel_id       = default_config.redundancy_channel_configurations[redundancy_channel_id].transport_channel_ids[0];
  redtyp_RedundancyMessage message          = {UT_REDCOR_MSG_LEN_MAX, {}};

  // set message seq nbr
  setDataInBuffer_u32(&message.message, msg_seq_nbr, expected_sequence_nbr_pos);

  // expect calls for configuration validation and init of both redundancy channel
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  // init the module
  redcor_Init(&default_config);

  if(redundancy_channel_id <= UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX)
  {
    // set next expected sequence numbers
    redcor_redundancy_channels[redundancy_channel_id].seq_rx = rx_seq_nbr;
    redcor_redundancy_channels[redundancy_channel_id].seq_tx = tx_seq_nbr;
  }

  // set expectations
  if(add_message_to_buffer)
  {
    EXPECT_CALL(reddia_mock, reddia_IsTransportChannelIdValid(redundancy_channel_id, valid_transport_channel_id)).WillOnce(Return(true));
    EXPECT_NO_THROW(redcor_WriteReceivedMessageToInputBuffer(redundancy_channel_id, valid_transport_channel_id, &message));
    EXPECT_CALL(reddfq_mock, reddfq_IsSequenceNumberOlder(_ ,_)).WillRepeatedly(Invoke(this, &redcorTest::IsSequenceNumberOlder));

    if(set_correct_crc)
    {
      EXPECT_CALL(redmsg_mock, redmsg_CheckMessageCrc(_)).Times(1).WillOnce(Return(radef_kNoError));
      EXPECT_CALL(redmsg_mock, redmsg_GetMessageSequenceNumber(_)).WillRepeatedly(Return(msg_seq_nbr));

      if(!((rx_seq_nbr == 0U) && (tx_seq_nbr == 0U) &&(msg_seq_nbr != 0U)))
      {

        // Explanation: For messages with Seq_pdu > (Seq_rx + 40), ignore the message without update of the redundancy channel diagnostics
        if(msg_seq_nbr <= (rx_seq_nbr + 10 * default_config.n_defer_queue_size))
        {
          EXPECT_CALL(reddia_mock, reddia_UpdateRedundancyChannelDiagnostics(_, _, _)).Times(1);
        }

        // Explanation: For messages with Seq_rx < Seq_pdu <= (Seq_rx +40), add message to the defer queue, if not already in the queue
        if(rx_seq_nbr == msg_seq_nbr)
        {
          EXPECT_CALL(redmsg_mock, redmsg_GetMessagePayload(_, _)).Times(1);
          EXPECT_CALL(redrbf_mock, redrbf_AddToBuffer(redundancy_channel_id, _)).Times(1);
          EXPECT_CALL(reddfq_mock, reddfq_Contains(redundancy_channel_id, _));
        }
        else if (msg_seq_nbr > rx_seq_nbr && !(msg_seq_nbr > (rx_seq_nbr + 10 * default_config.n_defer_queue_size)))
        {
          if(message_already_in_queue)
          {
            EXPECT_CALL(reddfq_mock, reddfq_Contains(redundancy_channel_id, _)).Times(1).WillOnce(Return(true));
          }
          else
          {
            EXPECT_CALL(reddfq_mock, reddfq_Contains(redundancy_channel_id, _)).Times(1).WillOnce(Return(false));
            EXPECT_CALL(reddfq_mock, reddfq_AddMessage(redundancy_channel_id, _)).Times(1);
          }
        }
      }
    }
    else
    {
      EXPECT_CALL(redmsg_mock, redmsg_CheckMessageCrc(_)).Times(1).WillOnce(Return(radef_kInvalidMessageCrc));
    }
  }

  // perform the test
  if(expected_return_code != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_return_code)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_ProcessReceivedMessage(redundancy_channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_ProcessReceivedMessage(redundancy_channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyProcessReceivedMessage,
  ProcessReceivedMessageTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, false, false, 0, 0, 0, radef_kNoMessageReceived),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 0, 0, 0, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 0, 1, 0, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 0, 1, 1, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 0, 0, 1, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, false, 0, 0, 0, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, false, true, true, 0, 0, 0, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 1, 0, 0, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 1, 0, 0, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 1, 0, 2, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 1, 0, (default_config.n_defer_queue_size * 10), radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, true, true, true, 1, 0, (default_config.n_defer_queue_size * 10), radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 1, 0, (default_config.n_defer_queue_size * 10 + 1), radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 41, 0, 1, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, true, true, 1, 0, 42, radef_kNoError),
    std::make_tuple((UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), false, false, false, 0, 0, 0, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{redcorTest013} Verify the WriteMessagePayloadToSendBuffer function.
 *
 * @details     This test verifies the correct behaviour of the WriteMessagePayloadToSendBuffer function.
 *
 * Test steps:
 * - Init module with default config
 * - Set message_in_buffer to true if parameter given
 * - Call WriteMessagePayloadToSendBuffer function for given redundancy channel
 * - Verify message content in buffer and message_in_buffer flag set to true
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Message payload size: Message payload size in bytes (valid range: 28 - 1101)
 * - Message already in buffer: Indicates if message gets added to buffer before writing payload
 * - Expected return code: Expected return code when calling function
 * .
 * | Test Run | Input parameter                             || Test config               | Expected values                             ||
 * |----------|-----------------------|----------------------|---------------------------|-------------------------|--------------------|
 * |          | Redundancy channel id | Message payload size | Message already in buffer | Expected return code    | Test Result        |
 * | 0        | 0U                    | 28U                  | false                     | radef_kNoError          | Normal operation   |
 * | 1        | 1U                    | 1101U                | false                     | radef_kNoError          | Normal operation   |
 * | 2        | 0U                    | 27U                  | false                     | radef_kInvalidParameter | Fatal error raised |
 * | 3        | 2U                    | 1101U                | false                     | radef_kInvalidParameter | Fatal error raised |
 * | 4        | 1U                    | 1102U                | false                     | radef_kInvalidParameter | Fatal error raised |
 * | 5        | 0U                    | 28U                  | true                      | radef_kSendBufferFull   | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(WriteMessagePayloadToSendBufferTestWithParam, redcorTest013VerifyWriteMessagePayloadToSendBuffer)
{
  uint8_t payload[UT_REDCOR_DATA_LEN_MAX] = {};

  // expect calls for configuration validation and init of both redundancy channel
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  // copy test payload
  for (uint16_t i = 0U; i < payload_size; i++)
  {
    payload[i] = test_red_payload_data_u8[i];
  }

  // init the module
  redcor_Init(&default_config);

  if(expected_return_code!= radef_kInvalidParameter)
  {
    if(message_already_in_buffer)
    {
      redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_in_buffer = true;
    }
    else
    {
      EXPECT_FALSE(redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_in_buffer);
    }
  }

  // perform the test
  if(expected_return_code != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_return_code)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_WriteMessagePayloadToSendBuffer(redundancy_channel_id, payload_size, payload), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_WriteMessagePayloadToSendBuffer(redundancy_channel_id, payload_size, payload));

    // verify the payload content
    for (uint16_t index = 0U; index < payload_size; ++index)
    {
      EXPECT_EQ(redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_payload_buffer.payload[index], test_red_payload_data_u8[index]);
    }

    // verify the message in buffer flag is set
    EXPECT_TRUE(redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_in_buffer);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyWriteMessagePayloadToSendBuffer,
  WriteMessagePayloadToSendBufferTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, UT_REDCOR_DATA_LEN_MIN, false, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, UT_REDCOR_DATA_LEN_MAX, false, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, (UT_REDCOR_DATA_LEN_MIN - 1), false, radef_kInvalidParameter),
    std::make_tuple((UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), UT_REDCOR_DATA_LEN_MAX, false, radef_kInvalidParameter),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, (UT_REDCOR_DATA_LEN_MAX + 1), false, radef_kInvalidParameter),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, UT_REDCOR_DATA_LEN_MIN, true, radef_kSendBufferFull)
  )
);

/**
 * @test        @ID{redcorTest015} Verify the SendMessage function.
 *
 * @details     This test verifies the correct behaviour of the SendMessage function.
 *
 * Test steps:
 * - Init module with given config for transport channel ids
 * - Add message to buffer if parameter given
 * - Call SendMessage function with given test parameter
 * - Verify message sent and next seq nbr to be sent
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Add message to buffer: Indicates if a message is added to buffer
 * - Message seq nbr: Seq nbr of message to verify next seq nbr to send
 * - Expected return code: Expected return code when calling function
 * .
 * | Test Run | Input parameter       | Test config                            || Expected values                             ||
 * |----------|-----------------------|-----------------------|-----------------|-------------------------|--------------------|
 * |          | Redundancy channel id | Add message to buffer | Message seq nbr | Expected return code    | Test Result        |
 * | 0        | 0U                    | true                  | 0U              | radef_kNoError          | Normal operation   |
 * | 1        | 1U                    | true                  | 0xffffffffU     | radef_kNoError          | Normal operation   |
 * | 2        | 2U                    | false                 | 0U              | radef_kInvalidParameter | Fatal error raised |
 * | 3        | 0U                    | false                 | 0U              | radef_kNoMessageToSend  | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(SendMessageTestWithParam, redcorTest015VerifySendMessage)
{
  // test variables
  uint16_t payload_size                         = UT_REDCOR_DATA_LEN_MAX;
  uint8_t payload                               = 0;
  uint32_t expected_next_seq_nbr_to_be_sent     = message_seq_nbr + 1;
  redtyp_RedundancyMessage message_to_send      = {UT_REDCOR_MSG_LEN_MAX, {}};

  // copy test message
  for (uint16_t i = 0; i < UT_REDCOR_MSG_LEN_MAX; i++)
  {
    message_to_send.message[i] = test_red_message_data_u8[i];
  }

  // seq nbr 0 only used on first send
  if(message_seq_nbr == UINT32_MAX)
  {
    expected_next_seq_nbr_to_be_sent = 1;
  }

  // expect calls for configuration validation and init of both redundancy channel
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  // init the module
  redcor_Init(&default_config);

  if(expected_return_code != radef_kInvalidParameter)
  {
    if(add_message_to_buffer)
    {
      EXPECT_NO_THROW(redcor_WriteMessagePayloadToSendBuffer(redundancy_channel_id, payload_size, &payload));
      EXPECT_TRUE(redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_in_buffer);
    }
    else
    {
      EXPECT_FALSE(redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_in_buffer);
    }
  }

  // perform the test
  if(expected_return_code != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_return_code)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_SendMessage(redundancy_channel_id), std::invalid_argument);
  }
  else
  {
    // Send message on each transport channel
    EXPECT_CALL(redmsg_mock, redmsg_CreateMessage(_, _, _)).WillOnce(SetArgPointee<2>(message_to_send));
    EXPECT_CALL(redtri_mock, redtri_SendMessage(_, _, _)).Times(2).WillRepeatedly(Invoke(this, &redcorTest::VerifySendMessageData));
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    EXPECT_NO_THROW(redcor_SendMessage(redundancy_channel_id));

    EXPECT_FALSE(redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_in_buffer);
    EXPECT_EQ(redcor_redundancy_channels[redundancy_channel_id].seq_tx, expected_next_seq_nbr_to_be_sent);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendMessage,
  SendMessageTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, true, 0, radef_kNoError),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, true, UINT32_MAX, radef_kNoError),
    std::make_tuple((UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), false, 0, radef_kInvalidParameter),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, 0, radef_kNoMessageToSend)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
