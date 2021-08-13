// Copyright (c) 2019, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "common/gyro.h"
#include "serialization.h"

template<> struct is_basic_type<cryptonote::gyro_type> { typedef boost::true_type type; };

template <template <bool> class Archive>
inline bool do_serialize(Archive<false>& ar, cryptonote::gyro_type &g)
{
  cryptonote::gyro_raw_type raw;
  ar.serialize_varint(raw.H);
  if (!ar.stream().good())
    return false;
  ar.serialize_varint(raw.L);
  if (!ar.stream().good())
    return false;
  ar.serialize_varint(raw.h);
  if (!ar.stream().good())
    return false;
  ar.serialize_varint(raw.l);
  if (!ar.stream().good())
    return false;
  ar.serialize_blob(raw.v, sizeof(raw.v));
  if (!ar.stream().good())
    return false;
  g.from_raw(raw);
  return true;
}

template <template <bool> class Archive>
inline bool do_serialize(Archive<true>& ar, cryptonote::gyro_type &g)
{
  if (!ar.stream().good())
    return false;
  cryptonote::gyro_raw_type raw;
  g.to_raw(raw);
  ar.serialize_varint(raw.H);
  ar.serialize_varint(raw.L);
  ar.serialize_varint(raw.h);
  ar.serialize_varint(raw.l);
  ar.serialize_blob(raw.v, sizeof(raw.v));
  if (!ar.stream().good())
    return false;
  return true;
}

