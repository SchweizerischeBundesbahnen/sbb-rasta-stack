/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file rasys_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the rasys module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasys_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
rasysMock * rasysMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
rasysMock::rasysMock() {
  instance = this;
}

rasysMock::~rasysMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  uint32_t rasys_GetTimerValue(void){
    if(rasysMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return rasysMock::getInstance()->rasys_GetTimerValue();
  }
  uint32_t rasys_GetTimerGranularity(void){
    if(rasysMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return rasysMock::getInstance()->rasys_GetTimerGranularity();
  }
  uint32_t rasys_GetRandomNumber(void){
    if(rasysMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return rasysMock::getInstance()->rasys_GetRandomNumber();
  }
  void rasys_FatalError(const radef_RaStaReturnCode error_reason){
    ASSERT_NE(rasysMock::getInstance(), nullptr) << "Mock object not initialized!";
    rasysMock::getInstance()->rasys_FatalError(error_reason);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
