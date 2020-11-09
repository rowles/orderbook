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
	void add_tick(const tick& t) {
          if (t.side == buy) {
            bid_book[t.price] =  { { {t.seq_num, t.quantity} } };
          } else if (t.side == sell) {
            ask_book[t.price] =  { { {t.seq_num, t.quantity} } };
          }
        }
	void undo();
	void is_locked() const;
	void is_crossed() const;

        std::string to_string() const noexcept {
          std::stringstream ss{};

          return ss.str();
        }

        // std::string to_json(resolution)
        // top of book
        // n price levels
        // full depth of book

private:
	std::map<price_type, price_level> ask_book{};
	std::map<price_type, price_level> bid_book{};
};

}
