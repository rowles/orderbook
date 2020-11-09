#include "tick.h"
#include "orderbook.h"
#include <iostream>

int main() {
  agr::tick t0 {
    .seq_num = 1,
    .message = agr::message_type::quote,
    .price = 101,
    .quantity = 10,
    .side = agr::side_type::sell
  };

  agr::tick t1 {
    .seq_num = 2,
    .message = agr::message_type::quote,
    .price = 99,
    .quantity = 10,
    .side = agr::side_type::buy
  };

  std::cout << t0.to_string();
  std::cout << t1.to_string();

  agr::orderbook ob{};
  ob.add_tick(t0);
  ob.add_tick(t1);
}
