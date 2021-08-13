#ifndef __gyro_raw_h__
#define __gyro_raw_h__

#pragma once

#include "cryptonote_config.h"

namespace cryptonote
{
  struct gyro_raw_type
  {
    uint64_t H;
    uint64_t L;
    uint64_t h;
    uint64_t l;
    int8_t v[GYRO_NUMBER_OF_DIMENSIONALS];
  };
}

#endif
