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
};

// TODO: cancel, modify, market

struct OrderEntry {
  // ptr to price
  price_t price;
  // side book_size;
};

enum class Side {
  Buy,
  Sell
};

struct Order {
  orderid_t oid;
  price_t price;
  quantity_t quantity;
  Side side;
};

class vec_orderbook {
public:
  using OrderID = int;
  using Book = std::vector<PriceLevel>;
  using const_iterator = Book::const_iterator;

  void market_order(const Order& o) {

  }

  void submit_order(const Order& o) {
    if (o.side == Side::Buy) {
      do_buy(o);
    } else if (o.side == Side::Sell) {
    }
  }

  void cancel_order(const orderid_t& oid) {
  }

  /*
  void add_tick(const tick& t) {
    if (t.side == buy) {
      do_buy(t);
    } else {
      do_sell(t);
    }
  }*/

  void remove_order(orderid_t& oid) {
    
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
  void rest_on_book(Book* levels, const Order& o, quantity_t& remainder, C comp) {
    std::cout << "resting\n";
    PriceLevel pl{.price=o.price};
    auto entry = OrderEntry {
      .price = remainder
    };

    ///order_map[oid] = entry;
 
    auto it = std::lower_bound(levels->begin(), levels->end(), pl, comp);

    const auto bo = BookOrder{.order_id=o.oid, .quantity=o.quantity};

    if (it == levels->end() || it->price != o.price) {
      // adding a new price level
      std::cout << "new price level\n";
      pl.orders = { bo };
      bid_levels.insert(it, pl);
    } else if (it->price == o.price) {
      std::cout << "existing price level\n";
      // add order to existing price level
      it->orders.push_back(bo);
    }
  }

  void do_buy(const Order& o) {
    auto remainder = o.quantity;

    bool cross_spread = o.price >= best_offer;
    ///std::cout << t.price << " buy " << best_offer << " crossed " << cross_spread << '\n';
    if (cross_spread) {
      std::cout << "crossing spread\n";
      do_cross_spread(&ask_levels, remainder);
    }

    if (remainder == 0) return;
    if (o.price > best_bid) best_bid = o.price;

    // check resting eligiblity
    rest_on_book(&bid_levels, o, remainder, [](auto& a, auto& b) {
        return a.price > b.price;
        });
    /*
    // order will rest on book
    PriceLevel pl{.price=o.price};
    auto entry = OrderEntry {
      .price = remainder
    };

    ///order_map[oid] = entry;
 
    auto it = std::lower_bound(bid_levels.begin(), bid_levels.end(), pl,
        [](auto& a, auto& b) {
          return a.price > b.price;
        });

    const auto bo = BookOrder{.order_id=o.oid, .quantity=o.quantity};

    if (it == bid_levels.end() || it->price != o.price) {
      //std::cout << "adding new!\n";
      // add new
      pl.orders = { bo };
      bid_levels.insert(it, pl);
    } else if (it->price == o.price) {
      //std::cout << "adding existing!\n";
      // add to existing level
      it->orders.push_back(bo);
    }*/
  }

  // cross the spread for a side
  void do_cross_spread(std::vector<PriceLevel>* levels, quantity_t& remainder) {
    auto price_level_iter = levels->begin();

    while (price_level_iter != levels->end() && remainder > 0) {
      auto order_iter = price_level_iter->orders.begin();

      while (order_iter != price_level_iter->orders.end() && remainder > 0) {
        int64_t tmp = remainder - order_iter->quantity;

        std::cout << "rem " << remainder << "\n";
        std::cout << "tmp " << tmp << "\n";

        if (tmp >= 0) {
          std::cout << "filling all order " << order_iter->quantity << '\n';
          // filled order on book!
          order_iter = price_level_iter->orders.erase(order_iter);
          remainder = tmp;
          
          // TODO: delete from meta
          if (price_level_iter->orders.size() == 0) {
            std::cout << "!! removing price level\n";
            price_level_iter = levels->erase(price_level_iter);
            break;
          }
        } else {
          std::cout << "filling some order " << order_iter->quantity << " to " << tmp << '\n';
          order_iter->quantity = std::abs(tmp);
          remainder = std::min<quantity_t>(tmp, 0);
        }
      }

      ++price_level_iter;

    }

  }

  /*
  void do_sell(const tick& t) {
    quantity_type remainder = t.quantity;

    bool cross_spread = t.price <= best_bid;
    std::cout << t.price << " sell " << best_bid << " crossed " << cross_spread << '\n';

    if (cross_spread) {
      std::cout << "crossing spread\n";
      do_cross_spread(&bid_levels, remainder);
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

    const auto o = order{.seq_num=t.seq_num, .quantity=t.quantity};
    
    if (it == ask_levels.end() || it->price != t.price) {
      std::cout << "adding new!\n";
      // add new
      pl.orders = { o };
      ask_levels.insert(it, pl);
    } else if (it->price == t.price) {
      std::cout << "adding existing!\n";
      // add to existing level
      it->orders.push_back(o);
    }
  }*/



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

  //std::unordered_map<OrderID, OrderMeta> order_map{};
};






} // namespace agr
