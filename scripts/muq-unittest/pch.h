//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "mupdf/fitz/context.h"
#include "mupdf/assertions.h"
#include "mupdf/fitz/geometry.h"

#include "gtest/gtest.h"

#include <iostream>
#include <sstream>

using namespace std;

using ::testing::InitGoogleTest;
using ::testing::TestEventListeners;
using ::testing::TestWithParam;
using ::testing::UnitTest;
using ::testing::Test;
using ::testing::Values;
