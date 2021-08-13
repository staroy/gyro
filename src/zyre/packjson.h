#ifndef __zyre_packjson_h__
#define __zyre_packjson_h__

#include <string>

namespace zyre
{
  bool pack2json(const std::string& data, std::size_t& off, std::string& out);
  bool json2pack(const std::string& data, std::string& out);
}

#endif
