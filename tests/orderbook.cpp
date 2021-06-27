#include "gtest/gtest.h"
#include "orderbook.hpp"


using namespace orderbook;


TEST(orderbook, scenario_one) {
  OrderBook ob{};

  ClientOrder s0{.oid = 1, .price = 102, .quantity = 100, .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s1{.oid = 2, .price = 101, .quantity = 50,  .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s2{.oid = 3, .price = 100, .quantity = 3,   .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s3{.oid = 4, .price = 100, .quantity = 1,   .side=Side::Sell, .type=OrderType::Limit};

  ClientOrder b0{.oid = 5, .price =  99, .quantity = 1,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b1{.oid = 6, .price =  98, .quantity = 5,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b2{.oid = 7, .price =  97, .quantity = 2,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b3{.oid = 8, .price =  97, .quantity = 10,  .side=Side::Buy, .type=OrderType::Limit};

  ob.submit_order(s0);
  ob.submit_order(s1);
  ob.submit_order(s2);
  ob.submit_order(s3);
  ob.submit_order(b0);
  ob.submit_order(b1);
  ob.submit_order(b2);
  ob.submit_order(b3);

  //////
  // submitted order get filled
  // order on book gets partial
  ClientOrder t0{.oid=9, .price = 100, .quantity = 1, .side=Side::Buy, .type=OrderType::Limit};

  auto rem = ob.submit_order(t0);
  EXPECT_EQ(rem, 0);

  auto it = ob.asks_iter();
  auto level = it.begin();
  
  // check best asks
  EXPECT_EQ(level->price, 100);
  EXPECT_EQ(level->orders.size(), 2);
  EXPECT_EQ(level->orders[0].order_id, 3);
  EXPECT_EQ(level->orders[0].quantity, 2); // 3 -> 2
  EXPECT_EQ(level->orders[1].order_id, 4);  // same
  EXPECT_EQ(level->orders[1].quantity, 1);

  //////
  // submitted order get filled
  // order on book gets filled
  ClientOrder t1{.oid=10, .price = 100, .quantity = 2, .side=Side::Buy, .type=OrderType::Limit};

  rem = ob.submit_order(t1);
  EXPECT_EQ(rem, 0);

  it = ob.asks_iter();
  level = it.begin();

  EXPECT_EQ(level->price, 100);
  EXPECT_EQ(level->orders.size(), 1); // pop old head
  EXPECT_EQ(level->orders[0].order_id, 4);
  EXPECT_EQ(level->orders[0].quantity, 1);

  //////
  // submitted order get filled
  // price level filled
  ClientOrder t2{.oid=11, .price = 100, .quantity = 1, .side=Side::Buy, .type=OrderType::Limit};
  
  rem = ob.submit_order(t2);
  EXPECT_EQ(rem, 0);
  
  it = ob.asks_iter();
  level = it.begin();

  // new best offer
  EXPECT_EQ(level->price, 101);
  EXPECT_EQ(level->orders.size(), 1); // pop old head
  EXPECT_EQ(level->orders[0].order_id, 2);
  EXPECT_EQ(level->orders[0].quantity, 50);

  //////
  // submitted order get filled
  // whole book side filled
  ClientOrder t3{.oid=12, .price = 100, .quantity = 150, .side=Side::Buy, .type=OrderType::Market};

  rem = ob.submit_order(t3);
  EXPECT_EQ(rem, 0);

  it = ob.asks_iter();
  
  EXPECT_EQ(it.begin(), it.end());
}


TEST(orderbook, scenario_two) {
  OrderBook ob{};

  ClientOrder s0{.oid = 1, .price = 110, .quantity = 100, .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s1{.oid = 2, .price = 109, .quantity = 50,  .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s2{.oid = 3, .price = 100, .quantity = 3,   .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s3{.oid = 4, .price = 100, .quantity = 1,   .side=Side::Sell, .type=OrderType::Limit};

  ClientOrder b0{.oid = 5, .price =  99, .quantity = 1,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b1{.oid = 6, .price =  98, .quantity = 5,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b2{.oid = 7, .price =  97, .quantity = 2,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b3{.oid = 8, .price =  90, .quantity = 100,  .side=Side::Buy, .type=OrderType::Limit};

  ob.submit_order(s0);
  ob.submit_order(s1);
  ob.submit_order(s2);
  ob.submit_order(s3);
  ob.submit_order(b0);
  ob.submit_order(b1);
  ob.submit_order(b2);
  ob.submit_order(b3);


  //////
  // submitted order get partial
  // whole side filled
  ClientOrder t0{.oid=9, .price = 0, .quantity = 200, .side=Side::Buy, .type=OrderType::Market};

  auto rem = ob.submit_order(t0);
  EXPECT_EQ(rem, 46);

  auto it = ob.asks_iter();
  EXPECT_EQ(it.begin(), it.end());

  //////
  // submitted order get filled
  // whole side filled
  ClientOrder t1{.oid=10, .price = 0, .quantity = 120, .side=Side::Sell, .type=OrderType::Market};

  rem = ob.submit_order(t1);
  EXPECT_EQ(rem, 12);
  
  it = ob.bids_iter();
  EXPECT_EQ(it.begin(), it.end());
}


TEST(orderbook, scenario_three) {
  OrderBook ob{};

  ClientOrder s0{.oid = 1, .price = 110, .quantity = 100, .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s1{.oid = 2, .price = 109, .quantity = 50,  .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s2{.oid = 3, .price = 100, .quantity = 3,   .side=Side::Sell, .type=OrderType::Limit};
  ClientOrder s3{.oid = 4, .price = 100, .quantity = 10,   .side=Side::Sell, .type=OrderType::Limit};

  ClientOrder b0{.oid = 5, .price =  99, .quantity = 10,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b1{.oid = 6, .price =  99, .quantity = 5,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b2{.oid = 7, .price =  97, .quantity = 2,   .side=Side::Buy, .type=OrderType::Limit};
  ClientOrder b3{.oid = 8, .price =  90, .quantity = 100,  .side=Side::Buy, .type=OrderType::Limit};

  ob.submit_order(s0);
  ob.submit_order(s1);
  ob.submit_order(s2);
  ob.submit_order(s3);
  ob.submit_order(b0);
  ob.submit_order(b1);
  ob.submit_order(b2);
  ob.submit_order(b3);

  //////
  // IOC buy priced too far
  ClientOrder t0{.oid=9, .price = 1, .quantity = 11, .side=Side::Buy, .type=OrderType::IOC};

  auto rem = ob.submit_order(t0);
  EXPECT_EQ(rem, 11);

  //////
  // IOC sell priced too far
  ClientOrder t1{.oid=10, .price = 130, .quantity = 12, .side=Side::Sell, .type=OrderType::IOC};

  rem = ob.submit_order(t1);
  EXPECT_EQ(rem, 12);
}
