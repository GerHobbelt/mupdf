#ifndef __ICEORYXVERSIONS__
#define __ICEORYXVERSIONS__

#define ICEORYX_VERSION_MAJOR    2
#define ICEORYX_VERSION_MINOR    90
#define ICEORYX_VERSION_PATCH    0
#define ICEORYX_VERSION_TWEAK    1

#define ICEORYX_LATEST_RELEASE_VERSION    "2.90.0.1"
#define ICEORYX_BUILDDATE                 "2023-02-16"
#define ICEORYX_SHA1                      "XXXXXXXXXXXXXXX"

#include "iceoryx_posh/internal/log/posh_logging.hpp"

#define ICEORYX_PRINT_BUILDINFO()     iox::LogInfo() << "Built: " << ICEORYX_BUILDDATE;

#endif
