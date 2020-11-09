#pragma once
#include "tick.h"

#include <map>
#include <vector>

namespace agr {

struct order {
  std::uint64_t seq_num;
  std::uint64_t quantity;
};

struct price_level {
  std::vector<order> orders;
};

class orderbook {
public:
  void add_tick(const tick &t) {
    if (t.side == buy) {

      if (t.price > best_bid) best_bid = t.price;
      if (bid_book.count(t.price) == 0) {
        // new price level
        bid_book[t.price] = {{{t.seq_num, t.quantity}}};
      } else {
        bid_book[t.price].orders.push_back({t.seq_num, t.quantity});
      }
    } else if (t.side == sell) {

      if (t.price < best_offer) best_offer = t.price;
      if (ask_book.count(t.price) == 0) {
        // new price level
        ask_book[t.price] = {{{t.seq_num, t.quantity}}};
      } else {
        ask_book[t.price].orders.push_back({t.seq_num, t.quantity});
      }
    }
  }
  void undo();
  void is_locked() const;
  void is_crossed() const;

  std::string to_string() const noexcept {
    std::stringstream ss{};
    ss << "bb:" << best_bid << " bo:" << best_offer << '\n'; 
    ss << "price levels bid:" << bid_book.size() << " ask:" << ask_book.size() << '\n'; 
    return ss.str();
  }

  // std::string to_json(resolution)
  // top of book
  // n price levels
  // full depth of book

private:
  std::map<price_type, price_level> ask_book{};
  std::map<price_type, price_level> bid_book{};

  price_type best_bid{std::numeric_limits<std::uint64_t>::min()};
  price_type best_offer{std::numeric_limits<std::uint64_t>::max()};
};

} // namespace agr
