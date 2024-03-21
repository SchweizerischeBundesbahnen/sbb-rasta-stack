/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srstm_states.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the state machine module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srstm.hh"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/**
 * @brief initializes the module and sets the current state
 *
 * This function calls the init function of the State Machine module with a maximum
 * number of connections of 1. Then it sets the state of the connection with given
 * connection ID to the desired value.
 *
 * @param connection_id     connection id
 * @param state             state to be set
 */
void InitModuleAndSetState(uint32_t connection_id, sraty_ConnectionStates state);

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized State Closed test
 *
 * The connection ID is set to zero.
 *
 * Parameter order:
 * - event (srtyp_ConnectionEvents)
 * - is sequence number in sequence (bool)
 * - next state (sraty_ConnectionStates)
 * - is server (bool)
 * .
 */
class StateClosedTestWithParam :    public srstmTest,
                                    public testing::WithParamInterface<std::tuple<srtyp_ConnectionEvents, bool, sraty_ConnectionStates, bool>>
{
    public:
    uint32_t connection_id = 0;                                         ///< connection id
    srtyp_ConnectionEvents event =      std::get<0>(GetParam());        ///< event to be processed
    bool is_sn_in_seq =                 std::get<1>(GetParam());        ///< indicates if the sequence number is in sequence
    sraty_ConnectionStates next_state = std::get<2>(GetParam());        ///< expected state after transition
    bool is_server =                    std::get<3>(GetParam());        ///< indicates if the system is configured as server
};

/**
 * @brief Test class for value-parametrized State Down test
 *
 * The connection ID is set to zero.
 *
 * Parameter order:
 * - event (srtyp_ConnectionEvents)
 * - is sequence number in sequence (bool)
 * - next state (sraty_ConnectionStates)
 * - is version check passed (bool)
 * - is timeliness respected (bool)
 * .
 */
class StateDownTestWithParam :  public srstmTest,
                                public testing::WithParamInterface<std::tuple<srtyp_ConnectionEvents, bool, sraty_ConnectionStates, bool, bool>>
{
    public:
    uint32_t connection_id = 0;                                         ///< connection id
    srtyp_ConnectionEvents event =      std::get<0>(GetParam());        ///< event to be processed
    bool is_sn_in_seq =                 std::get<1>(GetParam());        ///< indicates if the sequence number is in sequence
    sraty_ConnectionStates next_state = std::get<2>(GetParam());        ///< expected state after transition
    bool is_version_check_passed =      std::get<3>(GetParam());        ///< indicates if the version check is passed
    bool is_timeliness_respected =      std::get<4>(GetParam());        ///< indicates if the timeliness is respected
};

/**
 * @brief Test class for value-parametrized State Start test
 *
 * The connection ID is set to zero.
 *
 * Parameter order:
 * - connection id (uint32_t)
 * - event (srtyp_ConnectionEvents)
 * - is sequence number in sequence (bool)
 * - is confirmed timestamp in sequence (bool)
 * - next state (sraty_ConnectionStates)
 * - is version check passed (bool)
 * - is server (bool)
 * - disconnection reason (sraty_DiscReason)
 * - is timeliness respected (bool)
 * .
 */
class StateStartTestWithParam : public srstmTest,
                                public testing::WithParamInterface<std::tuple<srtyp_ConnectionEvents, bool, bool, sraty_ConnectionStates, bool, bool, sraty_DiscReason, bool>>
{
    public:
    uint32_t connection_id = 0;                                         ///< connection id
    srtyp_ConnectionEvents event =      std::get<0>(GetParam());        ///< event to be processed
    bool is_sn_in_seq =                 std::get<1>(GetParam());        ///< indicates if the sequence number is in sequence
    bool is_cts_in_seq =                std::get<2>(GetParam());        ///< indicates if the confirmed timestamp is in sequence
    sraty_ConnectionStates next_state = std::get<3>(GetParam());        ///< expected state after transition
    bool is_version_check_passed =      std::get<4>(GetParam());        ///< indicates if the version check is passed
    bool is_server =                    std::get<5>(GetParam());        ///< indicates if the system is configured as server
    sraty_DiscReason disc_reason =      std::get<6>(GetParam());        ///< expected reason for disconnection
    bool is_timeliness_respected =      std::get<7>(GetParam());        ///< indicates if the timeliness is respected
};

/**
 * @brief Test class for value-parametrized State Up test
 *
 * The connection ID is set to zero.
 *
 * Parameter order:
 * - event (srtyp_ConnectionEvents)
 * - is sequence number in sequence (bool)
 * - is confirmed timestamp in sequence (bool)
 * - next state (sraty_ConnectionStates)
 * - is retransmission request sequence number available (bool)
 * - disconnection reason (sraty_DiscReason)
 * - is timeliness respected (bool)
 * .
 */
class StateUpTestWithParam :    public srstmTest,
                                public testing::WithParamInterface<std::tuple<srtyp_ConnectionEvents, bool, bool, sraty_ConnectionStates,bool, sraty_DiscReason, bool>>
{
    public:
    uint32_t connection_id = 0;                                         ///< connection id
    srtyp_ConnectionEvents event =      std::get<0>(GetParam());        ///< event to be processed
    bool is_sn_in_seq =                 std::get<1>(GetParam());        ///< indicates if the sequence number is in sequence
    bool is_cts_in_seq =                std::get<2>(GetParam());        ///< indicates if the confirmed timestamp is in sequence
    sraty_ConnectionStates next_state = std::get<3>(GetParam());        ///< expected state after transition
    bool is_retr_sn_available =         std::get<4>(GetParam());        ///< indicates if the sequence number for retransmission is available
    sraty_DiscReason disc_reason =      std::get<5>(GetParam());        ///< expected reason for disconnection
    bool is_timeliness_respected =      std::get<6>(GetParam());        ///< indicates if the timeliness is respected
};

/**
 * @brief Test class for value-parametrized State RetrReq test
 *
 * The connection ID is set to zero.
 *
 * Parameter order:
 * - event (srtyp_ConnectionEvents)
 * - is sequence number in sequence (bool)
 * - next state (sraty_ConnectionStates)
 * - is retransmission request sequence number available (bool)
 * - disconnection reason (sraty_DiscReason)
 * - is timeliness respected (bool)
 * .
 */
class StateRetrReqTestWithParam :   public srstmTest,
                                    public testing::WithParamInterface<std::tuple<srtyp_ConnectionEvents, bool, sraty_ConnectionStates, bool, sraty_DiscReason, bool>>
{
    public:
    uint32_t connection_id = 0;                                         ///< connection id
    srtyp_ConnectionEvents event =      std::get<0>(GetParam());        ///< event to be processed
    bool is_sn_in_seq =                 std::get<1>(GetParam());        ///< indicates if the sequence number is in sequence
    sraty_ConnectionStates next_state = std::get<2>(GetParam());        ///< expected state after transition
    bool is_retr_sn_available =         std::get<3>(GetParam());        ///< indicates if the sequence number for retransmission is available
    sraty_DiscReason disc_reason =      std::get<4>(GetParam());        ///< expected reason for disconnection
    bool is_timeliness_respected =      std::get<5>(GetParam());        ///< indicates if the timeliness is respected
};

/**
 * @brief Test class for value-parametrized State RetrRun test
 *
 * The connection ID is set to zero.
 *
 * Parameter order:
 * - event (srtyp_ConnectionEvents)
 * - is sequence number in sequence (bool)
 * - is confirmed timestamp in sequence (bool)
 * - next state (sraty_ConnectionStates)
 * - is retransmission request sequence number available (bool)
 * - disconnection reason (sraty_DiscReason)
 * - is timeliness respected (bool)
 * .
 */
class StateRetrRunTestWithParam :   public srstmTest,
                                    public testing::WithParamInterface<std::tuple<srtyp_ConnectionEvents, bool, bool, sraty_ConnectionStates, bool, sraty_DiscReason, bool>>
{
    public:
    uint32_t connection_id = 0;                                         ///< connection id
    srtyp_ConnectionEvents event =      std::get<0>(GetParam());        ///< event to be processed
    bool is_sn_in_seq =                 std::get<1>(GetParam());        ///< indicates if the sequence number is in sequence
    bool is_cts_in_seq =                std::get<2>(GetParam());        ///< indicates if the confirmed timestamp is in sequence
    sraty_ConnectionStates next_state = std::get<3>(GetParam());        ///< expected state after transition
    bool is_retr_sn_available =         std::get<4>(GetParam());        ///< indicates if the sequence number for retransmission is available
    sraty_DiscReason disc_reason =      std::get<5>(GetParam());        ///< expected reason for disconnection
    bool is_timeliness_respected =      std::get<6>(GetParam());        ///< indicates if the timeliness is respected
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_sm
 * @{
 */

/**
 * @test        @ID{srstmTest005} Verify the State Closed.
 *
 * @details     This test verifies the state Closed and its transitions.
 *
 * Test steps:
 * - initialize the module for one connection
 * - set the current state to Closed
 * - verify that the current state is set correctly
 * - set expectations for different events
 * - call the ProcessConnectionStateMachine function
 * - verify that the new state matches the expectation
 * .
 *
 * @testParameter
 * - Event: Event to be processed
 * - SN in Seq.: Indicates if the sequence number is in sequence
 * - is Server: Indicates if the system is configured as server
 * - Next State: Expected state after transition
 * .
 * | Test Run | Test config                                  ||| Expected values                 ||
 * |----------|-----------------------|------------|-----------|------------|---------------------|
 * |          | Event                 | SN in Seq. | is Server | Next State | Test Result         |
 * | 0        | None                  | False      | false     | Closed     | No state transition |
 * | 1        | Open                  | False      | true      | Down       | Transition -> Down  |
 * | 2        | Open                  | False      | false     | Start      | Transition -> Start |
 * | 3        | Close                 | False      | false     | Closed     | No state transition |
 * | 4        | SendData              | False      | false     | Closed     | No state transition |
 * | 5        | ConnReqReceived       | False      | false     | Closed     | No state transition |
 * | 6        | ConnRespReceived      | False      | false     | Closed     | No state transition |
 * | 7        | RetrReqReceived       | True       | false     | Closed     | No state transition |
 * | 8        | RetrReqReceived       | False      | false     | Closed     | No state transition |
 * | 9        | RetrRespReceived      | False      | false     | Closed     | No state transition |
 * | 10       | DiscReqReceived       | False      | false     | Closed     | No state transition |
 * | 11       | HbReceived            | True       | false     | Closed     | No state transition |
 * | 12       | HbReceived            | False      | false     | Closed     | No state transition |
 * | 13       | DataReceived          | True       | false     | Closed     | No state transition |
 * | 14       | DataReceived          | False      | false     | Closed     | No state transition |
 * | 15       | RetrDataReceived      | True       | false     | Closed     | No state transition |
 * | 16       | RetrDataReceived      | False      | false     | Closed     | No state transition |
 * | 17       | SendHb                | False      | false     | Closed     | No state transition |
 * | 18       | Timeout               | False      | false     | Closed     | No state transition |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-560} sr_state_machine Events
 * @verifyReq{RASW-563} Process Connection State Machine Function
 */
TEST_P(StateClosedTestWithParam, srstmTest005VerifyStateClosed)
{
    // initialize the module and set the state to Closed
    InitModuleAndSetState(UT_SRSTM_MIN_NUM_CONN, sraty_kConnectionClosed);

    // verify if the state is set correctly
    EXPECT_EQ(sraty_kConnectionClosed, srstm_GetConnectionState(connection_id));

    if (event == srtyp_kConnEventNone)
    {
        // process state machine and verify that a fatal error is raised
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInternalError)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
        EXPECT_THROW(srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, false), std::invalid_argument);
    }
    else
    {
        // set expectations
        if (event == srtyp_kConnEventOpen)
        {
            EXPECT_CALL(srcor_mock, srcor_IsConnRoleServer(connection_id)).WillOnce(Return(is_server));
            EXPECT_CALL(srcor_mock, srcor_InitRaStaConnData(connection_id)).Times(1);
            if (!is_server)
            {
                EXPECT_CALL(srcor_mock, srcor_SendConnReqMessage(connection_id)).Times(1);
            }
            EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, _)).Times(1);
        }

        // process state machine
        srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, false);

        // verify if the state transition is correct
        EXPECT_EQ(next_state, srstm_GetConnectionState(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyStateClosed,
    StateClosedTestWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kConnEventNone, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventOpen, false, sraty_kConnectionDown, true),
        std::make_tuple(srtyp_kConnEventOpen, false, sraty_kConnectionStart, false),
        std::make_tuple(srtyp_kConnEventClose, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventSendData, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventConnReqReceived, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventConnRespReceived, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventRetrReqReceived, true, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventRetrReqReceived, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventRetrRespReceived, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventDiscReqReceived, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventHbReceived, true, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventHbReceived, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventDataReceived, true, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventDataReceived, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventRetrDataReceived, true, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventRetrDataReceived, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventSendHb, false, sraty_kConnectionClosed, false),
        std::make_tuple(srtyp_kConnEventTimeout, false, sraty_kConnectionClosed, false)
    ));

/**
 * @test        @ID{srstmTest006} Verify the State Down.
 *
 * @details     This test verifies the state Down and its transitions.
 *
 * Test steps:
 * - initialize the module for one connection
 * - set the current state to Down
 * - verify that the current state is set correctly
 * - set expectations for different events
 * - call the ProcessConnectionStateMachine function
 * - verify that the new state matches the expectation
 * .
 *
 * @testParameter
 * - Event: Event to be processed
 * - SN in Seq.: Indicates if the sequence number is in sequence
 * - Version Check passed: Indicates if the version check is passed
 * - Timeliness ok: Indicates if the timeliness is respected
 * - Next State: Expected state after transition
 * .
 * | Test Run | Test config                                                            |||| Expected values                  ||
 * |----------|-----------------------|------------|----------------------|---------------|------------|----------------------|
 * |          | Event                 | SN in Seq. | Version Check passed | Timeliness ok | Next State | Test Result          |
 * | 0        | None                  | False      | False                | True          | Down       | No state transition  |
 * | 1        | Open                  | False      | False                | True          | Closed     | Transition -> Closed |
 * | 2        | Close                 | False      | False                | True          | Closed     | Transition -> Closed |
 * | 3        | SendData              | False      | False                | True          | Closed     | Transition -> Closed |
 * | 4        | ConnReqReceived       | False      | True                 | True          | Start      | Transition -> Start  |
 * | 5        | ConnReqReceived       | False      | True                 | False         | Closed     | Transition -> Closed |
 * | 6        | ConnReqReceived       | False      | False                | True          | Closed     | Transition -> Closed |
 * | 7        | ConnRespReceived      | False      | False                | True          | Down       | No state transition  |
 * | 8        | RetrReqReceived       | True       | False                | True          | Down       | No state transition  |
 * | 9        | RetrReqReceived       | False      | False                | True          | Down       | No state transition  |
 * | 10       | RetrRespReceived      | False      | False                | True          | Down       | No state transition  |
 * | 11       | DiscReqReceived       | False      | False                | True          | Down       | No state transition  |
 * | 12       | HbReceived            | True       | False                | True          | Down       | No state transition  |
 * | 13       | HbReceived            | False      | False                | True          | Down       | No state transition  |
 * | 14       | DataReceived          | True       | False                | True          | Down       | No state transition  |
 * | 15       | DataReceived          | False      | False                | True          | Down       | No state transition  |
 * | 16       | RetrDataReceived      | True       | False                | True          | Down       | No state transition  |
 * | 17       | RetrDataReceived      | False      | False                | True          | Down       | No state transition  |
 * | 18       | SendHb                | False      | False                | True          | Down       | No state transition  |
 * | 19       | Timeout               | False      | False                | False         | Down       | No state transition  |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-560} sr_state_machine Events
 * @verifyReq{RASW-563} Process Connection State Machine Function
 */
TEST_P(StateDownTestWithParam, srstmTest006VerifyStateDown)
{
    // initialize the module and set the state to Down
    InitModuleAndSetState(UT_SRSTM_MIN_IDX_CONN, sraty_kConnectionDown);

    // verify if the state is set correctly
    EXPECT_EQ(sraty_kConnectionDown, srstm_GetConnectionState(connection_id));

    if (event == srtyp_kConnEventNone)
    {
        // process state machine and verify that a fatal error is raised
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInternalError)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
        EXPECT_THROW(srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, false), std::invalid_argument);
    }
    else
    {
        // set expectations
        if ((event == srtyp_kConnEventOpen) || (event == srtyp_kConnEventClose) || (event == srtyp_kConnEventSendData))
        {
            EXPECT_CALL(srcor_mock, srcor_CloseRedundancyChannel(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, sraty_kConnectionClosed, sraty_kDiscReasonNotInUse)).Times(1);
        }
        else if (event == srtyp_kConnEventConnReqReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_IsProtocolVersionAccepted(connection_id)).WillOnce(Return(is_version_check_passed));
            if (is_version_check_passed)
            {
                EXPECT_CALL(srcor_mock, srcor_ProcessReceivedMessage(connection_id)).WillOnce(Return(is_timeliness_respected));
                if (is_timeliness_respected)
                {
                    EXPECT_CALL(srcor_mock, srcor_SendConnRespMessage(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, sraty_kDiscReasonNotInUse)).Times(1);
                }
                else
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, sraty_kDiscReasonTimeout)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, sraty_kDiscReasonTimeout)).Times(1);
                }
            }
            else
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, sraty_kDiscReasonProtocolVersionError)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, sraty_kDiscReasonProtocolVersionError)).Times(1);
            }
        }
        else if (event == srtyp_kConnEventOpen || event == srtyp_kConnEventClose || event == srtyp_kConnEventSendData)
        {
            EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, sraty_kDiscReasonNotInUse)).Times(1);
        }

        // process state machine
        srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, false);

        // verify if the state transition is correct
        EXPECT_EQ(next_state, srstm_GetConnectionState(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyStateDown,
    StateDownTestWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kConnEventNone,               false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventOpen,               false,  sraty_kConnectionClosed,    false,  true),
        std::make_tuple(srtyp_kConnEventClose,              false,  sraty_kConnectionClosed,    false,  true),
        std::make_tuple(srtyp_kConnEventSendData,           false,  sraty_kConnectionClosed,    false,  true),
        std::make_tuple(srtyp_kConnEventConnReqReceived,    false,  sraty_kConnectionStart,     true,   true),
        std::make_tuple(srtyp_kConnEventConnReqReceived,    false,  sraty_kConnectionClosed,    true,   false),
        std::make_tuple(srtyp_kConnEventConnReqReceived,    false,  sraty_kConnectionClosed,    false,  true),
        std::make_tuple(srtyp_kConnEventConnRespReceived,   false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventRetrRespReceived,   false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventDiscReqReceived,    false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventHbReceived,         false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventDataReceived,       false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   true,   sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventSendHb,             false,  sraty_kConnectionDown,      false,  true),
        std::make_tuple(srtyp_kConnEventTimeout,            false,  sraty_kConnectionDown,      false,  false)
    ));

/**
 * @test        @ID{srstmTest007} Verify the State Start.
 *
 * @details     This test verifies the state Start and its transitions.
 *
 * Test steps:
 * - initialize the module for one connection
 * - set the current state to Start
 * - verify that the current state is set correctly
 * - set expectations for different events
 * - call the ProcessConnectionStateMachine function
 * - verify that the new state matches the expectation
 * .
 *
 * @testParameter
 * - Event: Event to be processed
 * - SN in Seq.: Indicates if the sequence number is in sequence
 * - CTS in Seq.: Indicates if the confirmed timestamp is in sequence
 * - Ver. Check passed: Indicates if the version check is passed
 * - Is server: Indicates if the system is configured as server
 * - Disc. Reason: Reason for disconnection
 * - Timeliness ok: Indicates if the timeliness is respected
 * - Next State: Expected state after transition
 * .
 * | Test Run | Test config                                                                                                        ||||||| Expected values                  ||
 * |----------|-----------------------|------------|-------------|-------------------|-----------|-----------------------|---------------|------------|----------------------|
 * |          | Event                 | SN in Seq. | CTS in Seq. | Ver. Check passed | Is server | Disc. Reason          | Timeliness ok | Next State | Test Result          |
 * | 0        | None                  | False      | False       | False             | False     | NotInUse              | True          | Start      | No state transition  |
 * | 1        | Open                  | False      | False       | False             | False     | ServiceNotAllowed     | True          | Closed     | Transition -> Closed |
 * | 2        | Close                 | False      | False       | False             | False     | UserRequest           | True          | Closed     | Transition -> Closed |
 * | 3        | SendData              | False      | False       | False             | False     | ServiceNotAllowed     | True          | Closed     | Transition -> Closed |
 * | 4        | ConnReqReceived       | False      | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 5        | ConnRespReceived      | False      | False       | False             | True      | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 6        | ConnRespReceived      | False      | False       | True              | False     | NotInUse              | True          | Up         | Transition -> Up     |
 * | 7        | ConnRespReceived      | False      | False       | True              | False     | Timeout               | False         | Closed     | Transition -> Closed |
 * | 8        | ConnRespReceived      | False      | False       | False             | False     | ProtocolVersionError  | True          | Closed     | Transition -> Closed |
 * | 9        | RetrReqReceived       | True       | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 10       | RetrReqReceived       | False      | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 11       | RetrRespReceived      | False      | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 12       | DiscReqReceived       | False      | False       | False             | False     | NotInUse              | True          | Closed     | Transition -> Closed |
 * | 13       | HbReceived            | True       | True        | False             | True      | NotInUse              | True          | Up         | Transition -> Up     |
 * | 14       | HbReceived            | True       | True        | False             | True      | Timeout               | False         | Closed     | Transition -> Closed |
 * | 15       | HbReceived            | True       | False       | False             | True      | ProtocolSequenceError | True          | Closed     | Transition -> Closed |
 * | 16       | HbReceived            | True       | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 17       | HbReceived            | False      | False       | False             | True      | SequenceNumberError   | True          | Closed     | Transition -> Closed |
 * | 18       | HbReceived            | False      | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 19       | DataReceived          | True       | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 20       | DataReceived          | False      | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 21       | RetrDataReceived      | True       | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 22       | RetrDataReceived      | False      | False       | False             | False     | UnexpectedMessage     | True          | Closed     | Transition -> Closed |
 * | 23       | SendHb                | False      | False       | False             | True      | NotInUse              | True          | Start      | No state transition  |
 * | 24       | SendHb                | False      | False       | False             | False     | NotInUse              | True          | Start      | No state transition  |
 * | 25       | Timeout               | False      | False       | False             | False     | Timeout               | False         | Closed     | Transition -> Closed |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-560} sr_state_machine Events
 * @verifyReq{RASW-563} Process Connection State Machine Function
 */
TEST_P(StateStartTestWithParam, srstmTest007VerifyStateStart)
{
    // initialize the module and set the state to Start
    InitModuleAndSetState(UT_SRSTM_MIN_IDX_CONN, sraty_kConnectionStart);

    // verify if the state is set correctly
    EXPECT_EQ(sraty_kConnectionStart, srstm_GetConnectionState(connection_id));

    if (event == srtyp_kConnEventNone)
    {
        // process state machine and verify that a fatal error is raised
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInternalError)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
        EXPECT_THROW(srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, is_cts_in_seq), std::invalid_argument);
    }
    else
    {
        // set expectations
        if (event == srtyp_kConnEventClose)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (   event == srtyp_kConnEventConnReqReceived ||
                    event == srtyp_kConnEventRetrReqReceived ||
                    event == srtyp_kConnEventRetrRespReceived ||
                    event == srtyp_kConnEventDataReceived ||
                    event == srtyp_kConnEventRetrDataReceived ||
                    event == srtyp_kConnEventTimeout)
        {
            EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventConnRespReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_IsConnRoleServer(connection_id)).WillOnce(Return(is_server));
            if (is_server)
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
            }
            else
            {
                EXPECT_CALL(srcor_mock, srcor_IsProtocolVersionAccepted(connection_id)).WillOnce(Return(is_version_check_passed));
                if (is_version_check_passed)
                {
                    EXPECT_CALL(srcor_mock, srcor_ProcessReceivedMessage(connection_id)).WillOnce(Return(is_timeliness_respected));
                    if (is_timeliness_respected)
                    {
                        EXPECT_CALL(srcor_mock, srcor_SendHbMessage(connection_id)).Times(1);
                    }
                    else
                    {
                        EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                        EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                    }
                }
                else
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                }
            }
        }
        else if (event == srtyp_kConnEventDiscReqReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_CloseRedundancyChannel(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
        }
        else if (event == srtyp_kConnEventHbReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_IsConnRoleServer(connection_id)).WillOnce(Return(is_server));
            if (is_sn_in_seq)
            {
                if (is_server && is_cts_in_seq)
                {
                    EXPECT_CALL(srcor_mock, srcor_ProcessReceivedMessage(connection_id)).WillOnce(Return(is_timeliness_respected));
                    if (!is_timeliness_respected)
                    {
                        EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                        EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                    }
                }
                else
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                }
            }
            else
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
            }
        }
        else if (event == srtyp_kConnEventOpen || event == srtyp_kConnEventSendData)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventSendHb)
        {
            EXPECT_CALL(srcor_mock, srcor_IsConnRoleServer(connection_id)).WillOnce(Return(is_server));
            if (is_server)
            {
                EXPECT_CALL(srcor_mock, srcor_SendHbMessage(connection_id)).Times(1);
            }
        }

        if (next_state != sraty_kConnectionStart)
        {
            EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, disc_reason)).Times(1);
        }

        // process state machine
        srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, is_cts_in_seq);

        // verify if the state transition is correct
        EXPECT_EQ(next_state, srstm_GetConnectionState(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyStateStart,
    StateStartTestWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kConnEventNone,               false,  false,  sraty_kConnectionStart,     false,  false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventOpen,               false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonServiceNotAllowed,     true),
        std::make_tuple(srtyp_kConnEventClose,              false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUserRequest,           true),
        std::make_tuple(srtyp_kConnEventSendData,           false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonServiceNotAllowed,     true),
        std::make_tuple(srtyp_kConnEventConnReqReceived,    false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventConnRespReceived,   false,  false,  sraty_kConnectionClosed,    false,  true,   sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventConnRespReceived,   false,  false,  sraty_kConnectionUp,        true,   false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventConnRespReceived,   false,  false,  sraty_kConnectionClosed,    true,   false,  sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventConnRespReceived,   false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonProtocolVersionError,  true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrRespReceived,   false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventDiscReqReceived,    false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   true,   sraty_kConnectionUp,        false,  true,   sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   true,   sraty_kConnectionClosed,    false,  true,   sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   false,  sraty_kConnectionClosed,    false,  true,   sraty_kDiscReasonProtocolSequenceError, true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventHbReceived,         false,  false,  sraty_kConnectionClosed,    false,  true,   sraty_kDiscReasonSequenceNumberError,   true),
        std::make_tuple(srtyp_kConnEventHbReceived,         false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventDataReceived,       false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   true,   false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventSendHb,             false,  false,  sraty_kConnectionStart,     false,  true,   sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventSendHb,             false,  false,  sraty_kConnectionStart,     false,  false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventTimeout,            false,  false,  sraty_kConnectionClosed,    false,  false,  sraty_kDiscReasonTimeout,               false)
    ));

/**
 * @test        @ID{srstmTest008} Verify the State Up.
 *
 * @details     This test verifies the state Up and its transitions.
 *
 * Test steps:
 * - initialize the module for one connection
 * - set the current state to Up
 * - verify that the current state is set correctly
 * - set expectations for different events
 * - call the ProcessConnectionStateMachine function
 * - verify that the new state matches the expectation
 * .
 *
 * @testParameter
 * - Event: Event to be processed
 * - SN in Seq.: Indicates if the sequence number is in sequence
 * - CTS in Seq.: Indicates if the confirmed timestamp is in sequence
 * - Is SN available: Indicates if the sequence number for retransmission is available
 * - Disc. Reason: Reason for disconnection
 * - Timeliness ok: Indicates if the timeliness is respected
 * - Next State: Expected state after transition
 * .
 * | Test Run | Test config                                                                                           |||||| Expected values                   ||
 * |----------|-----------------------|------------|-------------|-----------------|-----------------------|---------------|------------|-----------------------|
 * |          | Event                 | SN in Seq. | CTS in Seq. | Is SN available | Disc. Reason          | Timeliness ok | Next State | Test Result           |
 * | 0        | None                  | False      | False       | False           | NotInUse              | True          | Up         | No state transition   |
 * | 1        | Open                  | False      | False       | False           | ServiceNotAllowed     | True          | Closed     | Transition -> Closed  |
 * | 2        | Close                 | False      | False       | False           | UserRequest           | True          | Closed     | Transition -> Closed  |
 * | 3        | SendData              | False      | False       | False           | NotInUse              | True          | Up         | No state transition   |
 * | 4        | ConnReqReceived       | False      | False       | False           | UnexpectedMessage     | True          | Closed     | Transition -> Closed  |
 * | 5        | ConnRespReceived      | False      | False       | False           | UnexpectedMessage     | True          | Closed     | Transition -> Closed  |
 * | 6        | RetrReqReceived       | True       | False       | True            | NotInUse              | True          | Up         | No state transition   |
 * | 7        | RetrReqReceived       | True       | False       | True            | Timeout               | False         | Closed     | Transition -> Closed  |
 * | 8        | RetrReqReceived       | True       | False       | False           | RetransmissionFailed  | True          | Closed     | Transition -> Closed  |
 * | 9        | RetrReqReceived       | False      | False       | True            | NotInUse              | True          | RetrReq    | Transition -> RetrReq |
 * | 10       | RetrReqReceived       | False      | False       | False           | RetransmissionFailed  | True          | Closed     | Transition -> Closed  |
 * | 11       | RetrRespReceived      | False      | False       | False           | UnexpectedMessage     | True          | Closed     | Transition -> Closed  |
 * | 12       | DiscReqReceived       | False      | False       | False           | NotInUse              | True          | Closed     | Transition -> Closed  |
 * | 13       | HbReceived            | True       | True        | False           | NotInUse              | True          | Up         | No state transition   |
 * | 14       | HbReceived            | True       | True        | False           | Timeout               | False         | Closed     | Transition -> Closed  |
 * | 15       | HbReceived            | True       | False       | False           | ProtocolSequenceError | True          | Closed     | Transition -> Closed  |
 * | 16       | HbReceived            | False      | False       | False           | NotInUse              | True          | RetrReq    | Transition -> RetrReq |
 * | 17       | DataReceived          | True       | True        | False           | NotInUse              | True          | Up         | No state transition   |
 * | 18       | DataReceived          | True       | True        | False           | Timeout               | False         | Closed     | Transition -> Closed  |
 * | 19       | DataReceived          | True       | False       | False           | ProtocolSequenceError | True          | Closed     | Transition -> Closed  |
 * | 20       | DataReceived          | False      | False       | False           | NotInUse              | True          | RetrReq    | Transition -> RetrReq |
 * | 21       | RetrDataReceived      | True       | False       | False           | UnexpectedMessage     | True          | Closed     | Transition -> Closed  |
 * | 22       | RetrDataReceived      | False      | False       | False           | UnexpectedMessage     | True          | Closed     | Transition -> Closed  |
 * | 23       | SendHb                | False      | False       | False           | NotInUse              | True          | Up         | No state transition   |
 * | 24       | Timeout               | False      | False       | False           | Timeout               | False         | Closed     | Transition -> Closed  |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-560} sr_state_machine Events
 * @verifyReq{RASW-563} Process Connection State Machine Function
 */
TEST_P(StateUpTestWithParam, srstmTest008VerifyStateUp)
{
    // initialize the module and set the state to Up
    InitModuleAndSetState(UT_SRSTM_MIN_IDX_CONN, sraty_kConnectionUp);

    // verify if the state is set correctly
    EXPECT_EQ(sraty_kConnectionUp, srstm_GetConnectionState(connection_id));

    if (event == srtyp_kConnEventNone)
    {
        // process state machine and verify that a fatal error is raised
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInternalError)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
        EXPECT_THROW(srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, is_cts_in_seq), std::invalid_argument);
    }
    else
    {
        // set expectations
        if (event == srtyp_kConnEventOpen)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventClose)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventSendData)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDataMessage(connection_id)).Times(1);
        }
        else if (   event == srtyp_kConnEventConnReqReceived ||
                    event == srtyp_kConnEventConnRespReceived ||
                    event == srtyp_kConnEventRetrRespReceived ||
                    event == srtyp_kConnEventRetrDataReceived ||
                    event == srtyp_kConnEventTimeout)
        {
            EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventRetrReqReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_IsRetrReqSequenceNumberAvailable(connection_id)).WillOnce(Return(is_retr_sn_available));
            if (is_sn_in_seq && is_retr_sn_available)
            {
                EXPECT_CALL(srcor_mock, srcor_ProcessReceivedMessage(connection_id)).WillOnce(Return(is_timeliness_respected));
                if (is_timeliness_respected)
                {
                    EXPECT_CALL(srcor_mock, srcor_HandleRetrReq(connection_id)).Times(1);
                }
                else
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                }
            }
            else if (!is_sn_in_seq && is_retr_sn_available)
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedRxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_HandleRetrReq(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendRetrReqMessage(connection_id)).Times(1);
            }
            else
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
            }
        }
        else if (event == srtyp_kConnEventDiscReqReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_CloseRedundancyChannel(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
        }
        else if (event == srtyp_kConnEventHbReceived || event == srtyp_kConnEventDataReceived)
        {
            if (is_sn_in_seq)
            {
                if (is_cts_in_seq)
                {
                    EXPECT_CALL(srcor_mock, srcor_ProcessReceivedMessage(connection_id)).WillOnce(Return(is_timeliness_respected));
                    if (!is_timeliness_respected)
                    {
                        EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                        EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                    }
                }
                else
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                }
            }
            else
            {
                EXPECT_CALL(srcor_mock, srcor_ClearInputBufferMessagePendingFlag(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendRetrReqMessage(connection_id)).Times(1);
            }
        }
        else if (event == srtyp_kConnEventSendHb)
        {
            EXPECT_CALL(srcor_mock, srcor_SendHbMessage(connection_id)).Times(1);
        }

        if (next_state != sraty_kConnectionUp)
        {
            EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, disc_reason)).Times(1);
        }

        // process state machine
        srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, is_cts_in_seq);

        // verify if the state transition is correct
        EXPECT_EQ(next_state, srstm_GetConnectionState(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyStateUp,
    StateUpTestWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kConnEventNone,               false,  false,  sraty_kConnectionUp,                false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventOpen,               false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonServiceNotAllowed,     true),
        std::make_tuple(srtyp_kConnEventClose,              false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUserRequest,           true),
        std::make_tuple(srtyp_kConnEventSendData,           false,  false,  sraty_kConnectionUp,                false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventConnReqReceived,    false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventConnRespReceived,   false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   false,  sraty_kConnectionUp,                true,   sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   false,  sraty_kConnectionClosed,            true,   sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonRetransmissionFailed,  true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    false,  false,  sraty_kConnectionRetransRequest,    true,   sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonRetransmissionFailed,  true),
        std::make_tuple(srtyp_kConnEventRetrRespReceived,   false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventDiscReqReceived,    false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   true,   sraty_kConnectionUp,                false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   true,   sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonProtocolSequenceError, true),
        std::make_tuple(srtyp_kConnEventHbReceived,         false,  false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   true,   sraty_kConnectionUp,                false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   true,   sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonProtocolSequenceError, true),
        std::make_tuple(srtyp_kConnEventDataReceived,       false,  false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   true,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventSendHb,             false,  false,  sraty_kConnectionUp,                false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventTimeout,            false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false)
    ));

/**
 * @test        @ID{srstmTest009} Verify the State RetrReq.
 *
 * @details     This test verifies the state RetrReq and its transitions.
 *
 * Test steps:
 * - initialize the module for one connection
 * - set the current state to RetrReq
 * - verify that the current state is set correctly
 * - set expectations for different events
 * - call the ProcessConnectionStateMachine function
 * - verify that the new state matches the expectation
 * .
 *
 * @testParameter
 * - Event: Event to be processed
 * - SN in Seq.: Indicates if the sequence number is in sequence
 * - Is SN available: Indicates if the sequence number for retransmission is available
 * - Disc. Reason: Reason for disconnection
 * - Timeliness ok: Indicates if the timeliness is respected
 * - Next State: Expected state after transition
 * .
 * | Test Run | Test config                                                                              ||||| Expected values                   ||
 * |----------|-----------------------|------------|-----------------|-----------------------|---------------|------------|-----------------------|
 * |          | Event                 | SN in Seq. | Is SN available | Disc. Reason          | Timeliness ok | Next State | Test Result           |
 * | 0        | None                  | False      | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 1        | Open                  | False      | False           | ServiceNotAllowed     | True          | Closed     | Transition -> Closed  |
 * | 2        | Close                 | False      | False           | UserRequest           | True          | Closed     | Transition -> Closed  |
 * | 3        | SendData              | False      | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 4        | ConnReqReceived       | False      | False           | UnexpectedMessage     | True          | Closed     | Transition -> Closed  |
 * | 5        | ConnRespReceived      | False      | False           | UnexpectedMessage     | True          | Closed     | Transition -> Closed  |
 * | 6        | RetrReqReceived       | True       | True            | NotInUse              | True          | RetrReq    | No state transition   |
 * | 7        | RetrReqReceived       | True       | True            | Timeout               | False         | Closed     | Transition -> Closed  |
 * | 8        | RetrReqReceived       | True       | False           | RetransmissionFailed  | True          | Closed     | Transition -> Closed  |
 * | 9        | RetrReqReceived       | False      | True            | NotInUse              | True          | RetrReq    | No state transition   |
 * | 10       | RetrReqReceived       | False      | False           | RetransmissionFailed  | True          | Closed     | Transition -> Closed  |
 * | 11       | RetrRespReceived      | False      | False           | NotInUse              | True          | RetrRun    | Transition -> RetrRun |
 * | 12       | RetrRespReceived      | False      | False           | Timeout               | False         | Closed     | Transition -> Closed  |
 * | 13       | DiscReqReceived       | False      | False           | NotInUse              | True          | Closed     | Transition -> Closed  |
 * | 14       | HbReceived            | True       | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 15       | HbReceived            | False      | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 16       | DataReceived          | True       | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 17       | DataReceived          | False      | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 18       | RetrDataReceived      | True       | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 19       | RetrDataReceived      | False      | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 20       | SendHb                | False      | False           | NotInUse              | True          | RetrReq    | No state transition   |
 * | 21       | Timeout               | False      | False           | Timeout               | False         | Closed     | Transition -> Closed  |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-560} sr_state_machine Events
 * @verifyReq{RASW-563} Process Connection State Machine Function
 */
TEST_P(StateRetrReqTestWithParam, srstmTest009VerifyStateRetrReq)
{
    // initialize the module and set the state to RetrReq
    InitModuleAndSetState(UT_SRSTM_MIN_IDX_CONN, sraty_kConnectionRetransRequest);

    // verify if the state is set correctly
    EXPECT_EQ(sraty_kConnectionRetransRequest, srstm_GetConnectionState(connection_id));

    if (event == srtyp_kConnEventNone)
    {
        // process state machine and verify that a fatal error is raised
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInternalError)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
        EXPECT_THROW(srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, false), std::invalid_argument);
    }
    else
    {
        // set expectations
        if (event == srtyp_kConnEventOpen)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventClose)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventSendData)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDataMessage(connection_id)).Times(1);
        }
        else if (   event == srtyp_kConnEventConnReqReceived ||
                    event == srtyp_kConnEventConnRespReceived ||
                    event == srtyp_kConnEventTimeout)
        {
            EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventRetrReqReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_IsRetrReqSequenceNumberAvailable(connection_id)).WillOnce(Return(is_retr_sn_available));
            if (is_sn_in_seq && is_retr_sn_available)
            {
                EXPECT_CALL(srcor_mock, srcor_ProcessReceivedMessage(connection_id)).WillOnce(Return(is_timeliness_respected));
                if (is_timeliness_respected)
                {
                    EXPECT_CALL(srcor_mock, srcor_HandleRetrReq(connection_id)).Times(1);
                }
                else
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                }
            }
            else if (!is_sn_in_seq && is_retr_sn_available)
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedRxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_HandleRetrReq(connection_id)).Times(1);
            }
            else
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
            }
        }
        else if (event == srtyp_kConnEventRetrRespReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_ProcessReceivedMessage(connection_id)).WillOnce(Return(is_timeliness_respected));
            if (!is_timeliness_respected)
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
            }
        }
        else if (event == srtyp_kConnEventDiscReqReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_CloseRedundancyChannel(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
        }
        else if (event == srtyp_kConnEventSendHb)
        {
            EXPECT_CALL(srcor_mock, srcor_SendHbMessage(connection_id)).Times(1);
        }

        if (next_state != sraty_kConnectionRetransRequest)
        {
            EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, disc_reason)).Times(1);
        }

        // process state machine
        srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, false);

        // verify if the state transition is correct
        EXPECT_EQ(next_state, srstm_GetConnectionState(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyStateRetrReq,
    StateRetrReqTestWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kConnEventNone,               false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventOpen,               false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonServiceNotAllowed,     true),
        std::make_tuple(srtyp_kConnEventClose,              false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUserRequest,           true),
        std::make_tuple(srtyp_kConnEventSendData,           false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventConnReqReceived,    false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventConnRespReceived,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   sraty_kConnectionRetransRequest,    true,   sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   sraty_kConnectionClosed,            true,   sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   sraty_kConnectionClosed,            false,  sraty_kDiscReasonRetransmissionFailed,  true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    false,  sraty_kConnectionRetransRequest,    true,   sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonRetransmissionFailed,  true),
        std::make_tuple(srtyp_kConnEventRetrRespReceived,   false,  sraty_kConnectionRetransRunning,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrRespReceived,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventDiscReqReceived,    false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventHbReceived,         false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventDataReceived,       false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   true,   sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventSendHb,             false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventTimeout,            false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false)
    ));

/**
 * @test        @ID{srstmTest010} Verify the State RetrRun.
 *
 * @details     This test verifies the state RetrRun and its transitions.
 *
 * Test steps:
 * - initialize the module for one connection
 * - set the current state to RetrRun
 * - verify that the current state is set correctly
 * - set expectations for different events
 * - call the ProcessConnectionStateMachine function
 * - verify that the new state matches the expectation
 * .
 *
 * @testParameter
 * - Event: Event to be processed
 * - SN in Seq.: Indicates if the sequence number is in sequence
 * - CTS in Seq.: Indicates if the confirmed timestamp is in sequence
 * - Is SN available: Indicates if the sequence number for retransmission is available
 * - Disc. Reason: Reason for disconnection
 * - Timeliness ok: Indicates if the timeliness is respected
 * - Next State: Expected state after transition
 * .
 * | Test Run | Test config                                                                                           |||||| Expected values                   ||
 * |----------|-----------------------|------------|-------------|-----------------|-----------------------|---------------|------------|-----------------------|
 * |          | Event                 | SN in Seq. | CTS in Seq. | Is SN available | Disc. Reason          | Timeliness ok | Next State | Test Result           |
 * | 0        | None                  | False      | False       | False           | NotInUse              | true          | RetrRun    | No state transition   |
 * | 1        | Open                  | False      | False       | False           | ServiceNotAllowed     | true          | Closed     | Transition -> Closed  |
 * | 2        | Close                 | False      | False       | False           | UserRequest           | true          | Closed     | Transition -> Closed  |
 * | 3        | SendData              | False      | False       | False           | NotInUse              | true          | RetrRun    | No state transition   |
 * | 4        | ConnReqReceived       | False      | False       | False           | UnexpectedMessage     | true          | Closed     | Transition -> Closed  |
 * | 5        | ConnRespReceived      | False      | False       | False           | UnexpectedMessage     | true          | Closed     | Transition -> Closed  |
 * | 6        | RetrReqReceived       | True       | False       | False           | UnexpectedMessage     | true          | Closed     | Transition -> Closed  |
 * | 7        | RetrReqReceived       | False      | False       | True            | NotInUse              | true          | RetrReq    | Transition -> RetrReq |
 * | 8        | RetrReqReceived       | False      | False       | False           | RetransmissionFailed  | true          | Closed     | Transition -> Closed  |
 * | 9        | RetrRespReceived      | False      | False       | False           | UnexpectedMessage     | true          | Closed     | Transition -> Closed  |
 * | 10       | DiscReqReceived       | False      | False       | False           | NotInUse              | true          | Closed     | Transition -> Closed  |
 * | 11       | HbReceived            | True       | True        | False           | NotInUse              | true          | Up         | Transition -> Up      |
 * | 12       | HbReceived            | True       | True        | False           | Timeout               | false         | Closed     | Transition -> Closed  |
 * | 13       | HbReceived            | True       | False       | False           | ProtocolSequenceError | true          | Closed     | Transition -> Closed  |
 * | 14       | HbReceived            | False      | False       | False           | NotInUse              | true          | RetrReq    | Transition -> RetrReq |
 * | 15       | DataReceived          | True       | True        | False           | NotInUse              | true          | Up         | Transition -> Up      |
 * | 16       | DataReceived          | True       | True        | False           | Timeout               | false         | Closed     | Transition -> Closed  |
 * | 17       | DataReceived          | True       | False       | False           | ProtocolSequenceError | true          | Closed     | Transition -> Closed  |
 * | 18       | DataReceived          | False      | False       | False           | NotInUse              | true          | RetrReq    | Transition -> RetrReq |
 * | 19       | RetrDataReceived      | True       | True        | False           | NotInUse              | true          | RetrRun    | No state transition   |
 * | 20       | RetrDataReceived      | True       | True        | False           | Timeout               | false         | Closed     | Transition -> Closed  |
 * | 21       | RetrDataReceived      | True       | False       | False           | ProtocolSequenceError | true          | Closed     | Transition -> Closed  |
 * | 22       | RetrDataReceived      | False      | False       | False           | NotInUse              | true          | RetrReq    | Transition -> RetrReq |
 * | 23       | SendHb                | False      | False       | False           | NotInUse              | true          | RetrRun    | No state transition   |
 * | 24       | Timeout               | False      | False       | False           | Timeout               | false         | Closed     | Transition -> Closed  |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-560} sr_state_machine Events
 * @verifyReq{RASW-563} Process Connection State Machine Function
 */
TEST_P(StateRetrRunTestWithParam, srstmTest010VerifyStateRetrRun)
{
    // initialize the module and set the state to RetrRun
    InitModuleAndSetState(UT_SRSTM_MIN_IDX_CONN, sraty_kConnectionRetransRunning);

    // verify if the state is set correctly
    EXPECT_EQ(sraty_kConnectionRetransRunning, srstm_GetConnectionState(connection_id));

    if (event == srtyp_kConnEventNone)
    {
        // process state machine and verify that a fatal error is raised
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInternalError)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
        EXPECT_THROW(srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, is_cts_in_seq), std::invalid_argument);
    }
    else
    {
        // set expectations
        if (event == srtyp_kConnEventOpen)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventClose)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventSendData)
        {
            EXPECT_CALL(srcor_mock, srcor_SendDataMessage(connection_id)).Times(1);
        }
        else if (   event == srtyp_kConnEventConnReqReceived ||
                    event == srtyp_kConnEventConnRespReceived ||
                    event == srtyp_kConnEventRetrRespReceived ||
                    event == srtyp_kConnEventTimeout)
        {
            EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
        }
        else if (event == srtyp_kConnEventRetrReqReceived)
        {
            if (is_sn_in_seq)
            {
                EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
            }
            else
            {
                EXPECT_CALL(srcor_mock, srcor_IsRetrReqSequenceNumberAvailable(connection_id)).WillOnce(Return(is_retr_sn_available));
                if (is_retr_sn_available)
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedRxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_HandleRetrReq(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendRetrReqMessage(connection_id)).Times(1);
                }
                else
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                }
            }
        }
        else if (event == srtyp_kConnEventDiscReqReceived)
        {
            EXPECT_CALL(srcor_mock, srcor_CloseRedundancyChannel(connection_id)).Times(1);
            EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
        }
        else if (   event == srtyp_kConnEventHbReceived ||
                    event == srtyp_kConnEventDataReceived ||
                    event == srtyp_kConnEventRetrDataReceived)
        {
            if (is_sn_in_seq)
            {
                if (is_cts_in_seq)
                {
                    EXPECT_CALL(srcor_mock, srcor_ProcessReceivedMessage(connection_id)).WillOnce(Return(is_timeliness_respected));
                    if (!is_timeliness_respected)
                    {
                        EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                        EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                    }
                }
                else
                {
                    EXPECT_CALL(srcor_mock, srcor_UpdateConfirmedTxSequenceNumber(connection_id)).Times(1);
                    EXPECT_CALL(srcor_mock, srcor_SendDiscReqMessage(connection_id, disc_reason)).Times(1);
                }
            }
            else
            {
                EXPECT_CALL(srcor_mock, srcor_ClearInputBufferMessagePendingFlag(connection_id)).Times(1);
                EXPECT_CALL(srcor_mock, srcor_SendRetrReqMessage(connection_id)).Times(1);
            }
        }
        else if (event == srtyp_kConnEventSendHb)
        {
            EXPECT_CALL(srcor_mock, srcor_SendHbMessage(connection_id)).Times(1);
        }

        if (next_state != sraty_kConnectionRetransRunning)
        {
            EXPECT_CALL(srcor_mock, srcor_SendConnectionStateNotification(connection_id, next_state, disc_reason)).Times(1);
        }

        // process state machine
        srstm_ProcessConnectionStateMachine(connection_id, event, is_sn_in_seq, is_cts_in_seq);

        // verify if the state transition is correct
        EXPECT_EQ(next_state, srstm_GetConnectionState(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyStateRetrRun,
    StateRetrRunTestWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kConnEventNone,               false,  false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventOpen,               false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonServiceNotAllowed,     true),
        std::make_tuple(srtyp_kConnEventClose,              false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUserRequest,           true),
        std::make_tuple(srtyp_kConnEventSendData,           false,  false,  sraty_kConnectionRetransRunning,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventConnReqReceived,    false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventConnRespReceived,   false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    true,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    false,  false,  sraty_kConnectionRetransRequest,    true,   sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrReqReceived,    false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonRetransmissionFailed,  true),
        std::make_tuple(srtyp_kConnEventRetrRespReceived,   false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonUnexpectedMessage,     true),
        std::make_tuple(srtyp_kConnEventDiscReqReceived,    false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   true,   sraty_kConnectionUp,                false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   true,   sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventHbReceived,         true,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonProtocolSequenceError, true),
        std::make_tuple(srtyp_kConnEventHbReceived,         false,  false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   true,   sraty_kConnectionUp,                false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   true,   sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventDataReceived,       true,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonProtocolSequenceError, true),
        std::make_tuple(srtyp_kConnEventDataReceived,       false,  false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   true,   true,   sraty_kConnectionRetransRunning,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   true,   true,   sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   true,   false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonProtocolSequenceError, true),
        std::make_tuple(srtyp_kConnEventRetrDataReceived,   false,  false,  sraty_kConnectionRetransRequest,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventSendHb,             false,  false,  sraty_kConnectionRetransRunning,    false,  sraty_kDiscReasonNotInUse,              true),
        std::make_tuple(srtyp_kConnEventTimeout,            false,  false,  sraty_kConnectionClosed,            false,  sraty_kDiscReasonTimeout,               false)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

void InitModuleAndSetState(uint32_t connection_id, sraty_ConnectionStates state)
{
    extern sraty_ConnectionStates srstm_connection_states[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS];

    srstm_Init(UT_SRSTM_MIN_NUM_CONN);
    srstm_connection_states[connection_id] = state;
}
