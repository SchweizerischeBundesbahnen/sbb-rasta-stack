/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srmsg.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the messages module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srmsg.hh"

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_messages
 * @{
 */

/**
 * @test        @ID{srmsgTest015} Verify the init function.
 *
 * @details     This test verifies that the init function can only be called once.
 *
 * Test steps:
 * - call the init function and verify that it completes successfully
 * - call the init function again and verify that a radef_kAlreadyInitialized error is raised
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-629} Init sr_messages Function
 * @verifyReq{RASW-487} Enum Safety Code Type Structure
 */
TEST_F(srmsgTest, srmsgTest015VerifyInitFunction)
{
    srcty_SafetyCodeType safety_code_type = srcty_kSafetyCodeTypeFullMd4;
    srcty_Md4InitValue md4_init_value = {0, 0, 0, 0};

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));

    // call the init function and verify that it completes successfully
    EXPECT_NO_THROW(srmsg_Init(safety_code_type, md4_init_value));

    // call the init function again and verify that an assertion violations is raised as the module is already initialized
    EXPECT_THROW(srmsg_Init(safety_code_type, md4_init_value), std::invalid_argument);
}

/**
 * @test        @ID{srmsgTest016} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - call the srmsg_CreateConnReqMessage function with invalid message pointer and verify that a radef_kInvalidParameter error gets thrown
 * - call the srmsg_CreateConnRespMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CreateDataMessage function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CreateDataMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CreateRetrDataMessage function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CreateRetrDataMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CreateRetrReqMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CreateRetrRespMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CreateHeartbeatMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CreateDiscReqMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_UpdateMessageHeader function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_CheckMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetMessageHeader function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetMessageHeader function with invalid message header pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetMessageType function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetMessageSequenceNumber function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetDataMessagePayload function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetDataMessagePayload function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetConnMessageData function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetConnMessageData function with invalid protocol version pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetConnMessageData function with invalid nSendMax pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetDiscMessageData function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetDiscMessageData function with invalid detailed reason pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srmsg_GetDiscMessageData function with invalid reason pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-616} Check Message Function
 * @verifyReq{RASW-617} Create Connection Request Message Function
 * @verifyReq{RASW-618} Create Connection Response Message Function
 * @verifyReq{RASW-619} Create Retransmission Request Message Function
 * @verifyReq{RASW-620} Create Retransmission Response Message Function
 * @verifyReq{RASW-621} Create Disconnection Request Message Function
 * @verifyReq{RASW-622} Create Heartbeat Message Function
 * @verifyReq{RASW-623} Create Data Message Function
 * @verifyReq{RASW-624} Create Retransmitted Data Message Function
 * @verifyReq{RASW-625} Get Connection Message Data Function
 * @verifyReq{RASW-626} Get Data Message Payload Function
 * @verifyReq{RASW-627} Get Disconnection Message Data Function
 * @verifyReq{RASW-628} Get Message Header Function
 * @verifyReq{RASW-629} Init sr_messages Function
 * @verifyReq{RASW-630} Update Message Header Function
 * @verifyReq{RASW-824} Get Message Type Function
 * @verifyReq{RASW-825} Get Message Sequence Number Function
 * @verifyReq{RASW-520} Error Handling
 * @verifyReq{RASW-521} Input Parameter Check
 */
TEST_F(srmsgTest, srmsgTest016VerifyNullPointerCheck)
{
    uint8_t nbr_of_function_calls = 24;        // number of function calls

    // Test variables
    srcty_SafetyCodeType safety_code_type = srcty_kSafetyCodeTypeFullMd4;
    srcty_Md4InitValue md4_init_value = {0, 0, 0, 0};
    srtyp_SrMessageHeaderCreate message_header_create = {};
    srtyp_ProtocolVersion protocol_version = {UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN};
    uint16_t n_send_max = UT_SRMSG_NSENDMAX_MIN;
    srtyp_SrMessagePayload message_payload = {UT_SRMSG_DATA_LEN_MIN, {}};
    srtyp_SrMessage message = {};
    sraty_DiscReason reason = sraty_kDiscReasonUserRequest;
    uint16_t detailed_reason = 0;
    srtyp_SrMessageHeaderUpdate message_header_update = {};
    srtyp_SrMessageHeader message_header = {};

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .Times(nbr_of_function_calls)
        .WillRepeatedly(Invoke(this, &srmsgTest::invalidArgumentException));

    // initialize the module
    srmsg_Init(safety_code_type, md4_init_value);

    // call the functions
    EXPECT_THROW(srmsg_CreateConnReqMessage(message_header_create, protocol_version, n_send_max, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateConnRespMessage(message_header_create, protocol_version, n_send_max, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateDataMessage(message_header_create, (srtyp_SrMessagePayload *)NULL, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateDataMessage(message_header_create, &message_payload, (srtyp_SrMessage * )NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateRetrDataMessage(message_header_create, (srtyp_SrMessagePayload *)NULL, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateRetrDataMessage(message_header_create, &message_payload, (srtyp_SrMessage * )NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateRetrReqMessage(message_header_create, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateRetrRespMessage(message_header_create, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateHeartbeatMessage(message_header_create, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateDiscReqMessage(message_header_create, detailed_reason, reason, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_UpdateMessageHeader(message_header_update, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_CheckMessage((srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_GetMessageHeader((srtyp_SrMessage *)NULL, &message_header), std::invalid_argument);
    EXPECT_THROW(srmsg_GetMessageHeader(&message, (srtyp_SrMessageHeader *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_GetMessageType((srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_GetMessageSequenceNumber((srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_GetDataMessagePayload((srtyp_SrMessage *)NULL, &message_payload), std::invalid_argument);
    EXPECT_THROW(srmsg_GetDataMessagePayload(&message, (srtyp_SrMessagePayload *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_GetConnMessageData((srtyp_SrMessage *)NULL, &protocol_version, &n_send_max), std::invalid_argument);
    EXPECT_THROW(srmsg_GetConnMessageData(&message, (srtyp_ProtocolVersion *)NULL, &n_send_max), std::invalid_argument);
    EXPECT_THROW(srmsg_GetConnMessageData(&message, &protocol_version, (uint16_t *)NULL), std::invalid_argument);
    EXPECT_THROW(srmsg_GetDiscMessageData((srtyp_SrMessage *)NULL, &detailed_reason, &reason), std::invalid_argument);
    EXPECT_THROW(srmsg_GetDiscMessageData(&message, (uint16_t *)NULL, &reason), std::invalid_argument);
    EXPECT_THROW(srmsg_GetDiscMessageData(&message, &detailed_reason, (sraty_DiscReason *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{srmsgTest017} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the CreateConnReqMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CreateConnRespMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CreateDataMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CreateRetrDataMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CreateRetrReqMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CreateRetrRespMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CreateHeartbeatMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CreateDiscReqMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the UpdateMessageHeader function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CheckMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetMessageHeader function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetMessageType function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetMessageSequenceNumber function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetDataMessagePayload function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetConnMessageData function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetDiscMessageData function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-615} Component sr_messages Overview
 * @verifyReq{RASW-616} Check Message Function
 * @verifyReq{RASW-617} Create Connection Request Message Function
 * @verifyReq{RASW-618} Create Connection Response Message Function
 * @verifyReq{RASW-619} Create Retransmission Request Message Function
 * @verifyReq{RASW-620} Create Retransmission Response Message Function
 * @verifyReq{RASW-621} Create Disconnection Request Message Function
 * @verifyReq{RASW-622} Create Heartbeat Message Function
 * @verifyReq{RASW-623} Create Data Message Function
 * @verifyReq{RASW-624} Create Retransmitted Data Message Function
 * @verifyReq{RASW-625} Get Connection Message Data Function
 * @verifyReq{RASW-626} Get Data Message Payload Function
 * @verifyReq{RASW-627} Get Disconnection Message Data Function
 * @verifyReq{RASW-628} Get Message Header Function
 * @verifyReq{RASW-630} Update Message Header Function
 * @verifyReq{RASW-824} Get Message Type Function
 * @verifyReq{RASW-825} Get Message Sequence Number Function
 */
TEST_F(srmsgTest, srmsgTest017VerifyInitializationCheck)
{
    uint8_t nbr_of_function_calls = 16;         // number of function calls

    // Test variables
    srtyp_SrMessageHeaderCreate message_header_create = {};
    srtyp_ProtocolVersion protocol_version = {UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN};
    uint16_t n_send_max = UT_SRMSG_NSENDMAX_MIN;
    srtyp_SrMessagePayload message_payload = {UT_SRMSG_DATA_LEN_MIN, {}};
    srtyp_SrMessage message = {};
    sraty_DiscReason reason = sraty_kDiscReasonUserRequest;
    uint16_t detailed_reason = 0;
    srtyp_SrMessageHeaderUpdate message_header_update = {};
    srtyp_SrMessageHeader message_header = {};

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
        .Times(nbr_of_function_calls)
        .WillRepeatedly(Invoke(this, &srmsgTest::invalidArgumentException));

    // call the functions without initializing the module
    EXPECT_THROW(srmsg_CreateConnReqMessage(message_header_create, protocol_version, n_send_max, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateConnRespMessage(message_header_create, protocol_version, n_send_max, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateDataMessage(message_header_create, &message_payload, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateRetrDataMessage(message_header_create, &message_payload, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateRetrReqMessage(message_header_create, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateRetrRespMessage(message_header_create, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateHeartbeatMessage(message_header_create, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CreateDiscReqMessage(message_header_create, detailed_reason, reason, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_UpdateMessageHeader(message_header_update, &message), std::invalid_argument);
    EXPECT_THROW(srmsg_CheckMessage(&message), std::invalid_argument);
    EXPECT_THROW(srmsg_GetMessageHeader(&message, &message_header), std::invalid_argument);
    EXPECT_THROW(srmsg_GetMessageType(&message), std::invalid_argument);
    EXPECT_THROW(srmsg_GetMessageSequenceNumber(&message), std::invalid_argument);
    EXPECT_THROW(srmsg_GetDataMessagePayload(&message, &message_payload), std::invalid_argument);
    EXPECT_THROW(srmsg_GetConnMessageData(&message, &protocol_version, &n_send_max), std::invalid_argument);
    EXPECT_THROW(srmsg_GetDiscMessageData(&message, &detailed_reason, &reason), std::invalid_argument);
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
