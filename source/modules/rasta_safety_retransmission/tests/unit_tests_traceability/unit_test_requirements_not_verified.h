/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_requirements_not_verified.h
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief This file lists and justifies all requirements of of the SafRetL package and the RaSTA Common package which are not covered by unit tests.
 *
 * The SafRetL package and RaSTA Common package are implemented as SIL4 software. Therefore all requirements from "RaSTA-018 - Software Architecture
 * Specification / Software Design Specification" and from "RaSTA-020 - Software Interface Specification" must be verified. Most of them are verified by
 * unit tests. Some of the requirements can not be verified by unit tests. These are listed and justified in this file.
 *
 * <b>Requirements from "RaSTA-018 - Software Architecture Specification / Software Design Specification" not covered by unit tests:</b>
 *
 * The following requirement defines the safety integrity level of the SafRetL package to SIL4. This can't be proved with a unit test. It is achieved by
 * a SIL4 conform development process:
 * @verifyReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 *
 * The following requirements are linked to dynamic behaviour (sequence diagrams). These requirements can't be tested with unit tests because of the
 * interaction between different components. Therefore they will be verified on integration test level:
 * @verifyReq{RASW-735} Init SafRetL Sequence
 * @verifyReq{RASW-737} Open Connection Sequence
 * @verifyReq{RASW-739} Close Connection Sequence
 * @verifyReq{RASW-741} Send Data Sequence
 * @verifyReq{RASW-743} Read Data Sequence
 * @verifyReq{RASW-745} Get Connection State Sequence
 * @verifyReq{RASW-747} Message Received Notification Sequence
 * @verifyReq{RASW-749} Connection State Notification Sequence
 * @verifyReq{RASW-751} Redundancy Diagnostic Notification Sequence
 * @verifyReq{RASW-753} Check Timings Sequence
 * @verifyReq{RASW-755} Process State Machine Receive ConnReq Message Sequence
 * @verifyReq{RASW-757} Process State Machine Receive ConnResp Message Sequence
 * @verifyReq{RASW-759} Process State Machine Receive Data Message (in Sequence) Sequence
 * @verifyReq{RASW-761} Process State Machine Receive RetrReq Message (in Sequence) Sequence
 * @verifyReq{RASW-763} Process State Machine Receive RetrReq Message (not in Sequence) Sequence
 * @verifyReq{RASW-765} Send Pending Messages Sequence
 * @verifyReq{RASW-767} Send Disconnection Request Message Sequence
 * @verifyReq{RASW-769} Received Message Polling Sequence
 * @verifyReq{RASW-771} Receive Message Sequence
 * @verifyReq{RASW-773} Check Timings Message Timeout / Heartbeat Sequence
 * @verifyReq{RASW-775} Process Received Message Sequence
 * @verifyReq{RASW-777} Handle Retransmission Request Sequence
 *
 * <b>Requirements from "RaSTA-020 - Software Interface Specification" not covered by unit tests:</b>
 *
 * There are no not covered requirements form the software interface specification.
 */
