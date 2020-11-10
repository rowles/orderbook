#include "orderbook.h"
#include "tick.h"
#include <iostream>

int main() {
  agr::tick t0{.seq_num = 1,
               .message = agr::message_type::quote,
               .price = 101,
               .quantity = 10,
               .side = agr::side_type::sell};


  agr::tick t1{.seq_num = 2,
               .message = agr::message_type::quote,
               .price = 99,
               .quantity = 10,
               .side = agr::side_type::buy};


  std::cout << t0.to_string();
  std::cout << t1.to_string();

  agr::orderbook ob{};
  std::cout << ob.to_string();
  
#define Q agr::message_type::quote
#define B agr::side_type::buy
#define S agr::side_type::sell
  agr::tick s0{.message = Q, .price = 102, .quantity = 100, .side=S};
  agr::tick s1{.message = Q, .price = 101, .quantity = 50, .side=S};
  agr::tick s2{.message = Q, .price = 100, .quantity = 1, .side=S};
  agr::tick s3{.message = Q, .price = 100, .quantity = 2, .side=S};

  agr::tick b0{.message = Q, .price = 999, .quantity = 1, .side=B};
  agr::tick b1{.message = Q, .price = 998, .quantity = 5, .side=B};
  agr::tick b2{.message = Q, .price = 997, .quantity = 2, .side=B};
  agr::tick b3{.message = Q, .price = 997, .quantity = 10, .side=B};

  ob.add_tick(s0);
  ob.add_tick(s1);
  ob.add_tick(s2);
  ob.add_tick(s3);
  ob.add_tick(b0);
  ob.add_tick(b1);
  ob.add_tick(b2);
  ob.add_tick(b3);
  std::cout << ob.to_string();
}
