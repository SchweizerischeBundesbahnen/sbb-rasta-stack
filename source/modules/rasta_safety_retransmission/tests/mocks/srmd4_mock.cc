/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srmd4_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the srmd4 module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srmd4_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
srmd4Mock * srmd4Mock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
srmd4Mock::srmd4Mock() {
  instance = this;
}

srmd4Mock::~srmd4Mock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void srmd4_CalculateMd4(const srcty_Md4InitValue md4_initial_value, const uint16_t data_size, const uint8_t * data_buffer, srmd4_Md4 * calculated_md4){
    ASSERT_NE(srmd4Mock::getInstance(), nullptr) << "Mock object not initialized!";
    srmd4Mock::getInstance()->srmd4_CalculateMd4(md4_initial_value, data_size, data_buffer, calculated_md4);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
