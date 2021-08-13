#ifndef __spin_type_h__
#define __spin_type_h__

#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include "serialization/binary_archive.h"
#include "serialization/variant.h"
#include "cryptonote_config.h"

namespace cryptonote
{
  const int64_t int8max = std::numeric_limits<int8_t>::max();
  const int64_t int8min = std::numeric_limits<int8_t>::min();

  const uint64_t MEDIUM_MODULE_OF_DIMENSIONALS_VECTOR =
    (sqrt(boost::multiprecision::uint256_t((64*64)*GYRO_NUMBER_OF_DIMENSIONALS))).convert_to<uint64_t>();

  boost::multiprecision::uint128_t calc_vector_module(const int8_t * v);
  boost::multiprecision::uint128_t get_module(const boost::multiprecision::uint128_t& m, const int8_t * v);
  boost::multiprecision::uint128_t get_module(const uint64_t& m, const int8_t * v);

  struct spin_type
  {
    uint64_t m;
    int8_t v[GYRO_NUMBER_OF_DIMENSIONALS];

    BEGIN_SERIALIZE()
      VARINT_FIELD(m)
      FIELDS(v)
    END_SERIALIZE()

    spin_type()
      : m(1)
    {
      memset(v, 0, GYRO_NUMBER_OF_DIMENSIONALS);
      v[0] = 1;
    }

    uint64_t get_module();

    bool operator == (const spin_type& s);
    bool operator != (const spin_type& s);
  };
}

#endif
