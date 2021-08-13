#include "spin.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "spin"

namespace cryptonote {

  boost::multiprecision::uint128_t calc_vector_module(const int8_t * v)
  {
    boost::multiprecision::uint256_t S = 1;
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      S += int64_t(v[n]) * int64_t(v[n]);
    return sqrt(S).convert_to<boost::multiprecision::uint128_t>();
  }
  
  boost::multiprecision::uint128_t get_module(const boost::multiprecision::uint128_t& m, const int8_t * v)
  {
    return calc_vector_module(v) * m / MEDIUM_MODULE_OF_DIMENSIONALS_VECTOR + 1;
  }

  boost::multiprecision::uint128_t get_module(const uint64_t& m, const int8_t * v)
  {
    return calc_vector_module(v) * m / MEDIUM_MODULE_OF_DIMENSIONALS_VECTOR + 1;
  }

  uint64_t spin_type::get_module()
  {
    return cryptonote::get_module(m, v).convert_to<uint64_t>();
  }

  bool spin_type::operator == (const spin_type& s)
  {
    if(m != s.m)
      return false;
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      if(v[n] != s.v[n])
        return false;
    return true;
  }
  
  bool spin_type::operator != (const spin_type& s)
  {
    if(m != s.m)
      return true;
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      if(v[n] != s.v[n])
        return true;
    return false;
  }

}