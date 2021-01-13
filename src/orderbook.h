#pragma once
#include "tick.h"

#include <map>
#include <sstream>
#include <vector>
#include <functional>


// order pool
namespace agr {

//
// Book
//   Price -> <price_level= [vec<order>]>
//
//

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

  price_type price;
  std::vector<order> orders;

  std::string to_string() const {
    std::stringstream ss{};

    for (const auto& o : orders) {
      ss << o.to_string() << ", ";
    }

    return ss.str();
  }
};

template <class C>
struct book {
public:
  using iter = std::map<price_type, price_level, C>::const_iterator;
  
  iter begin() {
    return _book.cbegin();
  }

  iter end() {
    return _book.cend();
  }
  
private:
  std::map<price_type, price_level, C> _book{};
};

// TODO: cancel, modify, market
/*
class orderbook {
public:
  void add_tick(const tick &t) {
    if (t.side == buy) {
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
    ss << "------\n";
    ss << " bb:" << best_bid << " bo:" << best_offer << '\n'; 
    ss << " price levels bid:" << bid_book.size() << " ask:" << ask_book.size() << '\n';

    ss << " Ask Book\n";
    for (const auto& [price, orders] : ask_book) {
      ss << "  " << price << " -> " << orders.to_string() << "\n";
    }
    ss << " Bid Book\n";
    for (const auto& [price, orders] : bid_book) {
      ss << "  " << price << " -> " << orders.to_string() << "\n";
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
  std::map<price_type, price_level>::const_iterator asks_end() const {
    return ask_book.cend();
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
*/






struct OrderEntry {
  // ptr to price
  price_type price;
  // side book_size;
};




class vec_orderbook {
public:
  using OrderID = int;

  void add_tick(const tick& t) {
    if (t.side == buy) {
      do_buy(t);
    } else {
      do_sell(t);
    }
  }

  void remove_order(int seq) {
    
  }

  void do_buy(const tick& t) {
    quantity_type remainder = t.quantity;

    bool cross_spread = t.price <= best_offer;
    std::cout << t.price << " buy " << best_offer << " crossed " << cross_spread << '\n';
    if (cross_spread) {
      auto price_level_iter = ask_levels.begin();

      // iterate price levels
      while (price_level_iter != ask_levels.end() && remainder > 0) {
        auto order_iter = price_level_iter->orders.begin();

        // iterate orders resting at price
        while (order_iter != price_level_iter->orders.end() && remainder > 0) {
          int64_t tmp = remainder - order_iter->quantity;


          std::cout << "rem " << remainder << "\n";
          std::cout << "tmp " << tmp << "\n";

          if (tmp >= 0) {
            //std::cout << "filling all order " << it->quantity << '\n';
            // filled order on book!
            order_iter = price_level_iter->orders.erase(order_iter);
            remainder = tmp;
            // TODO: delete from meta
          } else {
            //std::cout << "filling some order " << it->quantity << " to " << tmp << '\n';
            order_iter->quantity = std::abs(tmp);
            remainder = std::min<quantity_type>(tmp, 0);
          }
        }
      }
    }

    if (remainder == 0) return;
    if (t.price > best_bid) best_bid = t.price;
    // order will rest on book
    auto entry = OrderEntry {
      .price = remainder
    };

    ///order_map[oid] = entry;

    price_level pl{.price=t.price};
    auto it = std::lower_bound(bid_levels.begin(), bid_levels.end(), pl,
        [](auto& a, auto& b) {
          return a.price > b.price;
        });

    if (it == bid_levels.end() || it->price != t.price) {
      std::cout << "adding new!\n";
      // add new
      pl.orders = { order{.seq_num=1, .quantity=t.quantity} };
      bid_levels.insert(it, pl);
    } else if (it->price == t.price) {
      std::cout << "adding existing!\n";
      // add to existing level
      it->orders.push_back(order{.seq_num=1, .quantity=t.quantity});
    }
  }

  void do_cross_spread(std::vector<price_level>* levels, quantity_type remainder) {
    auto price_level_iter = levels->begin();

    while (price_level_iter != levels->end() && remainder > 0) {
      auto order_iter = price_level_iter->orders.begin();

      while (order_iter != price_level_iter->orders.end() && remainder > 0) {
        int64_t tmp = remainder - order_iter->quantity;

        std::cout << "rem " << remainder << "\n";
        std::cout << "tmp " << tmp << "\n";

        if (tmp >= 0) {
          //std::cout << "filling all order " << it->quantity << '\n';
          // filled order on book!
          order_iter = price_level_iter->orders.erase(order_iter);
          remainder = tmp;
          // TODO: delete from meta
        } else {
          //std::cout << "filling some order " << it->quantity << " to " << tmp << '\n';
          order_iter->quantity = std::abs(tmp);
          remainder = std::min<quantity_type>(tmp, 0);
        }
      }

    }

  }

  void do_sell(const tick& t) {
    quantity_type remainder = t.quantity;

    bool cross_spread = t.price <= best_bid;
    std::cout << t.price << " sell " << best_bid << " crossed " << cross_spread << '\n';

    if (cross_spread) {
      std::cout << "crossing spread\n";
      do_cross_spread(&bid_levels, remainder);
      /*
      std::cout << "crossing spread\n";
      auto price_level_iter = ask_levels.begin();

      // iterate price levels
      while (price_level_iter != ask_levels.end()
          && remainder > 0) {
        auto order_iter = price_level_iter->orders.begin();

        // iterate orders resting at price
        while (order_iter != price_level_iter->orders.end() && remainder > 0) {
          int64_t tmp = remainder - order_iter->quantity;


          std::cout << "rem " << remainder << "\n";
          std::cout << "tmp " << tmp << "\n";

          if (tmp >= 0) {
            //std::cout << "filling all order " << it->quantity << '\n';
            // filled order on book!
            order_iter = price_level_iter->orders.erase(order_iter);
            remainder = tmp;
            // TODO: delete from meta
          } else {
            //std::cout << "filling some order " << it->quantity << " to " << tmp << '\n';
            order_iter->quantity = std::abs(tmp);
            remainder = std::min<quantity_type>(tmp, 0);
          }
        }
      }*/
    }

    if (remainder == 0) return;
    if (t.price < best_offer) best_offer = t.price;
    // order will rest on book
    auto entry = OrderEntry {
      .price = remainder
    };

    ///order_map[oid] = entry;

    price_level pl{.price=t.price};
    auto it = std::lower_bound(ask_levels.begin(), ask_levels.end(), pl,
        [](auto& a, auto& b) {
          return a.price < b.price;
        });

    if (it == ask_levels.end() || it->price != t.price) {
      std::cout << "adding new!\n";
      // add new
      pl.orders = { order{.seq_num=1, .quantity=t.quantity} };
      ask_levels.insert(it, pl);
    } else if (it->price == t.price) {
      std::cout << "adding existing!\n";
      // add to existing level
      it->orders.push_back(order{.seq_num=1, .quantity=t.quantity});
    }
  }

  std::string to_string() const noexcept {
    std::stringstream ss{};
    ss << "------\n";
    ss << " bb:" << best_bid << " bo:" << best_offer << '\n'; 

    ss << " Ask Book\n";
    for (const auto& pl : ask_levels) {
      auto price = pl.price;
      ss << "  " << price << " -> " << pl.to_string() << "\n";
    }
    ss << " Bid Book\n";
    for (const auto& pl : bid_levels) {
      auto price = pl.price;
      ss << "  " << price << " -> " << pl.to_string() << "\n";
    }

    return ss.str();
  }

  using const_iterator = std::vector<price_level>::const_iterator;

  const_iterator bids_begin() {
    return bid_levels.cbegin();
  }

  const_iterator bids_end() {
    return bid_levels.cend();
  }

  const_iterator asks_begin() {
    return ask_levels.cbegin();
  }

  const_iterator asks_end() {
    return ask_levels.cend();
  }
private:
  // best seller is offering
  price_type best_bid{std::numeric_limits<std::uint64_t>::min()};
  // best buyer is offering
  price_type best_offer{std::numeric_limits<std::uint64_t>::max()};
  
  // price_level
  //   price
  //   orders: vec
  //
  // order
  //   seq_num
  //   quantity
  //
  std::vector<price_level> bid_levels{};
  std::vector<price_level> ask_levels{};

  //std::unordered_map<OrderID, OrderMeta> order_map{};
};






} // namespace agr
