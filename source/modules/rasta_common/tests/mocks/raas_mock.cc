/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file raas_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the raas module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "raas_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
raasMock * raasMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
raasMock::raasMock() {
  instance = this;
}

raasMock::~raasMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void raas_AssertNotNull(const void * pointer, const radef_RaStaReturnCode error_reason){
    ASSERT_NE(raasMock::getInstance(), nullptr) << "Mock object not initialized!";
    raasMock::getInstance()->raas_AssertNotNull(pointer, error_reason);
  }
  void raas_AssertTrue(const bool condition, const radef_RaStaReturnCode error_reason){
    ASSERT_NE(raasMock::getInstance(), nullptr) << "Mock object not initialized!";
    raasMock::getInstance()->raas_AssertTrue(condition, error_reason);
  }
  void raas_AssertU8InRange(const uint8_t value, const uint8_t min_value, const uint8_t max_value, const radef_RaStaReturnCode error_reason){
    ASSERT_NE(raasMock::getInstance(), nullptr) << "Mock object not initialized!";
    raasMock::getInstance()->raas_AssertU8InRange(value, min_value, max_value, error_reason);
  }
  void raas_AssertU16InRange(const uint16_t value, const uint16_t min_value, const uint16_t max_value, const radef_RaStaReturnCode error_reason){
    ASSERT_NE(raasMock::getInstance(), nullptr) << "Mock object not initialized!";
    raasMock::getInstance()->raas_AssertU16InRange(value, min_value, max_value, error_reason);
  }
  void raas_AssertU32InRange(const uint32_t value, const uint32_t min_value, const uint32_t max_value, const radef_RaStaReturnCode error_reason){
    ASSERT_NE(raasMock::getInstance(), nullptr) << "Mock object not initialized!";
    raasMock::getInstance()->raas_AssertU32InRange(value, min_value, max_value, error_reason);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
