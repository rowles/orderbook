using price_type = std::uint64_t;

struct {
  std::uint64_t seq_num;

  std::int messageType; // Trade, Quote, Cancel, Info
  std::int timestamp;

  price_type price;
  std::uint64_t quantity;
  side;
} tick;


class tickstream {
public:
	
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


