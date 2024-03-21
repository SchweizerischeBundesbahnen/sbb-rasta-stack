/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srapi_timings.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the api module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srapi.hh"

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
 * @brief Test class for value-parametrized CheckTimings function test
 *
 * Parameter order:
 * - ID0: number of available messages (uint8_t)
 * - ID0: number of free entries in the read buffer (uint8_t)
 * - ID0: number of free entries in the send buffer (uint8_t)
 * - ID0: is a message timeout occurred (bool)
 * - ID0: number of messages to send (uint8_t)
 * - ID0: is the heartbeat interval elapsed (bool)
 * - ID1: number of available messages (uint8_t)
 * - ID1: number of free entries in the read buffer (uint8_t)
 * - ID1: number of free entries in the send buffer (uint8_t)
 * - ID1: is a message timeout occurred (bool)
 * - ID1: number of messages to send (uint8_t)
 * - ID1: is the heartbeat interval elapsed (bool)
 * - connection event (srtyp_ConnectionEvents)
 * - is sequence number in sequence (bool)
 * - is confirmed timestamp in sequence (bool)
 * .
 */
class CheckTimingsTestWithParam : public srapiTest,
                                  public testing::WithParamInterface<std::tuple<uint8_t, uint8_t, uint8_t, bool, uint8_t, bool, uint8_t, uint8_t, uint8_t, bool, uint8_t, bool, srtyp_ConnectionEvents, bool, bool>>
{
  public:
  uint8_t id0_msg_available =         std::get<0>(GetParam());        ///< ID0: number of messages available to read
  uint8_t id0_rbuf_free =             std::get<1>(GetParam());        ///< ID0: number of free entries in the receive buffer
  uint8_t id0_sbuf_free =             std::get<2>(GetParam());        ///< ID0: number of free entries in the send buffer
  bool    id0_is_msg_timeout =        std::get<3>(GetParam());        ///< ID0: return value for the srcor_IsMessageTimeout function
  uint8_t id0_n_msg_to_send =         std::get<4>(GetParam());        ///< ID0: number of messages to send
  bool    id0_is_hb_interval =        std::get<5>(GetParam());        ///< ID0: return value for the srcor_IsHeartbeatInterval function

  uint8_t id1_msg_available =         std::get<6>(GetParam());        ///< ID1: number of messages available to read
  uint8_t id1_rbuf_free =             std::get<7>(GetParam());        ///< ID1: number of free entries in the receive buffer
  uint8_t id1_sbuf_free =             std::get<8>(GetParam());        ///< ID1: number of free entries in the send buffer
  bool    id1_is_msg_timeout =        std::get<9>(GetParam());        ///< ID1: return value for the srcor_IsMessageTimeout function
  uint8_t id1_n_msg_to_send =         std::get<10>(GetParam());       ///< ID1: number of messages to send
  bool    id1_is_hb_interval =        std::get<11>(GetParam());       ///< ID1: return value for the srcor_IsHeartbeatInterval function

  srtyp_ConnectionEvents conn_event = std::get<12>(GetParam());       ///< connection event
  bool sn_in_seq =                    std::get<13>(GetParam());       ///< sequence number in sequence
  bool cts_in_seq =                   std::get<14>(GetParam());       ///< confirmed timestamp number in sequence
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_api
 * @{
 */

/**
 * @test        @ID{srapiTest011} Verify the CheckTimings function of the srapi module
 *
 * @details     This test verifies the behaviour of the CheckTimings function.
 *
 * Test steps:
 * - create a SafetyRetransmissionConfiguration and set maximum number of connections (2) and connection IDs 0 and 1
 * - initialize the module with this configuration
 * - calculate the number of reads from given test parameter (smallest of available messages and free read buffer elements)
 * - set the number of reads to zero if the number of free send buffer elements is smaller than 3
 * - expect the GetReceivedMessagePendingFlag function to be called, return true if messages are available and decrement the number of messages afterwards
 * - expect the srrece_GetFreeBufferEntries function to be called, return the number of free elements according to the test parameter and decrement afterwards
 * - expect the srsend_GetFreeBufferEntries function to be called and return the number of free elements according to the test parameter
 * - set the mock function call expectation on the ReceiveMessage function and set ConnectionEvent, sn_in_seq and cts_in seq according to the test parameter
 * - set the mock function call expectation on the ProcessConnectionStateMachine and verify ConnectionEvent, sn_in_seq and cts_in seq
 * - set the mock function call expectation on the SendPendingMessages function and verify the data
 * - set the mock function call expectation on the IsMessageTimeout function and return true or false according to the test parameter
 * - expect the ProcessConnectionStateMachine function to be called with event set to Timeout if isMessageTimeout is true
 * - set the mock function call expectations on the GetNumberOfMessagesToSend and IsHeartbeatInterval functions and return the values according to the test parameter
 * - expect the ProcessConnectionStateMachine function to be called if isHbInterval is true and no messages are to be sent
 * - call the CheckTimings function and verify the return value
 * .
 *
 *  @testParameter
 * - ID0 Msg: number of messages available to read on connection 0
 * - RBuf0 free: number of free entries in the receive buffer on connection 0
 * - SBuf0 free: number of free entries in the send buffer on connection 0
 * - ID0 isMsgTO: return value for the srcor_IsMessageTimeout function on connection 0
 * - ID0 MsgToSend: number of messages to send on connection 0
 * - ID0 isHbInt: return value for the srcor_IsHeartbeatInterval function on connection 0
 * - ID1 Msg: number of messages available to read on connection 1
 * - RBuf1 free: number of free entries in the receive buffer on connection 1
 * - SBuf1 free: number of free entries in the send buffer on connection 1
 * - ID1 isMsgTO: return value for the srcor_IsMessageTimeout function on connection 1
 * - ID1 MsgToSend: number of messages to send on connection 1
 * - ID1 isHbInt: return value for the srcor_IsHeartbeatInterval function on connection 1
 * - Conn. Event: Connection event
 * - SN in Seq.: Indicates if sequence number in sequence
 * - CTS is Seq.: Indicates if confirmed timestamp number in sequence
 * .
 * | Test Run | Test config                                                                                                                                                                              ||||||||||||||| Expected values  |
 * |----------|---------|------------|------------|-------------|---------------|-------------|---------|------------|------------|-------------|---------------|-------------|-------------|------------|-------------|------------------|
 * |          | ID0 Msg | RBuf0 free | SBuf0 free | ID0 isMsgTO | ID0 MsgToSend | ID0 isHbInt | ID1 Msg | RBuf1 free | SBuf1 free | ID1 isMsgTO | ID1 MsgToSend | ID1 isHbInt | Conn. Event | SN in Seq. | CTS is Seq. | Test Result      |
 * | 0        | 3       | 3          | 3          | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 1        | 3       | 2          | 3          | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 2        | 3       | 1          | 3          | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 3        | 3       | 0          | 3          | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 4        | 3       | 3          | 2          | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 5        | 1       | 20         | 20         | true        | 0             | false       | 5       | 3          | 3          | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 6        | 1       | 20         | 20         | true        | 0             | false       | 5       | 2          | 3          | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 7        | 1       | 20         | 20         | true        | 0             | false       | 5       | 1          | 3          | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 8        | 1       | 20         | 20         | true        | 0             | false       | 5       | 0          | 3          | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 9        | 1       | 20         | 20         | true        | 0             | false       | 5       | 3          | 2          | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 10       | 1       | 20         | 20         | false       | 1             | true        | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 11       | 1       | 20         | 20         | false       | 1             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 12       | 1       | 20         | 20         | false       | 0             | true        | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 13       | 1       | 20         | 20         | false       | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 14       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | false       | 1             | true        | Open        | false      | false       | Normal Operation |
 * | 15       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | false       | 1             | false       | Open        | false      | false       | Normal Operation |
 * | 16       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | false       | 0             | true        | Open        | false      | false       | Normal Operation |
 * | 17       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | false       | 0             | false       | Open        | false      | false       | Normal Operation |
 * | 18       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | true       | false       | Normal Operation |
 * | 19       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | false      | true        | Normal Operation |
 * | 20       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Open        | true       | true        | Normal Operation |
 * | 21       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | Timeout     | false      | false       | Normal Operation |
 * | 22       | 1       | 20         | 20         | true        | 0             | false       | 1       | 20         | 20         | true        | 0             | false       | None        | false      | false       | Normal Operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-551} Check Timings Function
 * @verifyReq{RASW-319} Check Timings Function Structure
 * @verifyReq{RASW-317} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(CheckTimingsTestWithParam, srapiTest011VerifyCheckTimingsFunction) {
  // create fake configuration
  const srcty_SafetyRetransmissionConfiguration config = {
    0,
    0,
    0,
    srcty_kSafetyCodeTypeLowerMd4,
    0,
    0,
    0,
    0,
    UT_SRAPI_MAX_NUM_CONN,
    {{UT_SRAPI_CONN_IDX_0, 0, 0}, {UT_SRAPI_CONN_IDX_1, 0, 0}}, // two connections, ID=0 and ID=1
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  // initialize the module
  EXPECT_CALL(srcor_mock, srcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(srstm_mock, srstm_Init(_)).Times(1);
  EXPECT_CALL(srcor_mock, srcor_Init(_)).Times(1);
  EXPECT_EQ(radef_kNoError, srapi_Init(&config));

  // number of rounds (get minimum of available messages and free read buffer space)
  uint8_t rounds_id0 = std::min(id0_msg_available, id0_rbuf_free);
  uint8_t rounds_id1 = std::min(id1_msg_available, id1_rbuf_free);
  // send buffer needs to have at least 3 free elements, otherwise no read operation can be performed
  if (id0_sbuf_free < 3) {
    rounds_id0 = 0;
  }
  if (id1_sbuf_free < 3) {
    rounds_id1 = 0;
  }

  // sequence for conn1 while loop
  {
    InSequence s1;
    if(rounds_id0 >= 1) {
      EXPECT_CALL(srcor_mock, srcor_IsReceivedMsgPendingAndBuffersNotFull(UT_SRAPI_CONN_IDX_0)).Times(rounds_id0).WillRepeatedly(Return(true));
    }
    EXPECT_CALL(srcor_mock, srcor_IsReceivedMsgPendingAndBuffersNotFull(UT_SRAPI_CONN_IDX_0)).WillOnce(Return(false));
  }
  // sequence for conn2 while loop
  {
    InSequence s2;
    if(rounds_id1 >= 1) {
      EXPECT_CALL(srcor_mock, srcor_IsReceivedMsgPendingAndBuffersNotFull(UT_SRAPI_CONN_IDX_1)).Times(rounds_id1).WillRepeatedly(Return(true));
    }
    EXPECT_CALL(srcor_mock, srcor_IsReceivedMsgPendingAndBuffersNotFull(UT_SRAPI_CONN_IDX_1)).WillOnce(Return(false));
  }

  // expect calls for ReceiveMessage and ProcessConnectionStateMachine (ID0)
  if (rounds_id0 > 0) {
    EXPECT_CALL(srcor_mock, srcor_ReceiveMessage(UT_SRAPI_CONN_IDX_0, _, _, _)).Times(rounds_id0)
      .WillRepeatedly(DoAll(SetArgPointee<1>(conn_event), SetArgPointee<2>(sn_in_seq), SetArgPointee<3>(cts_in_seq)));
    // if messages are received and the connection event is different from ConnEventNone, process the state machine
    if(conn_event != srtyp_kConnEventNone) {
    EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(UT_SRAPI_CONN_IDX_0, conn_event, sn_in_seq, cts_in_seq))
      .Times(rounds_id0)
      .RetiresOnSaturation();
    }
  }
  // expect calls for ReceiveMessage and ProcessConnectionStateMachine (ID1)
  if (rounds_id1 > 0) {
    EXPECT_CALL(srcor_mock, srcor_ReceiveMessage(UT_SRAPI_CONN_IDX_1, _, _, _)).Times(rounds_id1)
      .WillRepeatedly(DoAll(SetArgPointee<1>(conn_event), SetArgPointee<2>(sn_in_seq), SetArgPointee<3>(cts_in_seq)));
    // if messages are received and the connection event is different from ConnEventNone, process the state machine
    if(conn_event != srtyp_kConnEventNone) {
      EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(UT_SRAPI_CONN_IDX_1, conn_event, sn_in_seq, cts_in_seq))
        .Times(rounds_id1)
        .RetiresOnSaturation();
    }
  }

  // expect calls for SendPendingMessages
  EXPECT_CALL(srcor_mock, srcor_SendPendingMessages(UT_SRAPI_CONN_IDX_0)).Times(1);
  EXPECT_CALL(srcor_mock, srcor_SendPendingMessages(UT_SRAPI_CONN_IDX_1)).Times(1);

  // expect calls for IsMessageTimeout (ID0)
  EXPECT_CALL(srcor_mock, srcor_IsMessageTimeout(UT_SRAPI_CONN_IDX_0)).WillOnce(Return(id0_is_msg_timeout));
  // if a message timeout occurred, handle it
  if (id0_is_msg_timeout) {
    EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(UT_SRAPI_CONN_IDX_0, srtyp_kConnEventTimeout, false, false))
      .Times(1)
      .RetiresOnSaturation();
  }
  // if no message timeout occurred, check the heartbeat timeout
  else {
    // expect calls for GetNumberOfMessagesToSend and IsHeartbeatInterval
    EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(UT_SRAPI_CONN_IDX_0)).WillOnce(Return(id0_n_msg_to_send));
    EXPECT_CALL(srcor_mock, srcor_IsHeartbeatInterval(UT_SRAPI_CONN_IDX_0)).WillOnce(Return(id0_is_hb_interval));
    // if the heartbeat interval elapsed and no message waiting to be sent, send a heartbeat
    if(id0_is_hb_interval && (id0_n_msg_to_send == 0)) {
      EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(UT_SRAPI_CONN_IDX_0, srtyp_kConnEventSendHb, false, false))
        .Times(1)
        .RetiresOnSaturation();
    }
  }

  // expect calls for IsMessageTimeout (ID1)
  EXPECT_CALL(srcor_mock, srcor_IsMessageTimeout(UT_SRAPI_CONN_IDX_1)).WillOnce(Return(id1_is_msg_timeout));
  // if a message timeout occurred, handle it
  if (id1_is_msg_timeout) {
    EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(UT_SRAPI_CONN_IDX_1, srtyp_kConnEventTimeout, false, false))
      .Times(1)
      .RetiresOnSaturation();
  }
  // if no message timeout occurred, check the heartbeat timeout
  else {
    // expect calls for GetNumberOfMessagesToSend and IsHeartbeatInterval
    EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(UT_SRAPI_CONN_IDX_1)).WillOnce(Return(id1_n_msg_to_send));
    EXPECT_CALL(srcor_mock, srcor_IsHeartbeatInterval(UT_SRAPI_CONN_IDX_1)).WillOnce(Return(id1_is_hb_interval));
    // if the heartbeat interval elapsed and no message waiting to be sent, send a heartbeat
    if(id1_is_hb_interval && (id1_n_msg_to_send == 0)) {
      EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(UT_SRAPI_CONN_IDX_1, srtyp_kConnEventSendHb, false, false))
        .Times(1)
        .RetiresOnSaturation();
    }
  }

  // call the CheckTimings function and verify the return value
  EXPECT_EQ(radef_kNoError, srapi_CheckTimings());
}

INSTANTIATE_TEST_SUITE_P(
  verifyCheckTimingsFunction,
  CheckTimingsTestWithParam,
  ::testing::Values(
        std::make_tuple(3, 3, 3, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(3, 2, 3, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(3, 1, 3, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(3, 0, 3, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(3, 3, 2, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),

        std::make_tuple(1, 20, 20, true, 0, false, 5, 3, 3, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, true, 0, false, 5, 2, 3, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, true, 0, false, 5, 1, 3, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, true, 0, false, 5, 0, 3, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, true, 0, false, 5, 3, 2, true, 0, false, srtyp_kConnEventOpen, false, false),

        std::make_tuple(1, 20, 20, false, 1, true, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, false, 1, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, false, 0, true, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, false, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, false),

        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, false, 1, true, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, false, 1, false, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, false, 0, true, srtyp_kConnEventOpen, false, false),
        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, false, 0, false, srtyp_kConnEventOpen, false, false),

        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, true, false),
        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, false, true),
        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventOpen, true, true),

        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventTimeout, false, false),
        std::make_tuple(1, 20, 20, true, 0, false, 1, 20, 20, true, 0, false, srtyp_kConnEventNone, false, false)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
