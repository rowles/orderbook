#pragma once

#include <cstdint>
#include <sstream>

namespace agr {

using price_type = std::uint64_t;
using quantity_type = std::uint64_t;

enum message_type { trade, quote, cancel, info };

enum side_type { buy, sell };

struct tick {
  std::uint64_t seq_num{0};

  message_type message{info};
  std::uint64_t timestamp{0};

  //  char[20] symbol;
  //  char[20] exchange;

  price_type price{0};
  quantity_type quantity{0};
  side_type side{};

  inline std::string to_string() const noexcept {
    std::stringstream ss{};
    ss << "<Seq:" << seq_num << " Type:" << message << ">\n";
    return ss.str();
  }

  // std::string to_json() const noexcept {}
};

} // namespace agr
