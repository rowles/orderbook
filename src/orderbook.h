#pragma once
#include "tick.h"

#include <map>
#include <sstream>
#include <vector>
#include <functional>


// order pool
namespace agr {

struct order {
  std::uint64_t seq_num;
  // char[20] order_id;
  std::uint64_t quantity;
  
  std::string to_string() const {
    std::stringstream ss{};

    ss << "[seq:" << seq_num << ", quantity:" << quantity << "]"; 

    return ss.str();
  }
};

struct price_level {
  std::vector<order> orders;

  std::string to_string() const {
    std::stringstream ss{};

    for (const auto& o : orders) {
      ss << o.to_string() << ", ";
    }

    return ss.str();
  }
};

// TODO: cancel, modify, market

class orderbook {
public:
  void add_tick(const tick &t) {
    if (t.side == buy) {
      /*
      if (t.price > best_bid) best_bid = t.price;
      if (bid_book.count(t.price) == 0) {
        // new price level
        bid_book[t.price] = {{{t.seq_num, t.quantity}}};
      } else {
        bid_book[t.price].orders.push_back({t.seq_num, t.quantity});
      }*/
      do_buy(t);
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

    ss << "Ask Book\n";
    for (const auto& [price, orders] : ask_book) {
      ss << price << " -> " << orders.to_string() << "\n";
    }
    ss << "Bid Book\n";
    for (const auto& [price, orders] : bid_book) {
      ss << price << " -> " << orders.to_string() << "\n";
    }

    return ss.str();
  }

  template<typename S>
  quantity_type execute(const tick &t) {
    quantity_type rem = t.quantity;

    S* side_book;

    if (t.side == buy) {
      side_book = &ask_book;
    } else {
      side_book = &bid_book;
    }



    return rem;
  }

  void do_buy(const tick &t) {
    if (t.price <= best_offer) {
      // crosses spread
  
      quantity_type rem = t.quantity;

      auto pl_it = ask_book.begin();

      while (pl_it != ask_book.end() && rem > 0) {
      //for (auto& [pl, orders] : ask_book) {
        auto it = pl_it->second.orders.begin();

        while (it != pl_it->second.orders.end() && rem > 0) {
          int64_t tmp = rem - it->quantity;
          std::cout << "rem " << rem << "\n";
          std::cout << "tmp " << tmp << "\n";
          if (tmp >= 0) {
            std::cout << "filling all order " << it->quantity << '\n';
            // filled order on book!
            it = pl_it->second.orders.erase(it);
            rem = tmp;
          } else {
            std::cout << "filling some order " << it->quantity << " to " << tmp << '\n';
            it->quantity = std::abs(tmp);
            rem = std::min<quantity_type>(tmp, 0);
          }
          // to_fill = max(it->quantity - rem, )
        }

        if (pl_it->second.orders.size() == 0) {
          pl_it = ask_book.erase(pl_it);
        }
      }

      return;

    }

    if (t.price > best_bid) best_bid = t.price;
    if (bid_book.count(t.price) == 0) {
      // new price level
      bid_book[t.price] = {{{t.seq_num, t.quantity}}};
    } else {
      bid_book[t.price].orders.push_back({t.seq_num, t.quantity});
    }

  }
  // std::string to_json(resolution)
  // top of book
  // n price levels
  // full depth of book
  //
  std::map<price_type, price_level>::const_iterator asks_iter() const {
    return ask_book.cbegin();
  }
  std::map<price_type, price_level>::const_iterator bids_iter() const {
    return bid_book.cbegin();
  }

private:
  std::map<price_type, price_level> ask_book{};
  std::map<price_type, price_level, std::greater<price_type>> bid_book{};

  price_type best_bid{std::numeric_limits<std::uint64_t>::min()};
  price_type best_offer{std::numeric_limits<std::uint64_t>::max()};
};

} // namespace agr
