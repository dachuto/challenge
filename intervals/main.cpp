#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

#ifdef TESTER_VERSION
#include "../tester.hpp"
#endif

template <typename Small, typename Big, Small MOD>
struct Mod {
	static Small add(Small a, Small b) {
		return (a + b >= MOD) ? (a + b - MOD) : (a + b);
	}

	static Small sub(Small a, Small b) {
		return (a - b < 0) ? (a - b + MOD) : (a - b);
	}

	static Small mul(Small a, Small b) {
		return (static_cast<Big>(a) * b) % MOD;
	}

	struct extended_GCD {
		Small gcd;
		Small x;
		Small y;
	};

	static Small abs(Small a) {
		return (a < MOD) ? (a + MOD) : a;
	}

	static extended_GCD xGCD(Small a, Small b) {
		extended_GCD prev{a, 1, 0};
		extended_GCD current{b, 0, 1};
		while (current.gcd != 0) {
			Small const div = prev.gcd / current.gcd;
			extended_GCD const next{prev.gcd - div * current.gcd, prev.x - div * current.x, prev.y - div * current.y};
			prev = current;
			current = next;
		}
		return {abs(prev.gcd), abs(prev.x), abs(prev.y)};
	}

	static Small inverse(Small a) {
		auto const ret = xGCD(a, MOD);
		return ret.x;
	}

	static Small div(Small a, Small b) {
		return mul(a, inverse(b));
	}

	static std::vector<Small> factorials(Small end_arg_exclusive) {
		assert(end_arg_exclusive > 0);
		std::vector<Small> factorials;
		factorials.resize(end_arg_exclusive);
		std::iota(std::begin(factorials), std::end(factorials), 0);
		factorials[0] = 1;
		std::partial_sum(std::cbegin(factorials), std::cend(factorials), std::begin(factorials), mul);
		return factorials;
	}

	static std::vector<Small> inversions(Small end_arg_exclusive) {
		assert(end_arg_exclusive > 1);
		std::vector<Small> inversions;
		inversions.resize(end_arg_exclusive);
		inversions[0] = 1;
		inversions[1] = 1;
		for(Small i = 2; i < end_arg_exclusive; ++i) {
			inversions[i] = MOD - mul(MOD / i, inversions[MOD % i]);
		}
		return inversions;
	}

	static std::vector<Small> factorial_inversions(std::vector<Small> const &inversions) {
		std::vector<Small> factorial_inversions;
		factorial_inversions.resize(inversions.size());
		factorial_inversions[0] = inversions[0];
		std::partial_sum(std::next(std::cbegin(inversions)), std::cend(inversions), std::next(std::begin(factorial_inversions)), mul);
		return factorial_inversions;
	}

	static Small binomial_coefficient(Small n, Small k, std::vector<Small> const &factorials, std::vector<Small> const &factorial_inversions) {
		if(k < 0 or n < k) {
			return 0;
		}
		return mul(factorials[n], mul(factorial_inversions[k], factorial_inversions[n - k]));
	}
};

int32_t constexpr MAGIC_PRIME = int32_t(1e9) + 7;
using M = Mod<int32_t, int64_t, MAGIC_PRIME>;

struct Data {
	int32_t k;
	std::vector<int32_t> left;
	std::vector<int32_t> right;

	void Solve() {
		std::sort(std::begin(left), std::end(left));
		std::sort(std::begin(right), std::end(right));

		std::vector<int32_t> points;
		points.reserve(2 * left.size());
		std::merge(std::cbegin(left), std::cend(left), std::cbegin(right), std::cend(right), std::back_inserter(points));

		{
			auto const last = std::unique(std::begin(points), std::end(points));
			points.erase(last, std::end(points));
		}

		std::vector<int32_t> number_of_intervals(points.size(), 0);

		auto const count_intervals = [&](auto &type, int value) {
			size_t i = 0;
			for (auto it = std::cbegin(type); it != std::cend(type); ++it) {
				while (points[i] != *it) {
					++i;
				}
				number_of_intervals[i] += value;
			}
		};

		count_intervals(left, 1);
		count_intervals(right, -1);
		std::partial_sum(std::cbegin(number_of_intervals), std::cend(number_of_intervals), std::begin(number_of_intervals));

		std::vector<int32_t> interval_length;
		interval_length.reserve(number_of_intervals.size());
		std::adjacent_difference(std::cbegin(points), std::cend(points), std::back_inserter(interval_length));

		{
			int32_t total_points = 0;
			int32_t const max_arg = left.size() + 1;
			auto const factorials =  M::factorials(max_arg);
			auto const factorial_inversions =  M::factorial_inversions(M::inversions(max_arg));

			for (size_t i = 1; i < number_of_intervals.size(); ++i) {
				auto const temp = M::mul(interval_length[i], M::binomial_coefficient(number_of_intervals[i - 1], k, factorials, factorial_inversions));
				total_points = M::add(total_points, temp);
			}
			std::cout << total_points << "\n";
		}

		bool const debug = false;
		if (debug) {
			for (auto const &x : interval_length) {
				std::cout << x << " ";
			}
			std::cout << "\n";

			for (auto const &x : points) {
				std::cout << x << " ";
			}
			std::cout << "\n";

			for (auto const &x : number_of_intervals) {
				std::cout << x << " ";
			}
			std::cout << "\n";
		}
	}
};

std::ostream &operator<<(std::ostream &os, Data const &data) {
	os << data.left.size() << " " << data.k << "\n";
	for (size_t i = 0; i < data.left.size(); ++i) {
		std::cout << data.left[i] << " " << (data.right[i] - 1) << "\n";
	}

	return os;
}

std::istream &operator>>(std::istream &is, Data &data) {
	int32_t n;
	is >> n >> data.k;

	data.left.resize(n);
	data.right.resize(n);

	for (int32_t i = 0; i < n; ++i) {
		int temp;
		is >> data.left[i] >> temp;
		data.right[i] = temp + 1;
	}

	return is;
}

struct Tests {
	void DescribeTest(std::ostream& out, int index, bool input) const {
		if (index == 0) {
			if (input) {
				Data const data = {2, {1, 1, 2}, {3, 4, 4}};
				std::cout << data;
			} else {
				std::cout << "5\n";
			}
		} else if (index == 1) {
			if (input) {
				Data const data = {1, {1, 2, 3}, {3, 4, 5}};
				std::cout << data;
			} else {
				std::cout << "6\n";
			}
		} else if (index == 2) {
			if (input) {
				Data const data = {1, {5, 6, 7}, {6, 7, 8}};
				std::cout << data;
			} else {
				std::cout << "3\n";
			}
		} else {
			if (input) {
				int32_t const n = 2e5;
				int32_t const s = 1e9;
				std::vector<int> left(n);
				std::vector<int> right(n);
				for (int32_t i = 0; i < n; ++i) {
					left[i] = ((s / 2) / n) * i;
					right[i] = left[i] + (s / 2);
				}
				Data const data = {n / 2, left, right};
				std::cout << data;
			} else {
				std::cout << "just testing time\n";
			}
		}
	}

	int Size() const {
		return 4;
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
