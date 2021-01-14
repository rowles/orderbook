#pragma once

#include <map>
#include <sstream>
#include <vector>
#include <functional>


namespace agr {

using orderid_t = std::int64_t;//char[50];
using quantity_t = std::int64_t;
using price_t = std::int64_t;

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
inline bool operator> (const PriceLevel& lhs, const PriceLevel& rhs){ return lhs.price > rhs.price; }
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
  Side book_side;
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
    handle_order(o);
  }

  //void cancel_order(const orderid_t& oid) {
  //}

  const_iterator bids_begin() const {
    return bid_levels.cbegin();
  }

  const_iterator bids_end() const {
    return bid_levels.cend();
  }

  const_iterator asks_begin() const {
    return ask_levels.cbegin();
  }

  const_iterator asks_end() const {
    return ask_levels.cend();
  }

  price_t get_best_bid() const noexcept {
    return best_bid;
  }
  price_t get_best_offer() const noexcept {
    return best_offer;
  }

  // debug print book as string
  std::string to_string() const noexcept {
    std::stringstream ss{};
    
    ss << "------\n";
    ss << " bb:" << best_bid << " bo:" << best_offer << '\n'; 

    ss << " Ask Book (Sell): bo " << best_offer << '\n';
    for (const auto& pl : ask_levels) {
      auto price = pl.price;
      ss << "  " << price << " -> " << pl.to_string() << "\n";
    }

    ss << " Bid Book (Buy): bb " << best_bid << '\n';
    for (const auto& pl : bid_levels) {
      auto price = pl.price;
      ss << "  " << price << " -> " << pl.to_string() << "\n";
    }

    return ss.str();
  }

private:

  // add order to the book
  void rest_on_book(const Order& o, quantity_t& remainder) {
    PriceLevel pl{.price=o.price,.orders={}};
    auto entry = OrderEntry {
      .price = o.price,
      .book_side = o.side
    };

    order_map[o.oid] = entry;
    const auto bo = BookOrder{.order_id=o.oid, .quantity=remainder};
 
    Book* levels{nullptr};
    Book::iterator it{nullptr};

    if (o.side == Side::Buy) {
      levels = &bid_levels;
      it = std::lower_bound(levels->begin(), levels->end(), pl, std::greater<PriceLevel>());
    } else {
      levels = &ask_levels;
      it = std::lower_bound(levels->begin(), levels->end(), pl, std::less<PriceLevel>());
    }
    
    if (it == levels->end() || it->price != o.price) {
      // adding a new price level
      pl.orders = { bo };
      levels->insert(it, pl);
    } else if (it->price == o.price) {
      // add order to existing price level
      it->orders.push_back(bo);
    }
  }

  quantity_t handle_order(const Order& o) {
    auto remainder = o.quantity;

    bool cross_spread = o.type == OrderType::Market ||
      (o.side == Side::Sell && o.price <= best_bid)
      || (o.side == Side::Buy && o.price >= best_offer);
    
    if (cross_spread) {
      do_cross_spread(o.side, remainder);
    }

    bool is_resting_eligible = (remainder > 0 && o.type == OrderType::Limit);
    if (!is_resting_eligible) return remainder;
    if (o.side == Side::Sell && o.price < best_offer) best_offer = o.price;
    else if (o.side == Side::Buy && o.price > best_bid) best_bid = o.price;

    rest_on_book(o, remainder);
    return 0;
  }

  // cross the spread for a side
  void do_cross_spread(Side side, quantity_t& remainder) {
    auto levels = side == Side::Buy ? &ask_levels : &bid_levels;
    auto price_level_iter = levels->begin();

    while (levels->size() > 0 && remainder > 0) {
      auto order_iter = price_level_iter->orders.begin();

      do {
        int64_t tmp = remainder - order_iter->quantity;

        if (tmp >= 0) {
          // filled order on book!
          // TODO: remove metadata
          order_iter = price_level_iter->orders.erase(order_iter);
          remainder = tmp;
          
          if (price_level_iter->orders.size() == 0) {
            price_level_iter = levels->erase(price_level_iter);
            break;
          }
        } else {
          // partial fill
          order_iter->quantity = std::abs(tmp);
          remainder = std::min<quantity_t>(tmp, 0);
          ++price_level_iter;
        }
      } while (order_iter != price_level_iter->orders.end() && remainder > 0);

      // check new for bbo
      if (side == Side::Sell && price_level_iter == levels->begin())
        best_bid = price_level_iter->price;
      else if (side == Side::Buy && price_level_iter == levels->begin())
        best_offer = price_level_iter->price;
    }
  }

  // best seller is offering
  price_t best_bid{std::numeric_limits<std::int64_t>::min()};
  // best buyer is offering
  price_t best_offer{std::numeric_limits<std::int64_t>::max()};
  
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
