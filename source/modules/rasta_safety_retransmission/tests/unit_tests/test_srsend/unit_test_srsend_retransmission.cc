/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srsend_retransmission.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version c43d3a924e35983d1734435fa2bbfbffb1750d0c
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 *
 * @brief Unit test file for the send buffer module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srsend.hh"

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/**
 * @brief Create a message and add it to the buffer
 *
 * @param connection_id     ID of the connection buffer
 * @param sequence_number   sequence number of the message to be created
 * @param message_type      type of the message to be created
 * @param data              payload data to be added (optional - set to NULL if not used)
 */
void CreateAndAddMessage(const uint32_t connection_id, const uint32_t sequence_number, const uint16_t message_type, uint8_t * data);

/**
 * @brief Verify the message content
 *
 * @param message           message to be verified
 * @param header            expected message header
 * @param sequence_nbr      expected sequence number
 * @param type              expected message type
 * @param data              expected payload data (can be set to NULL if payload is not to be verified)
 */
void verifyMessage(srtyp_SrMessage * message, const srtyp_SrMessageHeaderCreate * header, uint32_t sequence_nbr, uint16_t type, uint8_t * data);

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

const uint16_t srsendTest::expected_type_retr_resp;     ///< message type for retransmission response
const uint16_t srsendTest::expected_type_retr_data;     ///< message type for retransmission data
const uint16_t srsendTest::expected_type_heartbeat;     ///< message type for heartbeat
const uint16_t srsendTest::expected_type_data;          ///< message type for data message
const uint16_t srsendTest::expected_type_retr_req;      ///< message type for retransmission request

// -----------------------------------------------------------------------------
// Test class definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized PrepareBufferForRetr test
 *
 * Parameter order:
 * - connection ID (uint32_t)
 * - add unsent message type 1 (uint16_t)
 * - add unsent message type 2 (uint16_t)
 * - add unsent message type 3 (uint16_t)
 * - expect fatal error (bool)
 * .
 */
class PrepareBufferForRetrTestWithParam :   public srsendTest,
                                            public testing::WithParamInterface<std::tuple<uint32_t, uint16_t, uint16_t, uint16_t, bool>>
{
    public:
    uint32_t connection_id =            std::get<0>(GetParam());    ///< connection ID
    uint16_t add_unsent_msg_type_1 =    std::get<1>(GetParam());    ///< unsent message nr. 1 to add to the buffer
    uint16_t add_unsent_msg_type_2 =    std::get<2>(GetParam());    ///< unsent message nr. 2 to add to the buffer
    uint16_t add_unsent_msg_type_3 =    std::get<3>(GetParam());    ///< unsent message nr. 3 to add to the buffer
    bool expect_fatal_error =           std::get<4>(GetParam());    ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized PrepareBufferForRetr error test
 *
 * Parameter order:
 * - maximum number of connections (uint8_t)
 * - connection ID (uint32_t)
 * - add message (bool)
 * - sequence number to add (uint32_t)
 * - error code (radef_RaStaReturnCode)
 * .
 */
class PrepareBufferForRetrErrorTestWithParam :  public srsendTest,
                                                public testing::WithParamInterface<std::tuple<uint8_t, uint32_t, bool, uint32_t, radef_RaStaReturnCode>>
{
    public:
    uint8_t max_num_conn =              std::get<0>(GetParam());    ///< maximum number of connections
    uint32_t connection_id =            std::get<1>(GetParam());    ///< connection ID
    bool add_message =                  std::get<2>(GetParam());    ///< indicates if a message will be added
    uint32_t seq_nbr_to_add =           std::get<3>(GetParam());    ///< sequence number of the message to be added
    radef_RaStaReturnCode error_code =  std::get<4>(GetParam());    ///< expected error code
};

/**
 * @brief Test class for value-parametrized IsSequenceNumberInBuffer test
 *
 * Parameter order:
 * - maximum number of connections (uint8_t)
 * - connection ID (uint32_t)
 * - add messages (bool)
 * - sequence number (uint32_t)
 * - expect fatal error (bool)
 * - return code (radef_RaStaReturnCode)
 * .
 */
class IsSequenceNumberInBufferTestWithParam :   public srsendTest,
                                                public testing::WithParamInterface<std::tuple<uint8_t, uint32_t, bool, uint32_t, bool, radef_RaStaReturnCode>>
{
    public:
    uint8_t max_num_conn =              std::get<0>(GetParam());    ///< maximum number of connections
    uint32_t connection_id =            std::get<1>(GetParam());    ///< connection ID
    bool add_messages =                 std::get<2>(GetParam());    ///< indicates if messages shall be added to the buffer
    uint32_t sequence_nbr =             std::get<3>(GetParam());    ///< sequence number under test
    bool expect_fatal_error =           std::get<4>(GetParam());    ///< indicates if a fatal error is expected
    radef_RaStaReturnCode return_code = std::get<5>(GetParam());    ///< expected return code
};

/**
 * @brief Test class for value-parametrized RemoveFromBuffer test
 *
 * Parameter order:
 * - maximum number of connections (uint8_t)
 * - connection ID (uint32_t)
 * - start sequence number (uint32_t)
 * - number of messages to add (uint32_t)
 * - number of messages to read (uint32_t)
 * - confirmed sequence number (uint32_t)
 * - expected number of used elements before removing (uint32_t)
 * - expected number of used elements after removing (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class RemoveFromBufferTestWithParam :   public srsendTest,
                                        public testing::WithParamInterface<std::tuple<uint8_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool>>
{
    public:
    uint8_t max_num_conn =              std::get<0>(GetParam());    ///< maximum number of connections
    uint32_t connection_id =            std::get<1>(GetParam());    ///< connection ID
    uint32_t start_sn =                 std::get<2>(GetParam());    ///< start sequence number
    uint32_t n_write =                  std::get<3>(GetParam());    ///< number of messages to add
    uint32_t n_read =                   std::get<4>(GetParam());    ///< number of messages to read
    uint32_t conf_seq_nbr =             std::get<5>(GetParam());    ///< confirmed sequence number
    uint32_t exp_used_elements_before = std::get<6>(GetParam());    ///< expected number of used elements before remove
    uint32_t exp_used_elements_after =  std::get<7>(GetParam());    ///< expected number of used elements after remove
    bool expect_fatal_error =           std::get<8>(GetParam());    ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_sendBuf
 * @{
 */

/**
 * @test        @ID{srsendTest010} Verify the PrepareBufferForRetr function.
 *
 * @details     This test verifies the correct behaviour of the PrepareBufferForRetr function.
 *
 * Test steps:
 * - initialize the module
 * - create and add the following messages to the buffer: connReq, connResp, retrReq, retrResp, discReq, Hb, data, retrData
 * - read those messages, so that the already_sent flag is set
 * - create and add additional messages according to the test parameter (they will remain unsent)
 * - expect a radef_kInternalError when fatal error gets thrown
 * - call the PrepareBufferForRetr function
 * - read out messages from the buffer and verify the order and content:
 *   - first message is a Retransmission Response
 *   - second message is a data message (converted to a retransmitted data message)
 *   - third message is a retransmitted data message
 *   - -> further order from this point is described in table below
 * - after complete order is verified, ensure that no more messages are to be read
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Unsent Msg #1: Type of unsent message nr. 1 to add to the buffer
 * - Unsent Msg #2: Type of Unsent message nr. 2 to add to the buffer
 * - Unsent Msg #3: Type of Unsent message nr. 3 to add to the buffer
 * - Exp. Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config                                 ||| Expected values                                                       |||
 * |----------|-----------------|---------------|---------------|---------------|------------------|---------------------------------|--------------------|
 * |          | Connection Id   | Unsent Msg #1 | Unsent Msg #2 | Unsent Msg #3 | Exp. Fatal Error | Exp further msg order in buffer | Test Result        |
 * | 0        | 0               | -             | -             | -             | false            | Heartbeat                       | Normal operation   |
 * | 1        | 0               | Data          | -             | -             | false            | Data                            | Normal operation   |
 * | 2        | 0               | Data          | Data          | -             | false            | Data, Data                      | Normal operation   |
 * | 3        | 0               | RetrData      | -             | -             | false            | RetrData, Heartbeat             | Normal operation   |
 * | 4        | 0               | RetrData      | Data          | -             | false            | RetrData, Data                  | Normal operation   |
 * | 5        | 0               | RetrData      | Data          | Heartbeat     | false            | RetrData, Data                  | Normal operation   |
 * | 6        | 0               | RetrData      | RetrData      | -             | false            | RetrData, RetrData, Heartbeat   | Normal operation   |
 * | 7        | 0               | RetrData      | RetrData      | Data          | false            | RetrData, RetrData, Data        | Normal operation   |
 * | 8        | 0               | RetrReq       | -             | -             | false            | Heartbeat, retrReq              | Normal operation   |
 * | 9        | 0               | Data          | RetrReq       | -             | false            | Data, RetrReq                   | Normal operation   |
 * | 10       | 0               | RetrData      | Data          | ConnReq       | true             |                                 | Fatal error raised |
 * | 11       | 0               | RetrData      | Data          | ConnResp      | true             |                                 | Fatal error raised |
 * | 12       | 0               | RetrData      | Data          | RetrData      | true             |                                 | Fatal error raised |
 * | 13       | 0               | RetrData      | Data          | RetrResp      | true             |                                 | Fatal error raised |
 * | 14       | 0               | RetrData      | Data          | DiscReq       | true             |                                 | Fatal error raised |
 * | 15       | 1               | -             | -             | -             | false            | Heartbeat                       | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 *              This test case makes use of the srmsg module which needs to be tested separately.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-603} Prepare Buffer for Retransmission Function
 */
TEST_P(PrepareBufferForRetrTestWithParam, srsendTest010PrepareBufferForRetransmissionTest)
{
    srtyp_SrMessage read_message = {0, {}};
    uint8_t number_of_messages = 8;
    uint8_t data_msg_payload[UT_SRSEND_DATA_LEN_MAX] = {};
    uint8_t retr_data_msg_payload[UT_SRSEND_DATA_LEN_MAX] = {};
    uint8_t unsent_data_msg_payload[UT_SRSEND_DATA_LEN_MAX] = {};
    uint8_t unsent_retr_data_msg_payload[UT_SRSEND_DATA_LEN_MAX] = {};
    uint32_t sequence_number = 0;
    uint32_t n_offset = 15;
    uint32_t i = 0;

    uint32_t last_conf_nbr = 9;

    const srtyp_SrMessageHeaderCreate message_header = {0x12345678, 0xAABBCCDD, 21, 0xA1B2C3D4};

    // create data arrays
    for (i = 0; i < UT_SRSEND_DATA_LEN_MAX; i++)
    {
        data_msg_payload[i] = (0xA << 4) | (i % 16);
        retr_data_msg_payload[i] = (0xB << 4) | (i % 16);
        unsent_data_msg_payload[i] = (0xC << 4) | (i % 16);
        unsent_retr_data_msg_payload[i] = (0xD << 4) | (i % 16);
    }

    //  initialize module
    srsend_Init(UT_SRSEND_MAX_NUM_CONN);

    // add offset to check wraparound
    for (i = 0; i < n_offset; i++)
    {
        CreateAndAddMessage(connection_id, i, srtyp_kSrMessageData, NULL);
        srsend_ReadMessageToSend(connection_id, &read_message);
        srsend_RemoveFromBuffer(connection_id, i);
    }

    // add messages
    CreateAndAddMessage(connection_id, (last_conf_nbr + 1), expected_type_conn_req, NULL);
    CreateAndAddMessage(connection_id, (last_conf_nbr + 2), expected_type_conn_resp, NULL);
    CreateAndAddMessage(connection_id, (last_conf_nbr + 3), expected_type_retr_req, NULL);
    CreateAndAddMessage(connection_id, (last_conf_nbr + 4), expected_type_retr_resp, NULL);
    CreateAndAddMessage(connection_id, (last_conf_nbr + 5), expected_type_disc_req, NULL);
    CreateAndAddMessage(connection_id, (last_conf_nbr + 6), expected_type_heartbeat, NULL);
    CreateAndAddMessage(connection_id, (last_conf_nbr + 7), expected_type_data, data_msg_payload);
    CreateAndAddMessage(connection_id, (last_conf_nbr + 8), expected_type_retr_data, retr_data_msg_payload);

    switch (add_unsent_msg_type_1)
    {
    case 0:
        break;
    case srtyp_kSrMessageData:
        CreateAndAddMessage(connection_id, (last_conf_nbr + 9), add_unsent_msg_type_1, unsent_data_msg_payload);
        break;
    case srtyp_kSrMessageRetrData:
        CreateAndAddMessage(connection_id, (last_conf_nbr + 9), add_unsent_msg_type_1, unsent_retr_data_msg_payload);
        break;
    default:
        CreateAndAddMessage(connection_id, (last_conf_nbr + 9), add_unsent_msg_type_1, NULL);
        break;
    }

    switch (add_unsent_msg_type_2)
    {
    case 0:
        break;
    case srtyp_kSrMessageData:
        CreateAndAddMessage(connection_id, (last_conf_nbr + 10), add_unsent_msg_type_2, unsent_data_msg_payload);
        break;
    case srtyp_kSrMessageRetrData:
        CreateAndAddMessage(connection_id, (last_conf_nbr + 10), add_unsent_msg_type_1, unsent_retr_data_msg_payload);
        break;
    default:
        CreateAndAddMessage(connection_id, (last_conf_nbr + 10), add_unsent_msg_type_2, NULL);
        break;
    }

    switch (add_unsent_msg_type_3)
    {
    case 0:
        break;
    case srtyp_kSrMessageData:
        CreateAndAddMessage(connection_id, (last_conf_nbr + 11), add_unsent_msg_type_3, unsent_data_msg_payload);
        break;
    default:
        CreateAndAddMessage(connection_id, (last_conf_nbr + 11), add_unsent_msg_type_3, NULL);
        break;
    }

    // read out messages to set the already_sent flag
    for (i = 0; i < number_of_messages; i++)
    {
        srsend_ReadMessageToSend(connection_id, &read_message);
    }

    if(expect_fatal_error)
    {
        // call the PrepareBufferForRetr function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInternalError)).WillRepeatedly(Invoke(this, &srsendTest::invalidArgumentException));
        EXPECT_THROW(srsend_PrepareBufferForRetr(connection_id, last_conf_nbr, message_header, &sequence_number), std::invalid_argument);
    }
    else
    {
        // call the PrepareBufferForRetr function
        srsend_PrepareBufferForRetr(connection_id, last_conf_nbr, message_header, &sequence_number);

        // read messages from the buffer and verify that they are correct
        // the first message should be the Retransmission Response
        srsend_ReadMessageToSend(connection_id, &read_message);
        verifyMessage(&read_message, &message_header, (message_header.sequence_number + 0), expected_type_retr_resp, (uint8_t *)NULL);

        // the second message should be the data message (converted to a retransmitted data message)
        srsend_ReadMessageToSend(connection_id, &read_message);
        verifyMessage(&read_message, &message_header, (message_header.sequence_number + 1), expected_type_retr_data, data_msg_payload);

        // the third message should be the retransmitted data message
        srsend_ReadMessageToSend(connection_id, &read_message);
        verifyMessage(&read_message, &message_header, (message_header.sequence_number + 2), expected_type_retr_data, retr_data_msg_payload);

        if (add_unsent_msg_type_1 == srtyp_kSrMessageData)
        {
            // if a unsent data message follows, it shall be added to the new buffer instead of the heartbeat
            srsend_ReadMessageToSend(connection_id, &read_message);
            verifyMessage(&read_message, &message_header, (message_header.sequence_number + 3), expected_type_data, unsent_data_msg_payload);

            if (add_unsent_msg_type_2 == srtyp_kSrMessageData)
            {
                // if an unsent data message follows after that, it will be added to the new buffer instead of the heartbeat
                srsend_ReadMessageToSend(connection_id, &read_message);
                verifyMessage(&read_message, &message_header, (message_header.sequence_number + 4), expected_type_data, unsent_data_msg_payload);
            }
            else if (add_unsent_msg_type_2 == srtyp_kSrMessageRetrReq)
            {
                // if an retransmission request follows, it will be added to the new buffer
                srsend_ReadMessageToSend(connection_id, &read_message);
                verifyMessage(&read_message, &message_header, (message_header.sequence_number + 4), expected_type_retr_req, (uint8_t*)NULL);
            }
        }
        else if (add_unsent_msg_type_1 == srtyp_kSrMessageRetrData)
        {
            // if an unsent retransmission data message follows, it shall be added to the new buffer
            srsend_ReadMessageToSend(connection_id, &read_message);
            verifyMessage(&read_message, &message_header, (message_header.sequence_number + 3), expected_type_retr_data, unsent_retr_data_msg_payload);

            if (add_unsent_msg_type_2 == srtyp_kSrMessageData)
            {
                // if an unsent data message follows after that, it will be added to the new buffer instead of the heartbeat
                srsend_ReadMessageToSend(connection_id, &read_message);
                verifyMessage(&read_message, &message_header, (message_header.sequence_number + 4), expected_type_data, unsent_data_msg_payload);
            }
            else if (add_unsent_msg_type_2 == srtyp_kSrMessageRetrData)
            {
                // if again an unsent retransmission data message follows after the unsent retransmission, it will be added
                srsend_ReadMessageToSend(connection_id, &read_message);
                verifyMessage(&read_message, &message_header, (message_header.sequence_number + 4), expected_type_retr_data, unsent_retr_data_msg_payload);
            }
            else if (add_unsent_msg_type_2 == 0)
            {
                // if no unsent data message follows after the unsent retransmission, a heartbeat will be added
                srsend_ReadMessageToSend(connection_id, &read_message);
                verifyMessage(&read_message, &message_header, (message_header.sequence_number + 4), expected_type_heartbeat, (uint8_t*)NULL);
            }

            if (add_unsent_msg_type_3 == srtyp_kSrMessageData)
            {
                // if an unsent data message follows after two unsent retrans., it will be added to the new buffer instead of the heartbeat
                srsend_ReadMessageToSend(connection_id, &read_message);
                verifyMessage(&read_message, &message_header, (message_header.sequence_number + 5), expected_type_data, unsent_data_msg_payload);
            }
            else if ((add_unsent_msg_type_3 == 0) && (add_unsent_msg_type_2 != 0) && (add_unsent_msg_type_2 != srtyp_kSrMessageData))
            {
                // if no unsent data message follows after two unsent retransmissions, a heartbeat will be added
                srsend_ReadMessageToSend(connection_id, &read_message);
                verifyMessage(&read_message, &message_header, (message_header.sequence_number + 5), expected_type_heartbeat, (uint8_t*)NULL);
            }
        }
        else if (add_unsent_msg_type_1 == srtyp_kSrMessageRetrReq)
        {
            // if an unsent retransmission request follows, it will be added to the new buffer, followed by a HB
            srsend_ReadMessageToSend(connection_id, &read_message);
            verifyMessage(&read_message, &message_header, (message_header.sequence_number + 3), expected_type_heartbeat, (uint8_t*)NULL);

            srsend_ReadMessageToSend(connection_id, &read_message);
            verifyMessage(&read_message, &message_header, (message_header.sequence_number + 4), expected_type_retr_req, (uint8_t*)NULL);
        }
        else
        {
            // if no other message is pending, a heartbeat will be added which signals the end of the retransmission
            srsend_ReadMessageToSend(connection_id, &read_message);
            verifyMessage(&read_message, &message_header, (message_header.sequence_number + 3), expected_type_heartbeat, (uint8_t*)NULL);
        }

        // verify that the buffer is now empty
        EXPECT_EQ(radef_kNoMessageToSend, srsend_ReadMessageToSend(connection_id, &read_message));
    }

}

INSTANTIATE_TEST_SUITE_P(
    verifyPrepareBufferForRetrFunction,
    PrepareBufferForRetrTestWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, 0, false),
        std::make_tuple(0, srtyp_kSrMessageData, 0, 0, false),
        std::make_tuple(0, srtyp_kSrMessageData, srtyp_kSrMessageData, 0, false),
        std::make_tuple(0, srtyp_kSrMessageRetrData, 0, 0, false),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageData, 0, false),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageData, srtyp_kSrMessageHb, false),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageRetrData, 0, false),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageRetrData, srtyp_kSrMessageData, false),
        std::make_tuple(0, srtyp_kSrMessageRetrReq, 0, 0, false),
        std::make_tuple(0, srtyp_kSrMessageData, srtyp_kSrMessageRetrReq, 0, false),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageData, srtyp_kSrMessageConnReq, true),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageData, srtyp_kSrMessageConnResp, true),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageData, srtyp_kSrMessageRetrData, true),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageData, srtyp_kSrMessageRetrResp, true),
        std::make_tuple(0, srtyp_kSrMessageRetrData, srtyp_kSrMessageData, srtyp_kSrMessageDiscReq, true),
        std::make_tuple(1, 0, 0, 0, false)
    ));

/**
 * @test        @ID{srsendTest011} Verify the PrepareBufferForRetr function (error case)
 *
 * @details     This test verifies the correct behaviour of the PrepareBufferForRetr function
 *              in case of invalid connection id or sequence number.
 *
 * Test steps:
 * - initialize the module
 * - create and add a data message with given sequence number (if activated)
 * - call the PrepareBufferForRetr function with last confirmed sequence number = 10
 * - check the return value
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Max. Num. Conn.: Number of configured connections
 * - Add message: Indicates if a message gets added to buffer before calling srsend_PrepareBufferForRetr function
 * - Sequence Nbr. Add: sequence number of the message to be added
 * - Exp. Error Code: Expected error code when calling srsend_PrepareBufferForRetr function
 * .
 * | Test Run | Input parameter | Test config                                     ||| Expected values                                  ||
 * |----------|-----------------|-----------------|-------------|-------------------|------------------------------|--------------------|
 * |          | Connection Id   | Max. Num. Conn. | Add message | Sequence Nbr. Add | Exp. Error Code              | Test Result        |
 * | 0        | 1               | 1               | False       | 0                 | radef_kInvalidParameter      | Fatal Error raised |
 * | 1        | 2               | 2               | False       | 0                 | radef_kInvalidParameter      | Fatal Error raised |
 * | 2        | 0               | 2               | False       | 0                 | radef_kInvalidSequenceNumber | Fatal Error raised |
 * | 3        | 0               | 2               | True        | 10                | radef_kInvalidSequenceNumber | Fatal Error raised |
 * | 4        | 0               | 2               | True        | 12                | radef_kInvalidSequenceNumber | Fatal Error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 *              This test case makes use of the srmsg module which needs to be tested separately.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-603} Prepare Buffer for Retransmission Function
 */
TEST_P(PrepareBufferForRetrErrorTestWithParam, srsendTest011PrepareBufferForRetransmissionTestErrorCase)
{
    const srtyp_SrMessageHeaderCreate dummy_msg_header = {0, 0, 0, 0};
    uint32_t new_sequence_number = 0;
    const uint32_t last_conf_msg = 10;

    //  initialize module
    srsend_Init(max_num_conn);

    // add a data message if indicated by test parameter
    if (add_message)
    {
        CreateAndAddMessage(connection_id, seq_nbr_to_add, srtyp_kSrMessageData, NULL);
    }

    // call the PrepareBufferForRetr function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(error_code)).WillOnce(Invoke(this, &srsendTest::invalidArgumentException));
    EXPECT_THROW(srsend_PrepareBufferForRetr(connection_id, last_conf_msg, dummy_msg_header, &new_sequence_number), std::invalid_argument);
}

INSTANTIATE_TEST_SUITE_P(
    verifyPrepareBufferForRetrFunctionErrorCase,
    PrepareBufferForRetrErrorTestWithParam,
    ::testing::Values(
        std::make_tuple(1, 1, false, 0, radef_kInvalidParameter),
        std::make_tuple(2, 2, false, 0, radef_kInvalidParameter),
        std::make_tuple(2, 0, false, 0, radef_kInvalidSequenceNumber),
        std::make_tuple(2, 0, true, 10, radef_kInvalidSequenceNumber),
        std::make_tuple(2, 0, true, 12, radef_kInvalidSequenceNumber)
    ));

/**
 * @test        @ID{srsendTest012} Verify the IsSequenceNumberInBuffer function
 *
 * @details     This test verifies the correct behaviour of the IsSequenceNumberInBuffer function
 *
 * Test steps:
 * - initialize the module
 * - add, read and remove 5 messages in the buffer (to generate index offset)
 * - add 20 messages to the buffer starting with sequence number 100 (if activated by the test parameter)
 * - call the IsSequenceNumberInBuffer function with given sequence number and check the return value
 * .
 *
 * @testParameter
 * - Conn. Id: RaSTA connection identification
 * - Seq. Number: Sequence number to check in buffer
 * - Max. Num. Conn.: Number of configured connections
 * - Exp. Fatal Error: Indicates if a fatal error is expected
 * - Exp. Return Value: Expected return value from srsend_IsSequenceNumberInBuffer function call
 * .
 * | Test Run | Input parameter        || Test config     | Expected values                                                    |||
 * |----------|-----------|-------------|-----------------|------------------|------------------------------|--------------------|
 * |          | Conn. Id  | Seq. Number | Max. Num. Conn. | Exp. Fatal Error | Exp. Return Value            | Test Result        |
 * | 0        | 0         | 99          | 2               | false            | radef_kInvalidSequenceNumber | Normal operation   |
 * | 1        | 0         | 100         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 2        | 0         | 101         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 3        | 0         | 102         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 4        | 0         | 103         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 5        | 0         | 104         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 6        | 0         | 105         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 7        | 0         | 106         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 8        | 0         | 107         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 9        | 0         | 108         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 10       | 0         | 109         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 11       | 0         | 110         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 12       | 0         | 111         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 13       | 0         | 112         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 14       | 0         | 113         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 15       | 0         | 114         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 16       | 0         | 115         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 17       | 0         | 116         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 18       | 0         | 117         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 19       | 0         | 118         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 20       | 0         | 119         | 2               | false            | radef_kNoError               | Normal operation   |
 * | 21       | 0         | 120         | 2               | false            | radef_kInvalidSequenceNumber | Normal operation   |
 * | 22       | 1         | 100         | 1               | true             | radef_kInvalidParameter      | Fatal error raised |
 * | 23       | 2         | 100         | 2               | true             | radef_kInvalidParameter      | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 *              This test case makes use of the srmsg module which needs to be tested separately.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-602} Is Sequence Number in Buffer Function
 */
TEST_P(IsSequenceNumberInBufferTestWithParam, srsendTest012IsSequenceNumberInBufferTest)
{
    uint32_t sequence_number_start = 100;
    srtyp_SrMessage read_message = {0, {}};
    uint8_t n_offset = 5;

    //  initialize module
    srsend_Init(max_num_conn);

    if(expect_fatal_error)
    {
        // call the PrepareBufferForRetr function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(return_code)).WillOnce(Invoke(this, &srsendTest::invalidArgumentException));
        EXPECT_THROW(srsend_IsSequenceNumberInBuffer(connection_id, sequence_nbr), std::invalid_argument);
    }
    else
    {
        // add offset to check wraparound
        for (uint8_t i = 0; i < n_offset; i++)
        {
            CreateAndAddMessage(connection_id, i, srtyp_kSrMessageData, NULL);
            srsend_ReadMessageToSend(connection_id, &read_message);
            srsend_RemoveFromBuffer(connection_id, i);
        }

        if (add_messages)
        {
            for (uint8_t i = 0; i < UT_SRSEND_NSENDMAX_MAX; i++)
            {
                CreateAndAddMessage(connection_id, (sequence_number_start + i), srtyp_kSrMessageData, NULL);
            }
        }

        // call the PrepareBufferForRetr function
        EXPECT_EQ(return_code, srsend_IsSequenceNumberInBuffer(connection_id, sequence_nbr));
    }
}

INSTANTIATE_TEST_SUITE_P(
    IsSequenceNumberInBufferTest,
    IsSequenceNumberInBufferTestWithParam,
    ::testing::Values(
        std::make_tuple(2, 0, false, 100, false, radef_kInvalidSequenceNumber),
        std::make_tuple(2, 0, true, 99, false, radef_kInvalidSequenceNumber),
        std::make_tuple(2, 0, true, 100, false, radef_kNoError),
        std::make_tuple(2, 0, true, 101, false, radef_kNoError),
        std::make_tuple(2, 0, true, 102, false, radef_kNoError),
        std::make_tuple(2, 0, true, 103, false, radef_kNoError),
        std::make_tuple(2, 0, true, 104, false, radef_kNoError),
        std::make_tuple(2, 0, true, 105, false, radef_kNoError),
        std::make_tuple(2, 0, true, 106, false, radef_kNoError),
        std::make_tuple(2, 0, true, 107, false, radef_kNoError),
        std::make_tuple(2, 0, true, 108, false, radef_kNoError),
        std::make_tuple(2, 0, true, 109, false, radef_kNoError),
        std::make_tuple(2, 0, true, 110, false, radef_kNoError),
        std::make_tuple(2, 0, true, 111, false, radef_kNoError),
        std::make_tuple(2, 0, true, 112, false, radef_kNoError),
        std::make_tuple(2, 0, true, 113, false, radef_kNoError),
        std::make_tuple(2, 0, true, 114, false, radef_kNoError),
        std::make_tuple(2, 0, true, 115, false, radef_kNoError),
        std::make_tuple(2, 0, true, 116, false, radef_kNoError),
        std::make_tuple(2, 0, true, 117, false, radef_kNoError),
        std::make_tuple(2, 0, true, 118, false, radef_kNoError),
        std::make_tuple(2, 0, true, 119, false, radef_kNoError),
        std::make_tuple(2, 0, true, 120, false, radef_kInvalidSequenceNumber),
        std::make_tuple(1, 1, true, 100, true, radef_kInvalidParameter),
        std::make_tuple(2, 2, true, 100, true, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{srsendTest013} Verify the RemoveFromBuffer function
 *
 * @details     This test verifies the correct behaviour of the RemoveFromBuffer function
 *
 * Test steps:
 * - initialize the module
 * - create and add the given number of messages
 * - read the given number of messages, so that the already_sent flag is set
 * - verify the number of used buffer entries before the remove function call
 * - call the RemoveFromBuffer function
 * - expect a radef_kInvalidParameter if fatal error gets thrown
 * - verify the number of used buffer entries after the remove function call
 * .
 *
 * @testParameter
 * - Conn. Id: RaSTA connection identification
 * - Start SN: Sequence number to start count up from
 * - Max. Conn.: Number of configured connections
 * - n Add: Number of messages to add
 * - n Read: Number of messages to read
 * - Conf. SN: Confirmed sequence number
 * - Buffer Size before/after: Expected number of used elements before and after remove
 * - Exp. Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter       || Test config                                                   ||||| Expected values                      ||
 * |----------|-----------|------------|------------|-------|--------|----------|--------------------------|------------------|--------------------|
 * |          | Conn. Id  | Start SN   | Max. Conn. | n Add | n Read | Conf. SN | Buffer Size before/after | Exp. Fatal Error | Test Result        |
 * | 0        | 0         | 10         | 2          | 0     | 0      | 0        | 0 / 0                    | false            | Normal operation   |
 * | 1        | 0         | 10         | 2          | 3     | 3      | 9        | 3 / 3                    | false            | Normal operation   |
 * | 2        | 0         | 10         | 2          | 3     | 3      | 10       | 3 / 2                    | false            | Normal operation   |
 * | 3        | 0         | 10         | 2          | 3     | 3      | 11       | 3 / 1                    | false            | Normal operation   |
 * | 4        | 0         | 10         | 2          | 3     | 3      | 12       | 3 / 0                    | false            | Normal operation   |
 * | 5        | 0         | 10         | 2          | 3     | 0      | 10       | 3 / 3                    | false            | Normal operation   |
 * | 6        | 0         | 0xfffffffe | 2          | 5     | 5      | 0        | 5 / 2                    | false            | Normal operation   |
 * | 7        | 0         | 10         | 2          | 25    | 18     | 32       | 20 / 2                   | false            | Normal operation   |
 * | 8        | 1         | 10         | 1          | 0     | 0      | 0        | 0 / 0                    | true             | Fatal error raised |
 * | 9        | 2         | 10         | 2          | 0     | 0      | 0        | 0 / 0                    | true             | Fatal error raised |

 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 *              This test case makes use of the srmsg module which needs to be tested separately.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-605} Remove from Buffer Function
 */
TEST_P(RemoveFromBufferTestWithParam, srsendTest013RemoveFromBufferTest)
{
    srtyp_SrMessage read_message = {0, {}};
    uint32_t n_offset = 0;
    uint32_t start_sn_offset = 0;

    //  initialize module
    srsend_Init(max_num_conn);

    if(expect_fatal_error)
    {
        // call the PrepareBufferForRetr function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srsendTest::invalidArgumentException));
        EXPECT_THROW(srsend_RemoveFromBuffer(connection_id, conf_seq_nbr), std::invalid_argument);
    }
    else
    {
        start_sn_offset = start_sn;

        // workaround to check the index wraparound in the buffer
        if (n_write > UT_SRSEND_NSENDMAX_MAX)
        {
            n_offset = n_write - UT_SRSEND_NSENDMAX_MAX;
            for (uint32_t i = 0; i < n_offset; i++)
            {
                // add, read and remove messages to create offset
                CreateAndAddMessage(connection_id, (start_sn + i), srtyp_kSrMessageData, NULL);
                srsend_ReadMessageToSend(connection_id, &read_message);
                srsend_RemoveFromBuffer(connection_id, (start_sn + i));
            }
            start_sn_offset = start_sn + n_offset;
        }

        // add messages
        for (uint32_t i = 0; i < (n_write - n_offset); i++)
        {
            if((start_sn + i) > UINT32_MAX)
            {
                CreateAndAddMessage(connection_id, (start_sn_offset + i - UINT32_MAX), srtyp_kSrMessageData, NULL);
            }
            else
            {
                CreateAndAddMessage(connection_id, (start_sn_offset + i), srtyp_kSrMessageData, NULL);
            }
        }

        // read messages
        for (uint32_t i = 0; i < n_read; i++)
        {
            srsend_ReadMessageToSend(connection_id, &read_message);
        }

        // check buffer elements before the remove function call
        EXPECT_EQ(exp_used_elements_before, srsend_GetUsedBufferEntries(connection_id));
        EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - exp_used_elements_before), srsend_GetFreeBufferEntries(connection_id));

        // call the RemoveFromBuffer function
        srsend_RemoveFromBuffer(connection_id, conf_seq_nbr);

        // check buffer elements before the remove function call
        EXPECT_EQ(exp_used_elements_after, srsend_GetUsedBufferEntries(connection_id));
        EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - exp_used_elements_after), srsend_GetFreeBufferEntries(connection_id));

    }
}

INSTANTIATE_TEST_SUITE_P(
    RemoveFromBufferTest,
    RemoveFromBufferTestWithParam,
    ::testing::Values(
        std::make_tuple(2, 0, 10, 0, 0, 0, 0, 0, false),
        std::make_tuple(2, 0, 10, 3, 3, 9, 3, 3, false),
        std::make_tuple(2, 0, 10, 3, 3, 10, 3, 2, false),
        std::make_tuple(2, 0, 10, 3, 3, 11, 3, 1, false),
        std::make_tuple(2, 0, 10, 3, 3, 12, 3, 0, false),
        std::make_tuple(2, 0, 10, 3, 0, 10, 3, 3, false),
        std::make_tuple(2, 0, (UINT32_MAX - 1), 5, 5, 0, 5, 2, false),
        std::make_tuple(2, 0, 10, 25, 18, 32, 20, 2, false),
        std::make_tuple(1, 1, 10, 0, 0, 0, 0, 0, true),
        std::make_tuple(2, 2, 10, 0, 0, 0, 0, 0, true)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

void CreateAndAddMessage(const uint32_t connection_id, const uint32_t sequence_number, const uint16_t message_type, uint8_t * data)
{
    srtyp_SrMessage message = {0, {}};

    // set message length (maximum)
    message.message_size = UT_SRSEND_MESSAGE_LEN_MAX;
    setDataInBuffer_u16(&message.message, UT_SRSEND_MESSAGE_LEN_MAX, srsendTest::expected_message_length_pos);

    // set message type and sequence number
    setDataInBuffer_u16(&message.message, message_type, srsendTest::expected_message_type_pos);
    setDataInBuffer_u32(&message.message, sequence_number, srsendTest::expected_sequence_nbr_pos);

    if(data != NULL)
    {
        setDataInBuffer_u16(&message.message, UT_SRSEND_DATA_LEN_MAX, srsendTest::expected_payload_size_pos);
        for (uint32_t i = 0; i < UT_SRSEND_DATA_LEN_MAX; i++)
        {
            setDataInBuffer_u8(&message.message, data[i], (srsendTest::expected_payload_pos + i));
        }
    }

    // add the message to the buffer
    srsend_AddToBuffer(connection_id, &message);
}

void verifyMessage(srtyp_SrMessage * message, const srtyp_SrMessageHeaderCreate * header, uint32_t sequence_nbr, uint16_t type, uint8_t * data)
{
    uint16_t i = 0;

    EXPECT_EQ(getDataFromBuffer_u16(message->message, srsendTest::expected_message_type_pos), type);
    EXPECT_EQ(getDataFromBuffer_u32(message->message, srsendTest::expected_receiver_id_pos), header->receiver_id);
    EXPECT_EQ(getDataFromBuffer_u32(message->message, srsendTest::expected_sender_id_pos), header->sender_id);
    EXPECT_EQ(getDataFromBuffer_u32(message->message, srsendTest::expected_sequence_nbr_pos), sequence_nbr);
    EXPECT_EQ(getDataFromBuffer_u32(message->message, srsendTest::expected_conf_timestamp_pos), header->confirmed_time_stamp);
    if (data != NULL)
    {
        EXPECT_EQ(message->message_size, UT_SRSEND_MESSAGE_LEN_MAX);
        EXPECT_EQ(getDataFromBuffer_u16(message->message, srsendTest::expected_message_length_pos), UT_SRSEND_MESSAGE_LEN_MAX);
        EXPECT_EQ(getDataFromBuffer_u16(message->message, srsendTest::expected_payload_size_pos), UT_SRSEND_DATA_LEN_MAX);
        for (i = 0; i < UT_SRSEND_DATA_LEN_MAX; i++)
        {
            EXPECT_EQ(getDataFromBuffer_u8(message->message, (srsendTest::expected_payload_pos + i)), data[i]);
        }
    }
}
