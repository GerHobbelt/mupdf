//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"
#include <glog/logging.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

#ifdef __cplusplus
extern "C" {
#endif

int run_all_tests_glog(void);

#ifdef __cplusplus
}
#endif
