#pragma once

#include <map>
#include <sstream>
#include <vector>
#include <functional>


namespace agr {

using orderid_t = std::uint64_t;//char[50];
using quantity_t = std::uint64_t;
using price_t = std::uint64_t;

enum class side_type { buy, sell };

struct BookOrder {
  orderid_t order_id;
  quantity_t quantity;
  
  std::string to_string() const {
    std::stringstream ss{};

    ss << "[oid:" << order_id << ", quantity:" << quantity << "]"; 

    return ss.str();
  }
};

struct PriceLevel {
  price_t price;
  std::vector<BookOrder> orders;

  std::string to_string() const {
    std::stringstream ss{};

    for (const auto& o : orders) {
      ss << o.to_string() << ", ";
    }

    return ss.str();
  }

  //inline bool operator< (const PriceLevel& lhs, const PriceLevel& rhs){ return lhs.price < rhs.price; }
  /*inline bool operator> (const PriceLevel& lhs, const PriceLevel& rhs){ return cmp(lhs,rhs) >  0; }
  bool Foo::operator==(const Foo& other) {
  return bar == other.bar;
}

// Implementation 1
bool Foo::operator!=(const Foo& other) {
  return bar != other.bar
}

// Implementation 2
bool Foo::operator!=(const Foo& other) {
  return !(*this == other);
}*/
};

  inline bool operator< (const PriceLevel& lhs, const PriceLevel& rhs){ return lhs.price < rhs.price; }
// TODO: cancel, modify, market

enum class Side {
  Buy,
  Sell
};

enum class OrderType {
  Market,
  Limit,  // GTC
  IOC
};

struct Order {
  orderid_t oid;
  price_t price;
  quantity_t quantity;
  Side side;
  OrderType type;
};

struct OrderEntry {
  // ptr to price
  price_t price;
  Side book_size;
};

//
// Supports:
//  - limit
//  - market
//  - ioc
//
// Todo:
//  - cancel
class vec_orderbook {
public:
  using Book = std::vector<PriceLevel>;
  using const_iterator = Book::const_iterator;

  void submit_order(const Order& o) {
    /*if (o.side == Side::Buy) {
      handle_buy_order(o);
    } else if (o.side == Side::Sell) {
      handle_sell_order(o);
    }*/
    handle_order(o);
  }

  void cancel_order(const orderid_t& oid) {
  }

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

  // debug print book as string
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

private:

  // add order to the book
  template <typename C>
  void rest_on_book(const Order& o, quantity_t& remainder, C comp) {
    auto levels = o.side == Side::Buy ? &bid_levels : &ask_levels;
    std::cout << "resting\n";
    PriceLevel pl{.price=o.price};
    auto entry = OrderEntry {
      .price = remainder
    };

    order_map[o.oid] = entry;
 
    //auto ff = (o.side == Side::Buy) ? std::less<price_t>() : std::greater<price_t>();
    auto it = std::lower_bound(levels->begin(), levels->end(), pl, comp);

    const auto bo = BookOrder{.order_id=o.oid, .quantity=o.quantity};

    if (it == levels->end() || it->price != o.price) {
      // adding a new price level
      std::cout << "new price level\n";
      pl.orders = { bo };
      levels->insert(it, pl);
    } else if (it->price == o.price) {
      std::cout << "existing price level\n";
      // add order to existing price level
      it->orders.push_back(bo);
    }
      std::cout << "rested\n";
  }

  quantity_t handle_order(const Order& o) {
    auto remainder = o.quantity;

    bool cross_spread = o.type == OrderType::Market ||
      (o.side == Side::Sell && o.price <= best_bid)
      || (o.side == Side::Buy && o.price >= best_offer);
    ///std::cout << t.price << " buy " << best_offer << " crossed " << cross_spread << '\n';
    if (cross_spread) {
      std::cout << "crossing spread\n";
      do_cross_spread(o.side, remainder);
    }

    bool is_resting_eligible = (remainder > 0 && o.type == OrderType::Limit);
    if (!is_resting_eligible) return remainder;
    if (o.side == Side::Sell && o.price < best_offer) best_offer = o.price;
    else if (o.side == Side::Buy && o.price > best_bid) best_bid = o.price;

    // check resting eligiblity
    if (o.side == Side::Sell)
      rest_on_book(o, remainder, [](auto& a, auto& b) {
        return a.price < b.price;
        });
    else
      rest_on_book(o, remainder, [](auto& a, auto& b) {
        return a.price > b.price;
        });
  }

  // cross the spread for a side
  void do_cross_spread(Side side, quantity_t& remainder) {
    auto levels = side == Side::Buy ? &ask_levels : &bid_levels;
    auto price_level_iter = levels->begin();

    while (price_level_iter != levels->end() && remainder > 0) {
      auto order_iter = price_level_iter->orders.begin();

      while (order_iter != price_level_iter->orders.end() && remainder > 0) {
        int64_t tmp = remainder - order_iter->quantity;

        //std::cout << "rem " << remainder << "\n";
        //std::cout << "tmp " << tmp << "\n";

        if (tmp >= 0) {
          //std::cout << "filling all order " << order_iter->quantity << '\n';
          // filled order on book!
          order_iter = price_level_iter->orders.erase(order_iter);
          remainder = tmp;
          
          // TODO: delete from meta
          if (price_level_iter->orders.size() == 0) {
            // 

            //std::cout << "!! removing price level\n";
            price_level_iter = levels->erase(price_level_iter);

            // TODO: new bbo
            break;
          }
        } else {
          //std::cout << "filling some order " << order_iter->quantity << " to " << tmp << '\n';
          order_iter->quantity = std::abs(tmp);
          remainder = std::min<quantity_t>(tmp, 0);
        }
      }

      ++price_level_iter;

    }

  }

  void remove_order(orderid_t& oid ) {
    
  }


  // best seller is offering
  price_t best_bid{std::numeric_limits<std::uint64_t>::min()};
  // best buyer is offering
  price_t best_offer{std::numeric_limits<std::uint64_t>::max()};
  
  // price_level
  //   price
  //   orders: vec
  //
  // order
  //   seq_num
  //   quantity
  //
  Book bid_levels{};
  Book ask_levels{};

  std::unordered_map<orderid_t, OrderEntry> order_map{};
};






} // namespace agr
