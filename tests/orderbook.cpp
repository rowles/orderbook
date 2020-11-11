#include <limits.h>
#include "gtest/gtest.h"
#include "orderbook.h"
#include "tick.h"

#define Q agr::message_type::quote
#define B agr::side_type::buy
#define S agr::side_type::sell

// submitted order get filled
// order on book gets partial
TEST(orderbook, partial_book_fill) {
  agr::orderbook ob{};

  agr::tick s0{.seq_num = 1, .message = Q, .price = 102, .quantity = 100, .side=S};
  agr::tick s1{.seq_num = 2, .message = Q, .price = 101, .quantity = 50,  .side=S};
  agr::tick s2{.seq_num = 3, .message = Q, .price = 100, .quantity = 3,   .side=S};
  agr::tick s3{.seq_num = 4, .message = Q, .price = 100, .quantity = 1,   .side=S};

  agr::tick b0{.seq_num = 5, .message = Q, .price = 999, .quantity = 1,   .side=B};
  agr::tick b1{.seq_num = 6, .message = Q, .price = 998, .quantity = 5,   .side=B};
  agr::tick b2{.seq_num = 7, .message = Q, .price = 997, .quantity = 2,   .side=B};
  agr::tick b3{.seq_num = 8, .message = Q, .price = 997, .quantity = 10,  .side=B};

  ob.add_tick(s0);
  ob.add_tick(s1);
  ob.add_tick(s2);
  ob.add_tick(s3);
  ob.add_tick(b0);
  ob.add_tick(b1);
  ob.add_tick(b2);
  ob.add_tick(b3);

  std::cout << ob.to_string() << '\n';

  //////
  // submitted order get filled
  // order on book gets partial
  agr::tick t0{.message = Q, .price = 100, .quantity = 1, .side=B};

  ob.add_tick(t0);

  std::cout << ob.to_string() << '\n';
  auto ait = ob.asks_iter();
  
  EXPECT_EQ(ait->first, 100);

  EXPECT_EQ(ait->second.orders.size(), 2);
  EXPECT_EQ(ait->second.orders.at(0).seq_num,  3);
  EXPECT_EQ(ait->second.orders.at(0).quantity, 2); // 3 -> 2
  EXPECT_EQ(ait->second.orders.at(1).seq_num,  4);
  EXPECT_EQ(ait->second.orders.at(1).quantity, 1);
  
  //////
  // submitted order get filled
  // order on book gets filled
  agr::tick t1{.message = Q, .price = 100, .quantity = 2, .side=B};

  ob.add_tick(t1);
  ait = ob.asks_iter();

  EXPECT_EQ(ait->first, 100);
  EXPECT_EQ(ait->second.orders.size(), 1);
  EXPECT_EQ(ait->second.orders.at(0).seq_num,  4);
  EXPECT_EQ(ait->second.orders.at(0).quantity, 1);

  //////
  // submitted order get filled
  // price level filled
  agr::tick t2{.message = Q, .price = 100, .quantity = 1, .side=B};

  ob.add_tick(t2);
  ait = ob.asks_iter();
  std::cout << ob.to_string() << '\n';

  EXPECT_EQ(ait->first, 101);
  EXPECT_EQ(ait->second.orders.size(), 1);
  EXPECT_EQ(ait->second.orders.at(0).seq_num,  2);
  EXPECT_EQ(ait->second.orders.at(0).quantity, 50);

  //////
  // submitted order get filled
  // whole book side filled
  agr::tick t3{.message = Q, .price = 100, .quantity = 150, .side=B};

  ob.add_tick(t3);
  ait = ob.asks_iter();
  std::cout << ob.to_string() << '\n';

  EXPECT_EQ(ait->first, 101);
  EXPECT_EQ(ait->second.orders.size(), 1);
  EXPECT_EQ(ait->second.orders.at(0).seq_num,  2);
  EXPECT_EQ(ait->second.orders.at(0).quantity, 50);

}

