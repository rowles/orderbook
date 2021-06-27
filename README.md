# Limit Orderbook

[![utest](https://github.com/rowles/orderbook/actions/workflows/test.yml/badge.svg)](https://github.com/rowles/orderbook/actions/workflows/test.yml)

Simple limit orderbook implementation. Supports market, limit, IOC orders. Utilizes sorted `std::vector` to maintain book and `std::unordered_map` for order metadata. 
