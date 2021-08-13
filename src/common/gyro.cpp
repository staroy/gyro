#include "gyro.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "gyro"

namespace cryptonote {

  void gyro_type::normalize()
  {
    boost::multiprecision::uint128_t Vm = calc_vector_module();
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      v[n] = (boost::multiprecision::int128_t(MEDIUM_MODULE_OF_DIMENSIONALS_VECTOR)*v[n]/Vm).convert_to<int8_t>();
    m *= MEDIUM_MODULE_OF_DIMENSIONALS_VECTOR;
    m /= calc_vector_module();
  }

  boost::multiprecision::uint128_t gyro_type::calc_vector_module()
  {
    return cryptonote::calc_vector_module(v);
  }
  
  boost::multiprecision::uint128_t gyro_type::get_module()
  {
    return cryptonote::get_module(m, v);
  }

  gyro_type& gyro_type::operator = (const gyro_type& g)
  {
    M = g.M;
    m = g.m;
    memcpy(v, g.v, GYRO_NUMBER_OF_DIMENSIONALS);
    return *this;
  }
  
  gyro_type& gyro_type::operator = (const spin_type& s)
  {
    M = m = cryptonote::get_module(s.m, s.v);
    memcpy(v, s.v, GYRO_NUMBER_OF_DIMENSIONALS);
    return *this;
  }

  gyro_type& gyro_type::operator += (const gyro_type& g)
  {
    M += g.M;
    boost::multiprecision::uint256_t S = 1;
    boost::multiprecision::int256_t min = int8min, max = int8max, V[GYRO_NUMBER_OF_DIMENSIONALS];
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
    {
      auto a = (boost::multiprecision::int256_t(m)*int32_t(v[n]) + boost::multiprecision::int256_t(g.m)*int32_t(g.v[n])).convert_to<boost::multiprecision::int256_t>();
      V[n] = a; if(a > max) max = a; if(a < min) min = a;
      S += (a * a).convert_to<boost::multiprecision::uint256_t>();
    }
    boost::multiprecision::int256_t Kmin = min / int8min;
    boost::multiprecision::int256_t Kmax = max / int8max;
    boost::multiprecision::int256_t K = (Kmin > Kmax) ? Kmin : Kmax;
    S = sqrt(S);
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      v[n] = (V[n]/K).convert_to<int8_t>();
    m = (S / calc_vector_module()).convert_to<boost::multiprecision::uint128_t>();
    return *this;
  }
  
  gyro_type& gyro_type::operator -= (const gyro_type& g)
  {
    if(M > g.M)
      M -= g.M;
    else
      M = 0;
    boost::multiprecision::uint256_t S = 1;
    boost::multiprecision::int256_t min = int8min, max = int8max, V[GYRO_NUMBER_OF_DIMENSIONALS];
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
    {
      auto a = (boost::multiprecision::int256_t(m)*int32_t(v[n]) - boost::multiprecision::int256_t(g.m)*int32_t(g.v[n])).convert_to<boost::multiprecision::int256_t>();
      V[n] = a; if(a > max) max = a; if(a < min) min = a;
      S += (a * a).convert_to<boost::multiprecision::uint256_t>();
    }
    boost::multiprecision::int256_t Kmin = min / int8min;
    boost::multiprecision::int256_t Kmax = max / int8max;
    boost::multiprecision::int256_t K = (Kmin > Kmax) ? Kmin : Kmax;
    S = sqrt(S);
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      v[n] = (V[n]/K).convert_to<int8_t>();
    m = (S / calc_vector_module()).convert_to<boost::multiprecision::uint128_t>();
    return *this;
  }
  
  gyro_type& gyro_type::operator += (const spin_type& s)
  {
    boost::multiprecision::uint256_t S = 1;
    boost::multiprecision::int256_t min = int8min, max = int8max, V[GYRO_NUMBER_OF_DIMENSIONALS];
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
    {
      auto a = (boost::multiprecision::int256_t(m)*int32_t(v[n]) + boost::multiprecision::int256_t(s.m)*int32_t(s.v[n])).convert_to<boost::multiprecision::int256_t>();
      V[n] = a; if(a > max) max = a; if(a < min) min = a;
      S += (a * a).convert_to<boost::multiprecision::uint256_t>();
    }
    boost::multiprecision::int256_t Kmin = min / int8min;
    boost::multiprecision::int256_t Kmax = max / int8max;
    boost::multiprecision::int256_t K = (Kmin > Kmax) ? Kmin : Kmax;
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      v[n] = (V[n]/K).convert_to<int8_t>();
    m = sqrt(S).convert_to<boost::multiprecision::uint128_t>() / calc_vector_module();
    M += cryptonote::get_module(s.m, s.v);
    return *this;
  }
  
  gyro_type& gyro_type::operator -= (const spin_type& s)
  {
    boost::multiprecision::uint256_t S = 1;
    boost::multiprecision::int256_t min = int8min, max = int8max, V[GYRO_NUMBER_OF_DIMENSIONALS];
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
    {
      auto a = (boost::multiprecision::int256_t(m)*int32_t(v[n]) - boost::multiprecision::int256_t(s.m)*int32_t(s.v[n])).convert_to<boost::multiprecision::int256_t>();
      V[n] = a; if(a > max) max = a; if(a < min) min = a;
      S += (a * a).convert_to<boost::multiprecision::uint256_t>();
    }
    boost::multiprecision::int256_t Kmin = min / int8min;
    boost::multiprecision::int256_t Kmax = max / int8max;
    boost::multiprecision::int256_t K = (Kmin > Kmax) ? Kmin : Kmax;
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      v[n] = (V[n]/K).convert_to<int8_t>();
    m = sqrt(S).convert_to<boost::multiprecision::uint128_t>() / calc_vector_module();
    auto Sm = cryptonote::get_module(s.m, s.v);
    if(M > Sm)
      M -= Sm;
    else
      M = 1;
    return *this;
  }

  bool gyro_type::operator == (const gyro_type& g)
  {
    if(M != g.M)
      return false;
    if(m != g.m)
      return false;
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      if(v[n] != g.v[n])
        return false;
    return true;
  }
  
  bool gyro_type::operator != (const gyro_type& g)
  {
    if(M != g.M)
      return true;
    if(m != g.m)
      return true;
    for(size_t n=0; n<GYRO_NUMBER_OF_DIMENSIONALS; n++)
      if(v[n] != g.v[n])
        return true;
    return false;
  }

  void gyro_type::from_raw(const gyro_raw_type& raw)
  {
    M = raw.H;
    M = (M << 64) + raw.L;
    m = raw.h;
    m = (m << 64) + raw.l;
    memcpy(v, raw.v, GYRO_NUMBER_OF_DIMENSIONALS);
  }

  void gyro_type::to_raw(gyro_raw_type& ret) const
  {
    ret.L = (M & 0xffffffffffffffff).convert_to<uint64_t>();
    ret.H = ((M >> 64) & 0xffffffffffffffff).convert_to<uint64_t>();
    ret.l = (m & 0xffffffffffffffff).convert_to<uint64_t>();
    ret.h = ((m >> 64) & 0xffffffffffffffff).convert_to<uint64_t>();
    memcpy(ret.v, v, GYRO_NUMBER_OF_DIMENSIONALS);
  }

  std::string hex(boost::multiprecision::uint128_t v)
  {
    static const char chars[] = "0123456789abcdef";
    std::string s;
    while (v > 0)
    {
      s.push_back(chars[(v & 0xf).convert_to<unsigned>()]);
      v >>= 4;
    }
    if (s.empty())
      s += "0";
    std::reverse(s.begin(), s.end());
    return "0x" + s;
  }

}
