#ifndef __gyro_h__
#define __gyro_h__

#pragma once

#include <string>

#include "gyro_raw_type.h"
#include "spin.h"

namespace cryptonote
{

  const boost::multiprecision::int128_t int128max =
    std::numeric_limits<boost::multiprecision::int128_t>::max();
  const boost::multiprecision::int128_t int128min =
    std::numeric_limits<boost::multiprecision::int128_t>::min();

  struct gyro_type
  {
    boost::multiprecision::uint128_t M, m;
    int8_t v[GYRO_NUMBER_OF_DIMENSIONALS];
    
    gyro_type()
      : M(1), m(1)
    {
      memset(v, 0, GYRO_NUMBER_OF_DIMENSIONALS);
      v[0] = 1;
    }
    
    gyro_type(const gyro_type& g)
      : M(g.M), m(g.m)
    {
      memcpy(v, g.v, GYRO_NUMBER_OF_DIMENSIONALS);
    }
    
    gyro_type(const spin_type& s)
      : M(s.m), m(s.m)
    {
      memcpy(v, s.v, GYRO_NUMBER_OF_DIMENSIONALS);
    }

    void normalize();
    boost::multiprecision::uint128_t calc_vector_module();
    boost::multiprecision::uint128_t get_module();
    
    gyro_type& operator = (const gyro_type& g);
    gyro_type& operator = (const spin_type& s);

    gyro_type& operator += (const gyro_type& g);
    gyro_type& operator -= (const gyro_type& g);

    gyro_type& operator += (const spin_type& s);
    gyro_type& operator -= (const spin_type& s);

    bool operator == (const gyro_type& g);
    bool operator != (const gyro_type& g);

    void from_raw(const gyro_raw_type& raw);
    void to_raw(gyro_raw_type& ret) const;
  };

  inline gyro_type operator - (const gyro_type& a, const gyro_type& b)
  {
    gyro_type r(a);
    r -= b;
    return r;
  }

  inline gyro_type operator + (const gyro_type& a, const gyro_type& b)
  {
    gyro_type r(a);
    r += b;
    return r;
  }
  
  inline gyro_type operator - (const gyro_type& a, const spin_type& b)
  {
    gyro_type r(a);
    r -= b;
    return r;
  }

  inline gyro_type operator + (const gyro_type& a, const spin_type& b)
  {
    gyro_type r(a);
    r += b;
    return r;
  }
  
  std::string hex(boost::multiprecision::uint128_t v);
}

#endif
