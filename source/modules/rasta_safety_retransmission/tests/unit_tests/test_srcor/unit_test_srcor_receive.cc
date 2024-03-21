/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srcor_receive.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version be8366f390fb790e46303e9eb6cf4fb9357a5659
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4958: added connection id check before configuring the test,12.12.2022,N. Fricker}
 *
 * @brief Unit test file for the receive message relevant functions from the core module of the safety and retransmission layer.
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
 * @brief Test class for value-parametrized ReceiveMessage tests
 *
 * Parameter order:
 * - configured connections (uint32_t)
 * - connection id (uint32_t)
 * - return value of read message call (radef_RaStaReturnCode)
 * - return value of check message call (radef_RaStaReturnCode)
 * - message type (uint16_t)
 * - configured expected next receive seq number (uint32_t)
 * - configured confirmed timestamp rx (uint32_t)
 * - configured confirmed sequence number rx (uint32_t)
 * - configured sequence number tx (uint32_t)
 * - message sequence number (uint32_t)
 * - message timestamp (uint32_t)
 * - message confirmed timestamp (uint32_t)
 * - message confirmed sequence number (uint32_t)
 * - message sender id (uint32_t)
 * - message receiver id (uint32_t)
 * - expect confirmed timestamp to be in sequence (bool)
 * - expected connection event (srtyp_ConnectionEvents)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class ReceiveMessageTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t,
  uint32_t, radef_RaStaReturnCode, radef_RaStaReturnCode, uint16_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, srtyp_ConnectionEvents, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  radef_RaStaReturnCode read_msg_return_value =           std::get<2>(GetParam());    ///< return value of read message call
  radef_RaStaReturnCode check_msg_return_value =          std::get<3>(GetParam());    ///< return value of check message call
  uint16_t message_type =                                 std::get<4>(GetParam());    ///< message type
  uint32_t configured_seq_nbr_rx =                        std::get<5>(GetParam());    ///< configured expected next receive seq number
  uint32_t configured_confirmed_timestamp_rx =            std::get<6>(GetParam());    ///< configured confirmed timestamp rx
  uint32_t configured_confirmed_seq_nr_rx =               std::get<7>(GetParam());    ///< configured confirmed sequence number rx
  uint32_t configured_seq_number_tx =                     std::get<8>(GetParam());    ///< configured sequence number tx
  uint32_t message_seq_nbr =                              std::get<9>(GetParam());    ///< message sequence number
  uint32_t message_timestamp =                            std::get<10>(GetParam());   ///< message timestamp
  uint32_t message_confirmed_timestamp =                  std::get<11>(GetParam());   ///< message confirmed timestamp
  uint32_t message_confirmed_seq_nbr =                    std::get<12>(GetParam());   ///< message confirmed sequence number
  uint32_t message_sender_id =                            std::get<13>(GetParam());   ///< message sender id
  uint32_t message_receiver_id =                          std::get<14>(GetParam());   ///< message receiver id
  bool expect_conf_timestamp_in_seq =                     std::get<15>(GetParam());   ///< expect confirmed timestamp to be in sequence
  srtyp_ConnectionEvents expected_connection_event =      std::get<16>(GetParam());   ///< expected connection event
  radef_RaStaReturnCode expected_error =                  std::get<17>(GetParam());   ///< expected error
};

/**
 * @brief Test class for value-parametrized HandleRetrReq tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class HandleRetrReqTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  bool expect_fatal_error =                               std::get<2>(GetParam());    ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized ProcessReceivedMessage tests
 *
 * Parameter order:
 * - configured connections (uint32_t)
 * - connection id (uint32_t)
 * - message in buffer (bool)
 * - configured confirmed seq nr rx (uint32_t)
 * - configured last send confirmed seq nr tx (uint32_t)
 * - configured timer ti value (uint32_t)
 * - message type (srtyp_SrMessageType)
 * - message sequence number (uint32_t)
 * - message confirmed sequence number (uint32_t)
 * - message confirmed timestamp (uint32_t)
 * - timer value (uint32_t)
 * - return value of send module get nbr of msgs to send (uint32_t)
 * - expect timeliness respected (bool)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class ProcessReceivedMessageTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, uint32_t, uint32_t, uint32_t, srtyp_SrMessageType, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                    std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                             std::get<1>(GetParam());    ///< connection id
  bool message_in_buffer =                             std::get<2>(GetParam());    ///< message in buffer
  uint32_t configured_confirmed_seq_nr_rx =            std::get<3>(GetParam());    ///< configured confirmed seq nr rx
  uint32_t configured_last_send_confirmed_seq_nr_tx =  std::get<4>(GetParam());    ///< configured last send confirmed seq nr tx
  uint32_t configured_timer_ti_value =                 std::get<5>(GetParam());    ///< configured timer ti value
  srtyp_SrMessageType message_type =                   std::get<6>(GetParam());    ///< message type
  uint32_t message_seq_nr =                            std::get<7>(GetParam());    ///< message sequence number
  uint32_t message_confirmed_seq_nr =                  std::get<8>(GetParam());    ///< message confirmed sequence number
  uint32_t message_confirmed_ts =                      std::get<9>(GetParam());    ///< message confirmed timestamp
  uint32_t timer_value =                               std::get<10>(GetParam());   ///< timer value
  uint32_t msgs_to_send_return_value =                 std::get<11>(GetParam());   ///< return value of send module get nbr of msgs to send
  bool expect_timeliness_respected =                   std::get<12>(GetParam());   ///< expect timeliness respected (return value)
  radef_RaStaReturnCode expected_error =               std::get<13>(GetParam());   ///< expected error
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_core
 * @{
 */

/**
 * @test        @ID{srcorTest030} Verify the ReceiveMessage function
 *
 * @details     This test verifies the ReceiveMessage function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Set expectations if check are going to pass or not
 * - Create a message according to test parameter
 * - Initialize the module
 * - Set connection data to given test parameter
 * - Call the function and verify each of these values
 *   - Connection event pointer value given by function (according test parameter)
 *   - Expect seq nbr in seq pointer value
 *   - Confirmed timestamp in seq pointer value (according test parameter)
 *   - Message in buffer property: true when checks passed, false when not passed
 *   - Received data pending property set to false when read message return value noMessageReceived
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Conf connections: Number of configured connections
 * - Read msg return: Return value of sradin_ReadMessage function call
 * - Check msg return: Return value of srmsg_CheckMessage function call
 * - Message type: Message type of message which gets received
 * - Seq nr rx: Configured next receive seq number on connection
 * - Confirmed ts rx: Configured confirmed timestamp received on connection
 * - Confirmed seq nr rx: Configured confirmed sequence number received on connection
 * - Seq nr tx: Configured next sequence to send on connection
 * - Seq nr: Message sequence number
 * - Timestamp: Message timestamp
 * - Confirmed ts: Message confirmed timestamp
 * - Confirmed seq nr: Message confirmed sequence number
 * - Sender id: Message sender id
 * - Receiver id: Message receiver id
 * - Timestamp in seq: Indicates if timestamp received in sequence
 * - Connection event: Expected raised connection event
 * - Expected error code: Expected error code when calling srcor_ReceiveMessage function
 * .
 * | Test Run | Input parameter | Test config                                                                                                                                                                                                      |||||||||||||| Expected values                                                          ||||
 * |----------|-----------------|------------------|-------------------|--------------------|--------------|-------------|-----------------|---------------------|-----------|--------|-------------|--------------|------------------|-----------|-------------|------------------|------------------|------------------|--------------------|
 * |          |                 |                  |                   |                    |              | Rasta connection configuration                               |||| Message configuration                                                       ||||||                  |                  |                  | Test Result        |
 * |          | Connection Id   | Conf connections | Read msg return   | Check msg return   | Message type | Seq nr rx   | Confirmed ts rx | Confirmed seq nr rx | Seq nr tx | Seq nr | Timestamp   | Confirmed ts | Confirmed seq nr | Sender id | Receiver id | Timestamp in seq | Connection event | Error code       |                    |
 * |          | vary read message return value                                                                                                                                                                                                                                                          ||||||||||||||||||                    |
 * | 0        | 0               | 1                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | ConnReqReceived  | NoError          | Normal operation   |
 * | 1        | 0               | 1                | NoMessageReceived | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 2        | 0               | 1                | InternalError     | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | kInternalError   | Fatal error raised |
 * |          | vary check message return values                                                                                                                                                                                                                                                        ||||||||||||||||||                    |
 * | 3        | 0               | 1                | NoError           | InvalidMessageMd4  | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 4        | 0               | 1                | NoError           | InvalidMessageType | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 5        | 0               | 1                | NoError           | InvalidMessageSize | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 6        | 0               | 1                | NoError           | InternalError      | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | InternalError    | Fatal error raised |
 * |          | vary message types                                                                                                                                                                                                                                                                      ||||||||||||||||||                    |
 * | 7        | 0               | 1                | NoError           | NoError            | ConnResp     | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 8        | 0               | 1                | NoError           | NoError            | RetrReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | RetrReqReceived  | NoError          | Normal operation   |
 * | 9        | 0               | 1                | NoError           | NoError            | RetrResp     | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | RetrRespReceived | NoError          | Normal operation   |
 * | 10       | 0               | 1                | NoError           | NoError            | DiscReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | DiscReqReceived  | NoError          | Normal operation   |
 * | 11       | 0               | 1                | NoError           | NoError            | Hb           | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | HbReceived       | NoError          | Normal operation   |
 * | 12       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 13       | 0               | 1                | NoError           | NoError            | RetrData     | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | RetrDataReceived | NoError          | Normal operation   |
 * |          | vary sender and receiver id                                                                                                                                                                                                                                                             ||||||||||||||||||                    |
 * | 14       | 0               | 1                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 97        | 97          | false            | None             | NoError          | Normal operation   |
 * | 15       | 0               | 1                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 98          | false            | None             | NoError          | Normal operation   |
 * | 16       | 0               | 1                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 97        | 98          | false            | None             | NoError          | Normal operation   |
 * |          | vary message seq nbr                                                                                                                                                                                                                                                                    ||||||||||||||||||                    |
 * | 17       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 0                   | 0         | 1      | 0           | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 18       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 0                   | 0         | 200    | 0           | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 19       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 0                   | 0         | 201    | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 20       | 0               | 1                | NoError           | NoError            | Data         | 1           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 21       | 0               | 1                | NoError           | NoError            | Data         | 0xffffffff  | 0               | 0                   | 0         | 199    | 0           | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 22       | 0               | 1                | NoError           | NoError            | Data         | 0xffffffff  | 0               | 0                   | 0         | 200    | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * |          | vary message confirmed seq nbr                                                                                                                                                                                                                                                          ||||||||||||||||||                    |
 * | 23       | 0               | 1                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 1                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 24       | 0               | 1                | NoError           | NoError            | ConnResp     | 0           | 0               | 0                   | 2         | 0      | 0           | 0            | 1                | 98        | 97          | true             | ConnRespReceived | NoError          | Normal operation   |
 * | 25       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 0                   | 0         | 1      | 0           | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 26       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 1                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | NoError          | Normal operation   |
 * | 27       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 2                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 28       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 0xffffffff          | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 29       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 0xffffffff          | 0         | 0      | 749         | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 30       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 0xffffffff          | 0         | 0      | 750         | 0            | 0                | 98        | 97          | true             | None             | NoError          | Normal operation   |
 * |          | vary confirmed timestamp                                                                                                                                                                                                                                                                ||||||||||||||||||                    |
 * | 31       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 2                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 32       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 2                   | 0         | 0      | 0           | 1            | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 33       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 2                   | 0         | 0      | 0           | 749          | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 34       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0               | 2                   | 0         | 0      | 0           | 750          | 0                | 98        | 97          | false            | DataReceived     | NoError          | Normal operation   |
 * | 35       | 0               | 1                | NoError           | NoError            | Data         | 0           | 1000            | 2                   | 0         | 0      | 0           | 1749         | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 36       | 0               | 1                | NoError           | NoError            | Data         | 0           | 1000            | 2                   | 0         | 0      | 0           | 1750         | 0                | 98        | 97          | false            | DataReceived     | NoError          | Normal operation   |
 * | 37       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0xffffffff      | 2                   | 0         | 0      | 0           | 748          | 0                | 98        | 97          | true             | DataReceived     | NoError          | Normal operation   |
 * | 38       | 0               | 1                | NoError           | NoError            | Data         | 0           | 0xffffffff      | 2                   | 0         | 0      | 0           | 759          | 0                | 98        | 97          | false            | DataReceived     | NoError          | Normal operation   |
 * |          | vary connection configuration and connection id                                                                                                                                                                                                                                         ||||||||||||||||||                    |
 * | 39       | 0               | 2                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | true             | ConnReqReceived  | NoError          | Normal operation   |
 * | 40       | 1               | 2                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 3         | 1           | true             | ConnReqReceived  | NoError          | Normal operation   |
 * |          | invalid parameter                                                                                                                                                                                                                                                                       ||||||||||||||||||                    |
 * | 41       | 1               | 1                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | InvalidParameter | Fatal error raised |
 * | 42       | 2               | 2                | NoError           | NoError            | ConnReq      | 0           | 0               | 0                   | 0         | 0      | 0           | 0            | 0                | 98        | 97          | false            | None             | InvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-580} Receive Message Function
 * @verifyReq{RASW-803} Sequence Number Range Check
 * @verifyReq{RASW-805} Sequence Number Check
 * @verifyReq{RASW-804} Sequence Integrity of the Confirmed Sequence Number
 * @verifyReq{RASW-822} Sequence Integrity of the Confirmed Time Stamp
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-652} Read Message Function
 * @verifyReq{RASW-375} Read Message Function Structure
 * @verifyReq{RASW-381} Redundancy Channel Id
 * @verifyReq{RASW-379} Buffer Size
 * @verifyReq{RASW-372} Message Size
 * @verifyReq{RASW-371} Message Buffer
 * @verifyReq{RASW-374} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(ReceiveMessageTestWithParam, srcorTest030ReceiveMessageFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config =  default_config;

  test_config.number_of_connections     = configured_connections;
  uint16_t message_length               = UT_SRCOR_MESSAGE_LEN_MAX;
  uint16_t buffer_size                  = message_length;
  uint8_t message_buffer[buffer_size]   = {};
  uint32_t configured_timestamp_rx      = 0;

  bool seq_nbr_in_range =  false;
  bool msg_checks_passed = false;
  bool expect_seq_nbr_in_seq = false;
  bool confirmed_seq_nbr_in_seq = false;
  bool message_authentic = false;
  bool timestamp_in_seq = false;

  // message gets dropped when timestamp not in sequence (non time relevant messages are not getting checked)
  uint32_t timestamp_diff = message_timestamp - configured_timestamp_rx;
  if((timestamp_diff >= 0 && timestamp_diff < test_config.t_max) ||
    message_type ==  srtyp_kSrMessageConnReq ||
    message_type ==  srtyp_kSrMessageConnResp ||
    message_type ==  srtyp_kSrMessageRetrReq ||
    message_type ==  srtyp_kSrMessageRetrResp ||
    message_type ==  srtyp_kSrMessageDiscReq)
  {
    timestamp_in_seq = true;
  }

  if(expected_error == radef_kNoError)
  {
    if(message_sender_id == test_config.connection_configurations[connection_id].receiver_id && message_receiver_id == test_config.connection_configurations[connection_id].sender_id)
    {
      message_authentic = true;

      // check if given seq nbr in range of accepted seq numbers
      if((message_seq_nbr - configured_seq_nbr_rx) >= 0 && (message_seq_nbr - configured_seq_nbr_rx) <= 10 * test_config.n_send_max ||
        message_type == srtyp_kSrMessageConnReq ||
        message_type == srtyp_kSrMessageConnResp ||
        message_type == srtyp_kSrMessageRetrResp)
      {
        seq_nbr_in_range = true;

        // uint wrap around check for any other message type than srtyp_kSrMessageConnReq or srtyp_kSrMessageConnResp
        uint32_t difference = message_confirmed_seq_nbr - configured_confirmed_seq_nr_rx;
        uint32_t max_value_difference = configured_seq_number_tx - (configured_confirmed_seq_nr_rx - 1U);  // Unsigned integer wrap around allowed here

        // confirmed seq nbr check
        if(message_type == srtyp_kSrMessageConnReq && message_confirmed_seq_nbr == 0 ||
          message_type == srtyp_kSrMessageConnResp && message_confirmed_seq_nbr == (configured_seq_number_tx - 1) ||
          (message_type != srtyp_kSrMessageConnReq &&  message_type != srtyp_kSrMessageConnResp) && (difference >= 0) && (difference <= max_value_difference))
        {
          confirmed_seq_nbr_in_seq = true;
        }
      }
    }
  }

  // add given test parameter to message
  setDataInBuffer_u16(message_buffer, message_length, expected_message_length_pos);
  setDataInBuffer_u16(message_buffer, message_type, expected_message_type_pos);
  setDataInBuffer_u32(message_buffer, message_receiver_id, expected_receiver_id_pos);
  setDataInBuffer_u32(message_buffer, message_sender_id, expected_sender_id_pos);
  setDataInBuffer_u32(message_buffer, message_seq_nbr, expected_sequence_nbr_pos);
  setDataInBuffer_u32(message_buffer, message_confirmed_seq_nbr, expected_conf_seq_nbr_pos);
  setDataInBuffer_u32(message_buffer, message_confirmed_timestamp, expected_conf_timestamp_pos);
  setDataInBuffer_u32(message_buffer, message_timestamp, expected_timestamp_pos);

  // values for ReceiveMessage function call to verify
  srtyp_ConnectionEvents connection_event;
  bool sequence_number_in_seq;
  bool confirmed_time_stamp_in_seq;

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // expect calls for ReceiveMessage call
  // allow mock return value which is not possible on normal function call but needs to be done for coverage
  if(expected_error == radef_kNoError || read_msg_return_value == radef_kInternalError || check_msg_return_value == radef_kInternalError)
  {
    uint32_t red_channel_id = connection_id; // 1:1 mapping

    // set expect call for reading message and set created message as pointer
    EXPECT_CALL(sradin_mock, sradin_ReadMessage(red_channel_id, _, _, _)).WillOnce(DoAll(SetArrayArgument<3>(message_buffer, message_buffer + buffer_size), Return(read_msg_return_value)));

    if(read_msg_return_value != radef_kNoMessageReceived && read_msg_return_value != radef_kInternalError)
    {
      // GeneralMessageCheck (local function) calls
      EXPECT_CALL(srmsg_mock, srmsg_CheckMessage(_)).WillOnce(Return(check_msg_return_value));

      switch (check_msg_return_value)
      {
        case radef_kNoError:
            EXPECT_CALL(srmsg_mock, srmsg_GetMessageHeader(_, _)).WillOnce((Invoke(this, &srcorTest::getMessageHeader)));
          if(message_authentic)
          {
            // if CheckSequenceNumberRange check
            if(seq_nbr_in_range)
            {
              if(timestamp_in_seq)
              {
                // confirmed seq nbr check
                if(confirmed_seq_nbr_in_seq)
                {
                  // if incoming message seq nbr equal to expected or specific message type
                  if(message_seq_nbr == configured_seq_nbr_rx ||
                    message_type == srtyp_kSrMessageConnReq ||
                    message_type == srtyp_kSrMessageConnResp ||
                    message_type == srtyp_kSrMessageRetrResp ||
                    message_type == srtyp_kSrMessageDiscReq)
                  {
                    expect_seq_nbr_in_seq = true;
                  }
                  else
                  {
                    EXPECT_CALL(srdia_mock, srdia_IncSequenceNumberErrorCounter(connection_id));
                  }

                  msg_checks_passed = true;
                }
                else
                {
                  EXPECT_CALL(srdia_mock, srdia_IncConfirmedSequenceNumberErrorCounter(connection_id));
                }
              }
            }
          }
          else
          {
            // message authenticity check fails
            EXPECT_CALL(srdia_mock, srdia_IncAddressErrorCounter(connection_id));
          }
          break;
        case radef_kInvalidMessageSize:
          break;
        case radef_kInvalidMessageMd4:
          EXPECT_CALL(srdia_mock, srdia_IncSafetyCodeErrorCounter(connection_id));
          break;
        case radef_kInvalidMessageType:
          EXPECT_CALL(srdia_mock,srdia_IncTypeErrorCounter(connection_id));
          break;
        default:
          break;
      }
    }
  }

  // initialize the module
  srcor_Init(&test_config);

  if(expected_error == radef_kNoError)
  {
    // set next expect receive and send seq nbr and confirmed timestamp rx
    srcor_rasta_connections[connection_id].sequence_number_rx = configured_seq_nbr_rx;
    srcor_rasta_connections[connection_id].sequence_number_tx = configured_seq_number_tx;
    srcor_rasta_connections[connection_id].confirmed_sequence_number_rx = configured_confirmed_seq_nr_rx;
    srcor_rasta_connections[connection_id].confirmed_time_stamp_rx = configured_confirmed_timestamp_rx;
    srcor_rasta_connections[connection_id].time_stamp_rx = configured_timestamp_rx;
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_ReceiveMessage(connection_id, &connection_event, &sequence_number_in_seq, &confirmed_time_stamp_in_seq), std::invalid_argument);
  }
  else
  {
    // call the function
    srcor_ReceiveMessage(connection_id, &connection_event, &sequence_number_in_seq, &confirmed_time_stamp_in_seq);
    EXPECT_EQ(connection_event, expected_connection_event);
    EXPECT_EQ(sequence_number_in_seq, expect_seq_nbr_in_seq);
    EXPECT_EQ(confirmed_time_stamp_in_seq, expect_conf_timestamp_in_seq);

    if(msg_checks_passed)
    {
      // verify message header in input buffer
      ASSERT_EQ(srcor_rasta_connections[connection_id].input_buffer.message_header.confirmed_sequence_number, message_confirmed_seq_nbr);
      ASSERT_EQ(srcor_rasta_connections[connection_id].input_buffer.message_header.confirmed_time_stamp, message_confirmed_timestamp);
      ASSERT_EQ(srcor_rasta_connections[connection_id].input_buffer.message_header.message_length, message_length);
      ASSERT_EQ(srcor_rasta_connections[connection_id].input_buffer.message_header.message_type, message_type);
      ASSERT_EQ(srcor_rasta_connections[connection_id].input_buffer.message_header.receiver_id, message_receiver_id);
      ASSERT_EQ(srcor_rasta_connections[connection_id].input_buffer.message_header.sender_id, message_sender_id);
      ASSERT_EQ(srcor_rasta_connections[connection_id].input_buffer.message_header.sequence_number, message_seq_nbr);
      ASSERT_EQ(srcor_rasta_connections[connection_id].input_buffer.message_header.time_stamp, message_timestamp);

      // verify message in buffer
      ASSERT_TRUE(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer);
    }
    else
    {
      // verify message not in buffer
      ASSERT_FALSE(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer);

      if(read_msg_return_value == radef_kNoMessageReceived)
      {
        // verify no received data pending when no message received
        ASSERT_FALSE(srcor_rasta_connections[connection_id].received_data_pending);
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyReceiveMessageFunction,
  ReceiveMessageTestWithParam,
  ::testing::Values(
    // vary read message return value
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventConnReqReceived,  radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoMessageReceived,  radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kInternalError,      radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kInternalError),
    // vary check message return values
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kInvalidMessageMd4,  srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kInvalidMessageType, srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kInvalidMessageSize, srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kInternalError,      srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kInternalError),
    // vary message types
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnResp,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageRetrReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventRetrReqReceived,  radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageRetrResp,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventRetrRespReceived, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageDiscReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventDiscReqReceived,  radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageHb,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventHbReceived,       radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageRetrData,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventRetrDataReceived, radef_kNoError),
    // vary sender and receiver id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 97, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 98, 98, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  0, 97, 98, false, srtyp_kConnEventNone,             radef_kNoError),
    // vary message seq nbr
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          0,          0, 1,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          0,          0, 200,                0,          0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          0,          0, 201,                0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    1,          0,          0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    UINT32_MAX, 0,          0,          0, (UINT32_MAX + 200), 0,          0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    UINT32_MAX, 0,          0,          0, (UINT32_MAX + 201), 0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    // vary message confirmed seq nbr
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,          0,          0, 0,                  0,          0,                  1, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnResp,
                    0,          0,          0,          2, 0,                  0,          0,                  1, 98, 97, true,  srtyp_kConnEventConnRespReceived, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          0,          0, 1,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          1,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          2,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          UINT32_MAX, 0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          UINT32_MAX, 0, 0,                  749,       0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          UINT32_MAX, 0, 0,                  750,       0,                  0, 98, 97, false,          srtyp_kConnEventNone,     radef_kNoError),
    // vary confirmed timestamp
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          2,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          2,          0, 0,                  0,          1,                  0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          2,          0, 0,                  0,          749,                0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          0,          2,          0, 0,                  0,          750,                0, 98, 97, false, srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          1000,       2,          0, 0,                  0,          1749,               0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          1000,       2,          0, 0,                  0,          1750,               0, 98, 97, false, srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          UINT32_MAX, 2,          0, 0,                  0,          (UINT32_MAX + 749), 0, 98, 97, true,  srtyp_kConnEventDataReceived,     radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageData,
                    0,          UINT32_MAX, 2,          0, 0,                  0,          (UINT32_MAX + 750), 0, 98, 97, false, srtyp_kConnEventDataReceived,     radef_kNoError),
    // vary connection configuration and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,         0,          0, 0,                  0,          0,                  0, 98, 97, true,  srtyp_kConnEventConnReqReceived,  radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,         0,          0, 0,                  0,          0,                  0, 3,  1,  true,  srtyp_kConnEventConnReqReceived,  radef_kNoError),
    // invalid parameter
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX,       radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,         0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), radef_kNoError,            radef_kNoError,            srtyp_kSrMessageConnReq,
                    0,          0,         0,          0, 0,                  0,          0,                  0, 98, 97, false, srtyp_kConnEventNone,             radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest031} Verify the HandleRetrReq function
 *
 * @details     This test verifies the HandleRetrReq function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Set expectations for send buffer to prepare for retransmission
 * - Init the module
 * - Set seq nr tx and confirmed timestamp connection data on channel
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call the HandleRetrReq function
 * - When no error Verify header from srsend_PrepareBufferForRetr call
 * - verify seq nbr update
 * .
 *
 * @testParameter
 * - Channel id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config            | Expected values                         ||
 * |----------|-----------------|------------------------|---------------------|--------------------|
 * |          | Channel id      | Configured connections | Expect fatal error  | Test Result        |
 * | 0        | 0               | 1                      | False               | Normal operation   |
 * | 1        | 0               | 2                      | False               | Normal operation   |
 * | 2        | 1               | 2                      | False               | Normal operation   |
 * | 3        | 1               | 1                      | True                | Fatal error thrown |
 * | 4        | 2               | 2                      | True                | Fatal error thrown |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-570} Handle Retransmission Request Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(HandleRetrReqTestWithParam, srcorTest031VerifyHandleRetrReqFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t seq_nr_tx = 100;
  uint32_t confirmed_time_stamp = 10000;
  uint32_t last_seq_nr_in_bfr = seq_nr_tx + 5;
  uint32_t confirmed_seq_nr = 99;

  // values to verify
  srtyp_SrMessageHeaderCreate message_header_to_verify = {};

  if(!expect_fatal_error)
  {
    EXPECT_CALL(srsend_mock, srsend_PrepareBufferForRetr(connection_id, confirmed_seq_nr, _, _))
      .WillOnce(DoAll(SaveArg<2>(&message_header_to_verify), SetArgPointee<3>(last_seq_nr_in_bfr)));
  }

  // set expectations for init module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(test_config.safety_code_type, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(test_config.number_of_connections, test_config.t_max, test_config.n_diag_window, test_config.diag_timing_distr_intervals)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(test_config.number_of_connections)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(test_config.number_of_connections, test_config.n_send_max)).Times(1);

  // call the init function
  EXPECT_NO_THROW(srcor_Init(&test_config));

  if(!expect_fatal_error)
  {
    // set seq nbr tx to verify in header
    srcor_rasta_connections[connection_id].sequence_number_tx = seq_nr_tx;
    srcor_rasta_connections[connection_id].time_stamp_rx = confirmed_time_stamp;

    // set confirmed seq nbr
    srcor_rasta_connections[connection_id].confirmed_sequence_number_rx = confirmed_seq_nr;
  }

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_HandleRetrReq(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_HandleRetrReq(connection_id));

    // verify seq number tx gets updated with seq nr from srsend_PrepareBufferForRetr call
    EXPECT_EQ(srcor_rasta_connections[connection_id].sequence_number_tx, last_seq_nr_in_bfr);

    // verify header from srsend_PrepareBufferForRetr call
    EXPECT_EQ(message_header_to_verify.sender_id, test_config.connection_configurations[connection_id].sender_id);
    EXPECT_EQ(message_header_to_verify.receiver_id, test_config.connection_configurations[connection_id].receiver_id);
    EXPECT_EQ(message_header_to_verify.sequence_number, seq_nr_tx);
    EXPECT_EQ(message_header_to_verify.confirmed_time_stamp, confirmed_time_stamp);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyHandleRetrReqFunction,
  HandleRetrReqTestWithParam,
  ::testing::Values(
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), true)
  ));

/**
 * @test        @ID{srcorTest032} Verify the ProcessReceivedMessage function
 *
 * @details     This test verifies the ProcessReceivedMessage function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Set expectations
 *   - For get message header and timer value
 *   - For expecting timeliness respected
 *   - For remove from buffer if message timeout related and type not connection request
 *   - For get connection message data call if message connection request or connection response
 *   - For get message payload and add to buffer if message type data or retransmission data
 *   - For received flow control calls
 *   - For updating connection diagnostics if message timeout related
 *   - For init module calls
 * - Init the module
 * - Set confirmed sequence number rx, last send confirmed seq nr_tx and message in buffer flag according test parameter
 * - Call the process received message function
 * - Verify  following values if no error occurred
 *   - sequence number & time stamp update
 *   - confirmed timestamp rx set from message
 *   - confirmed rx and confirmed timestamp set at connection request
 *   - confirmed sequence nbr changed
 *   - message in buffer flag cleared
 * .
 *
 * @testParameter
 * - Connection id: RaSTA connection identification
 * - Connections: Number of configured connections
 * - Msg in buffer: Indicates if a message is in the input buffer
 * - Confirmed seq nr rx: Configured confirmed received sequence number on connection
 * - Last send confirmed seq nr tx: Configured last sent sequence number on connection
 * - Timer ti value: Timer ti value on connection
 * - Message type: Message type of message in buffer
 * - Seq nr: Message sequence number
 * - Confirmed seq nr: Message confirmed sequence number
 * - Confirmed ts: Message confirmed timestamp
 * - Timer value: Return value of rasys_GetTimerValue function call
 * - Msgs to send return value: Return value of srsend_GetNumberOfMessagesToSend function call
 * - Expect timeliness respected: Indicates if timeliness expect to be respected
 * - Expected error code: Expected error code when calling function
 * .
 * | Test Run | Input parameter | Test config                                                                                                                                                                                  ||||||||||| Expected values                                                          |||
 * |----------|-----------------|-------------|---------------|---------------------|-------------------------------|----------------|--------------|--------|------------------|--------------|-------------|---------------------------|-----------------------------|-------------------------|--------------------|
 * |          |                 |             |               | Rasta connection configuration 				               	             ||| Message configuration		                            ||||             |                           |                             |                         | Test Result        |
 * |          | Connection id   | Connections | Msg in buffer | Confirmed seq nr rx | Last send confirmed seq nr tx | Timer ti value | Message type | Seq nr | Confirmed seq nr | Confirmed ts | Timer value | Msgs to send return value | Expect timeliness respected | Expected error code     |                    |
 * |          | vary message in buffer                                                                                                                                                                                                                                              ||||||||||||||                    |
 * | 0        | 0               | 1           | false         | 0                   | 0                             | 0              | ConnReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kInternalError    | Fatal error raised |
 * | 1        | 0               | 1           | true          | 0                   | 0                             | 0              | ConnReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * |          | vary message type not time related                                                                                                                                                                                                                                  ||||||||||||||                    |
 * | 2        | 0               | 1           | true          | 0                   | 0                             | 0              | ConnResp     | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 3        | 0               | 1           | true          | 0                   | 0                             | 0              | RetrReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 4        | 0               | 1           | true          | 0                   | 0                             | 0              | RetrResp     | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 5        | 0               | 1           | true          | 0                   | 0                             | 0              | DiscReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * |          | vary message type time related                                                                                                                                                                                                                                      ||||||||||||||                    |
 * | 6        | 0               | 1           | true          | 0                   | 0                             | 0              | Hb           | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 7        | 0               | 1           | true          | 0                   | 0                             | 0              | Data         | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 8        | 0               | 1           | true          | 0                   | 0                             | 0              | RetrData     | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * |          | vary current timer ti value on non time related message                                                                                                                                                                                                             ||||||||||||||                    |
 * | 9        | 0               | 1           | true          | 0                   | 0                             | 1              | ConnReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 10       | 0               | 1           | true          | 0                   | 0                             | 1              | RetrReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 11       | 0               | 1           | true          | 0                   | 0                             | 1              | RetrResp     | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 12       | 0               | 1           | true          | 0                   | 0                             | 0xffffffff     | ConnReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * |          | vary current timer value and message timer value on time related message                                                                                                                                                                                            ||||||||||||||                    |
 * | 13       | 0               | 1           | true          | 0                   | 0                             | 0              | Hb           | 0      | 0                | 1000         | 1000        | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 14       | 0               | 1           | true          | 0                   | 0                             | 0              | Hb           | 0      | 0                | 1000         | 1001        | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 15       | 0               | 1           | true          | 0                   | 0                             | 0              | Hb           | 0      | 0                | 1000         | 1749        | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 16       | 0               | 1           | true          | 0                   | 0                             | 0              | Hb           | 0      | 0                | 1000         | 1750        | 0                         | false                       | radef_kNoError          | Normal operation   |
 * |          | vary msg seq nbr                                                                                                                                                                                                                                                    ||||||||||||||                    |
 * | 17       | 0               | 1           | true          | 0                   | 0                             | 0              | ConnReq      | 1      | 0                | 1000         | 1050        | 0                         | true                       | radef_kNoError          | Normal operation   |
 * | 18       | 0               | 1           | true          | 0                   | 0                             | 0              | Data         | 2      | 0                | 1000         | 1050        | 0                         | true                        | radef_kNoError          | Normal operation   |
 * |          | vary confirmed seq nbr of message and connection                                                                                                                                                                                                                    ||||||||||||||                    |
 * | 19       | 0               | 1           | true          | 1                   | 0                             | 0              | Data         | 2      | 0                | 1000         | 1050        | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 20       | 0               | 1           | true          | 1                   | 1                             | 1              | ConnReq      | 2      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 21       | 0               | 1           | true          | 1                   | 1                             | 1              | ConnResp     | 2      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 22       | 0               | 1           | true          | 0                   | 0                             | 0              | Data         | 2      | 1                | 1000         | 1050        | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 23       | 0               | 1           | true          | 1                   | 0                             | 0              | Data         | 2      | 1                | 1000         | 1050        | 0                         | true                        | radef_kNoError          | Normal operation   |
 * |          | vary configured last send confirmed sequence number tx                                                                                                                                                                                                              ||||||||||||||                    |
 * | 24       | 0               | 1           | true          | 1                   | 1                             | 0              | Data         | 11     | 0                | 1000         | 1050        | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 25       | 0               | 1           | true          | 1                   | 1                             | 0              | Data         | 11     | 0                | 1000         | 1050        | 1                         | true                        | radef_kNoError          | Normal operation   |
 * |          | vary connection configuration and connection id                                                                                                                                                                                                                     ||||||||||||||                    |
 * | 26       | 0               | 2           | true          | 0                   | 0                             | 0              | ConnReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 27       | 1               | 2           | true          | 0                   | 0                             | 0              | ConnReq      | 0      | 0                | 0            | 0           | 0                         | true                        | radef_kNoError          | Normal operation   |
 * | 28       | 1               | 1           | false         | 0                   | 0                             | 0              | ConnReq      | 0      | 0                | 0            | 0           | 0                         | false                       | radef_kInvalidParameter | Fatal error raised |
 * | 29       | 2               | 2           | false         | 0                   | 0                             | 0              | ConnReq      | 0      | 0                | 0            | 0           | 0                         | false                       | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-579} Process Received Messages Function
 * @verifyReq{RASW-806} Timeliness Check
 * @verifyReq{RASW-808} Timer Ti
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-422} Timer Value
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 * @verifyReq{RASW-419} Timer Granularity
 */
TEST_P(ProcessReceivedMessageTestWithParam, srcorTest032VerifyProcessReceivedMessageFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;

  srtyp_SrMessageHeader message_header = {
    0,
    message_type,
    0,
    0,
    message_seq_nr,
    message_confirmed_seq_nr,
    0,
    message_confirmed_ts
  };

  bool confirmed_sequence_number_changed = false;
  bool message_timeout_related = false;
  bool timeliness_respected = true;
  uint32_t expected_timer_t_i_value = 0;
  uint32_t confirmed_seq_nr_tx = message_seq_nr;

  if(expected_error == radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).WillOnce(Return(timer_value));

    if((message_type == srtyp_kSrMessageHb) || (message_type == srtyp_kSrMessageData) || (message_type == srtyp_kSrMessageRetrData))
    {
      message_timeout_related = true;
      uint32_t t_rtd_value = timer_value + UT_SRCOR_TIMER_GRANULARITY - message_confirmed_ts;

      if(test_config.t_max >= t_rtd_value)
      {
        expected_timer_t_i_value = (test_config.t_max - t_rtd_value);
      }
      else
      {
        expected_timer_t_i_value = 0;
      }
    }
    else
    {
      expected_timer_t_i_value = configured_timer_ti_value;
    }

    if(timeliness_respected)
    {
      // timeliness_respected = true;

      if(configured_confirmed_seq_nr_rx != message_header.confirmed_sequence_number)
      {
        confirmed_sequence_number_changed = true;

        if(message_type != srtyp_kSrMessageConnReq)
        {
          // Expect remove confirmed messages from send buffer
          EXPECT_CALL(srsend_mock, srsend_RemoveFromBuffer(connection_id, _));
        }
      }

      if ((message_type == srtyp_kSrMessageConnReq) || (message_type == srtyp_kSrMessageConnResp))
      {
        // Expect to handle received ConnReq & ConnResp
        EXPECT_CALL(srmsg_mock, srmsg_GetConnMessageData(_, _, _));
      }

      if ((message_type == srtyp_kSrMessageData) || (message_type == srtyp_kSrMessageRetrData))
      {
        // Expect to handle received data message
        EXPECT_CALL(srmsg_mock, srmsg_GetDataMessagePayload(_, _));
        EXPECT_CALL(srrece_mock, srrece_AddToBuffer(connection_id, _));
      }

      // expect received flow control
      if ((message_type == srtyp_kSrMessageHb) || (message_type == srtyp_kSrMessageRetrResp) || (message_type == srtyp_kSrMessageData) || (message_type == srtyp_kSrMessageRetrData))
      {
        if ((confirmed_seq_nr_tx - configured_last_send_confirmed_seq_nr_tx) >= test_config.m_w_a)
        {
          EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(3).WillRepeatedly(Return(msgs_to_send_return_value));

          if(msgs_to_send_return_value == 0U)
          {
            // Expect to send a HB message
            EXPECT_CALL(srmsg_mock, srmsg_CreateHeartbeatMessage(_, _));
            EXPECT_CALL(srsend_mock, srsend_AddToBuffer(_, _));
          }

          // expect send pending messages call
          EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id));
        }
      }

      if(message_timeout_related && expect_timeliness_respected)
      {
        // Expect connection diagnostics to be updated when timeout related
        EXPECT_CALL(srdia_mock, srdia_UpdateConnectionDiagnostics(connection_id, _, _));
      }
    }
  }

  // set expectations for init module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).WillOnce(Return(UT_SRCOR_TIMER_GRANULARITY));
  EXPECT_CALL(srmsg_mock, srmsg_Init(test_config.safety_code_type, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(test_config.number_of_connections, test_config.t_max, test_config.n_diag_window, test_config.diag_timing_distr_intervals)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(test_config.number_of_connections)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(test_config.number_of_connections, test_config.n_send_max)).Times(1);

  // call the init function
  EXPECT_NO_THROW(srcor_Init(&test_config));

  if(expected_error == radef_kNoError)
  {
    if(message_type != message_timeout_related)
    {
      srcor_rasta_connections[connection_id].timer_t_i = configured_timer_ti_value;
    }

    // set confirmed sequence number rx, last send confirmed seq nr_tx, message in buffer flag and message header according to test parameter
    srcor_rasta_connections[connection_id].confirmed_sequence_number_rx = configured_confirmed_seq_nr_rx;
    srcor_rasta_connections[connection_id].last_send_confirmed_sequence_number_tx = configured_last_send_confirmed_seq_nr_tx;
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = message_in_buffer;
    srcor_rasta_connections[connection_id].input_buffer.message_header = message_header;
  }

  // execute the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_ProcessReceivedMessage(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_EQ(expect_timeliness_respected, srcor_ProcessReceivedMessage(connection_id));

    if(expect_timeliness_respected)
    {
      // verify sequence number & time stamp update
      EXPECT_EQ(srcor_rasta_connections[connection_id].sequence_number_rx, message_header.sequence_number + 1U);
      EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_tx, message_header.sequence_number);
      EXPECT_EQ(srcor_rasta_connections[connection_id].time_stamp_rx, message_header.time_stamp);

      if(message_timeout_related)
      {
        // verify confirmed timestamp rx set from message
        EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_time_stamp_rx, message_header.confirmed_time_stamp);
        EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_rx, message_header.confirmed_sequence_number);
      }
      else if (message_type == srtyp_kSrMessageConnReq)
      {
        // verify confirmed rx and confirmed timestamp set at connection request
        EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_time_stamp_rx, timer_value);
        EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_rx, (srcor_rasta_connections[connection_id].sequence_number_tx - 1));
      }
      else if((message_type != srtyp_kSrMessageConnReq) && confirmed_sequence_number_changed)
      {
        // verify confirmed sequence nbr changed
        EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_rx, message_header.confirmed_sequence_number);
      }
    }

    // verify message in buffer flag cleared
    EXPECT_FALSE(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyProcessReceivedMessageFunction,
  ProcessReceivedMessageTestWithParam,
  ::testing::Values(
    // vary message in buffer
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       false, 0, 0, 0,          srtyp_kSrMessageConnReq,  0,  0,    0,    0, 0, true, radef_kInternalError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageConnReq,  0,  0,    0,    0, 0, true, radef_kNoError),
    // vary message type not time related
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageConnResp, 0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageRetrReq,  0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageRetrResp, 0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageDiscReq,  0,  0,    0,    0, 0, true, radef_kNoError),
    // vary message type time related
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageHb,       0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageData,     0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageRetrData, 0,  0,    0,    0, 0, true, radef_kNoError),
    // vary current timer ti value on non time related message
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 1,          srtyp_kSrMessageConnReq,  0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 1,          srtyp_kSrMessageRetrReq,  0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 1,          srtyp_kSrMessageRetrResp, 0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, UINT32_MAX, srtyp_kSrMessageConnReq,  0,  0,    0,    0, 0, true, radef_kNoError),
    // vary current timer value and message timer value on time related message
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageHb,       0,  0, 1000, 1000, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageHb,       0,  0, 1000, 1001, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageHb,       0,  0, 1000, (1000 + default_config.t_max - 1), 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageHb,       0,  0, 1000, (1000 + default_config.t_max), 0, false, radef_kNoError),
    // vary msg seq nbr
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageConnReq,  1,  0, 1000, 1050, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageData,     2,  0, 1000, 1050, 0, true, radef_kNoError),
    // vary confirmed seq nbr of message and connection
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  1, 0, 0,          srtyp_kSrMessageData,     2,  0, 1000, 1050, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  1, 1, 1,          srtyp_kSrMessageConnReq,  2,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  1, 1, 1,          srtyp_kSrMessageConnResp, 2,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageData,     2,  1, 1000, 1050, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  1, 0, 0,          srtyp_kSrMessageData,     2,  1, 1000, 1050, 0, true, radef_kNoError),
    // vary configured last send confirmed sequence number tx
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  1, 1, 0,          srtyp_kSrMessageData,     11, 0, 1000, 1050, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  1, 1, 0,          srtyp_kSrMessageData,     11, 0, 1000, 1050, 1, true, radef_kNoError),
    // vary connection configuration and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN,       true,  0, 0, 0,          srtyp_kSrMessageConnReq,  0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX,       true,  0, 0, 0,          srtyp_kSrMessageConnReq,  0,  0,    0,    0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX,       false, 0, 0, 0,          srtyp_kSrMessageConnReq,  0,  0,    0,    0, 0, false, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), false, 0, 0, 0,          srtyp_kSrMessageConnReq,  0,  0,    0,    0, 0, false, radef_kInvalidParameter)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

