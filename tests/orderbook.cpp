#include <limits.h>
#include "gtest/gtest.h"
#include "orderbook.h"
//#include "tick.h"

#define Q agr::message_type::quote
#define B agr::Side::Buy
#define S agr::Side::Sell
#define L agr::OrderType::Limit
#define M agr::OrderType::Market

// submitted order get filled
// order on book gets partial
TEST(orderbook, partial_book_fill) {
  agr::vec_orderbook ob{};

  std::cout << ob.to_string() << '\n';
  agr::Order s0{.oid = 1, .price = 102, .quantity = 100, .side=S, .type=L};
  agr::Order s1{.oid = 2, .price = 101, .quantity = 50,  .side=S, .type=L};
  agr::Order s2{.oid = 3, .price = 100, .quantity = 3,   .side=S, .type=L};
  agr::Order s3{.oid = 4, .price = 100, .quantity = 1,   .side=S, .type=L};

  agr::Order b0{.oid = 5, .price =  99, .quantity = 1,   .side=B, .type=L};
  agr::Order b1{.oid = 6, .price =  98, .quantity = 5,   .side=B, .type=L};
  agr::Order b2{.oid = 7, .price =  97, .quantity = 2,   .side=B, .type=L};
  agr::Order b3{.oid = 8, .price =  97, .quantity = 10,  .side=B, .type=L};

  ob.submit_order(s0);
  ob.submit_order(s1);
  ob.submit_order(s2);
  ob.submit_order(s3);
  std::cout << ob.to_string() << '\n';
  ob.submit_order(b0);
  ob.submit_order(b1);
  ob.submit_order(b2);
  ob.submit_order(b3);

  std::cout << ob.to_string() << '\n';

  //////
  // submitted order get filled
  // order on book gets partial
  agr::Order t0{.oid=9, .price = 100, .quantity = 1, .side=B, .type=L};

  ob.submit_order(t0);

  std::cout << ob.to_string() << '\n';
  auto it = ob.asks_begin();
  
  // check best asks
  EXPECT_EQ(it->price, 100);
  EXPECT_EQ(it->orders.size(), 2);
  EXPECT_EQ(it->orders[0].order_id, 3);
  EXPECT_EQ(it->orders[0].quantity, 2); // 3 -> 2
  EXPECT_EQ(it->orders[1].order_id, 4);  // same
  EXPECT_EQ(it->orders[1].quantity, 1);

  //////
  // submitted order get filled
  // order on book gets filled
  agr::Order t1{.oid=10, .price = 100, .quantity = 2, .side=B, .type=L};

  ob.submit_order(t1);
  it = ob.asks_begin();

  EXPECT_EQ(it->price, 100);
  EXPECT_EQ(it->orders.size(), 1); // pop old head
  EXPECT_EQ(it->orders[0].order_id, 4);
  EXPECT_EQ(it->orders[0].quantity, 1);

  //////
  // submitted order get filled
  // price level filled
  agr::Order t2{.oid=11, .price = 100, .quantity = 1, .side=B, .type=L};

  ob.submit_order(t2);
  it = ob.asks_begin();
  std::cout << ob.to_string() << '\n';

  // new best offer
  EXPECT_EQ(it->price, 101);
  EXPECT_EQ(it->orders.size(), 1); // pop old head
  EXPECT_EQ(it->orders[0].order_id, 2);
  EXPECT_EQ(it->orders[0].quantity, 50);

  //////
  // submitted order get filled
  // whole book side filled
  agr::Order t3{.oid=12, .price = 100, .quantity = 150, .side=B, .type=M};

  ob.submit_order(t3);
  it = ob.asks_begin();
  std::cout << ob.to_string() << '\n';
  
  EXPECT_EQ(it, ob.asks_end());
}

