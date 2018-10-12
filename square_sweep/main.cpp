#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#ifdef TESTER_VERSION
#include "../tester.hpp"
#endif

template <class InputIt, class BinaryOperation>
InputIt consecutive_pairs(InputIt first, InputIt last, BinaryOperation binary_operation) {
	if (first != last) {
		InputIt second = std::next(first);
		if (second != last) {
			do {
				binary_operation(*first++, *second++);
			} while (second != last);
			return first;
		}
	}
	return last;
}

struct Data {
	typedef std::pair<int, int> point_t;
	typedef std::tuple<int, int, bool> event_x_t;
	typedef std::pair<int, bool> event_y_t;

	std::vector<point_t> points;
	int square_size;

	void Solve() const {
		std::vector<event_x_t> events_x;
		events_x.reserve(2 * points.size());
		for (auto const &p : points) {
			events_x.push_back(event_x_t{p.first, p.second, true});
			events_x.push_back(event_x_t{p.first + square_size, p.second, false});
		}

		std::sort(events_x.begin(), events_x.end());

		std::map<event_y_t, int> events_y;

		std::vector<std::int64_t> count;//(points.size(), 0);
		count.resize(points.size());

		consecutive_pairs(events_x.cbegin(), events_x.cend(), [&](event_x_t const &ax, event_x_t const &bx) {
			{
				event_y_t const add_event{std::get<1>(ax), true};
				event_y_t const remove_event{std::get<1>(ax) + square_size, false};
				if (std::get<2>(ax)) {
					++events_y[add_event];
					++events_y[remove_event];
				} else {
					auto const remove_from_map = [&events_y](event_y_t const &event) {
						auto const it = events_y.find(event);
						if (it != events_y.end()) {
							--it->second;
							if (it->second == 0) {
								events_y.erase(it);
							}
						}
					};

					remove_from_map(add_event);
					remove_from_map(remove_event);
				}
			}

			int const distance_x = std::get<0>(bx) - std::get<0>(ax);
			if (distance_x == 0) {
				return;
			}

			std::int64_t overlapping = 0;
			consecutive_pairs(events_y.cbegin(), events_y.cend(), [&](std::pair<event_y_t, int> const &a, std::pair<event_y_t, int> const &b) {
				if (a.first.second) {
					overlapping += a.second;
				} else {
					overlapping -= a.second;
				}

				int const distance_y = b.first.first - a.first.first;
				if (distance_y == 0) {
					return;
				}

				if (overlapping > 0) {
					count[overlapping - 1] += distance_x * distance_y;
				}
			});
		});

		for (auto c : count) {
			std::cout << c << " ";
		}
	}
};

std::ostream &operator<<(std::ostream &os, Data const &data) {
	os << data.points.size() << " " << data.square_size << "\n";
	for (auto const &p : data.points) {
		os << p.first << " " << p.second << "\n";
	}
	return os;
}

std::istream &operator>>(std::istream &is, Data &data) {
	std::size_t n;
	is >> n >> data.square_size;

	data.points.resize(n);

	for (auto &p : data.points) {
		is >> p.first >> p.second;
	}

	return is;
}

struct Tests {
	void DescribeTest(std::ostream& out, int index, bool input) const {
		if (index == 0) {
			if (input) {
				Data const data = {{{0, 0}}, 5};
				std::cout << data;
			} else {
				std::cout << "25 ";
			}
		} else if (index == 1) {
			if (input) {
				Data const data = {{{4, 5}, {4, 6}, {5, 5}, {5, 6}, {7, 7}}, 3};
				std::cout << data;
			} else {
				std::cout << "10 8 1 4 0 ";
			}
		} else if (index == 2) {
			std::int64_t const n = 100000;
			int const square_size = 300;
			if (input) {
				Data data;
				data.square_size = square_size;
				for (int i = 0; i < n; ++i) {
					data.points.push_back({0, 2 * i * square_size});
				}
				std::cout << data;
			} else {
				std::cout << n * square_size * square_size << " ";
				for (int i = 1; i < n; ++i) {
					std::cout << "0 ";
				}
			}
		}
	}

	int Size() const {
		return 3;
	}
};

int main(int argc, char *argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

#ifdef TESTER_VERSION
	Tests tests;
	return Dispatch(argc, argv, tests);
#else
	Data input;
	std::cin >> input;
	input.Solve();
#endif
	return 0;
}

