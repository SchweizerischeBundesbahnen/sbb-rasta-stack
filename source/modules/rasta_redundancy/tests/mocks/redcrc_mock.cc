/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcrc_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the redcrc module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redcrc_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
redcrcMock * redcrcMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
redcrcMock::redcrcMock() {
  instance = this;
}

redcrcMock::~redcrcMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void redcrc_Init(const redcty_CheckCodeType configured_check_code_type){
    ASSERT_NE(redcrcMock::getInstance, nullptr) << "Mock object not initialized!";
    redcrcMock::getInstance()->redcrc_Init(configured_check_code_type);
  }
  void redcrc_CalculateCrc(const uint16_t data_size, const uint8_t * data_buffer, uint32_t * calculated_crc){
    ASSERT_NE(redcrcMock::getInstance, nullptr) << "Mock object not initialized!";
    redcrcMock::getInstance()->redcrc_CalculateCrc(data_size, data_buffer, calculated_crc);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
