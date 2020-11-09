using price_type = std::uint64_t;
using quantity_type = std::uint64_t;

enum message_type {
  trade, quote, cancel, info
};

enum side_type {
  buy, sell
};

struct {
  std::uint64_t seq_num{0};

  message_type messageType{info}; // Trade, Quote, Cancel, Info
  std::uint64_t timestamp{0};

//  char[20] symbol;
//  char[20] exchange;

  price_type price{0};
  quantity_type quantity{0};
  side_type side{};
} tick;


class tickstream {
public:
	
	void has_next();
	void has_prev();

	void next();
	void next_trade();
	void next_quote();
	void next_info();

	void prev();
	void prev_trade();
	void prev_quote();
	void prev_info();

	// go to time
	void at();

	void current() const;
private:
	std::size_t idx;
};

struct {
	uint64_t seq_num;
	uint64_t quantity;
} order;

struct {
	std::vector<order> orders;
} price_level;

class orderbook {
public:
	void add_tick(const tick& t) const;
	void undo() const;
	void is_locked() const;
	void is_crossed() const;
private:
	std::map<price, price_level> ask_book;
	std::map<price, price_level> bid_book;
}


class orderbook_player {
public:
	void step();
	void step_backwards();
}


