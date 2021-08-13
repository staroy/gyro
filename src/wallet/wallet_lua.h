#pragma once

#include "lua.hpp"
#include "sol/sol.hpp"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "lwallet"

namespace lwallet
{
  bool reg(sol::state_view& lua);
} /* lwallet */
