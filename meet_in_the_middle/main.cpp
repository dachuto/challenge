#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <stack>
#include <vector>

struct v3 {
	std::array<int, 3> values;
};

v3 operator+(v3 const &lhs, v3 const &rhs) {
	return {{{lhs.values[0] + rhs.values[0], lhs.values[1] + rhs.values[1], lhs.values[2] + rhs.values[2]}}};
}

v3 operator-(v3 const &lhs, v3 const &rhs) {
	return {{{lhs.values[0] - rhs.values[0], lhs.values[1] - rhs.values[1], lhs.values[2] - rhs.values[2]}}};
}

bool operator<(v3 const &lhs, v3 const &rhs) {
	return lhs.values < rhs.values;
}

int sum(v3 const &v) {
	return std::accumulate(v.values.cbegin(), v.values.cend(), 0);
}

std::pair<int, int> meet_in_the_middle_condition_first_half(v3 const &v) {
	return {v.values[0] - v.values[1], v.values[1] - v.values[2]};
}

std::pair<int, int> meet_in_the_middle_condition_second_half(v3 const &v) {
	return {v.values[1] - v.values[0], v.values[2] - v.values[1]};
}

struct Found {
	std::pair<int, int> partial;
	int x;
	int order;
};

template <typename OutputIterator, typename InputIterator, typename Functor>
void IterativeNestedLoop(OutputIterator slot_first, OutputIterator slot_last, InputIterator max_first, Functor f) {
	std::fill(slot_first, slot_last, 0);

	InputIterator max_index = max_first;
	OutputIterator slot_index = slot_first;

	while (slot_index != slot_last) {
		f();

		max_index = max_first;
		slot_index = slot_first;

		while (slot_index != slot_last) {
			++(*slot_index);
			if (*slot_index == *max_index) {
				*slot_index = 0;
				++max_index;
				++slot_index;
			} else {
				break;
			}
		}
	}
}

template <typename InputIt1, typename InputIt2, typename OutputFunctor, typename Compare>
void set_intersection_out(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputFunctor out, Compare comp) {
	while (first1 != last1 && first2 != last2) {
		if (comp(*first1, *first2)) {
			++first1;
		} else if (comp(*first2, *first1)) {
			++first2;
		} else {
			out(*first1, *first2);
			++first1;
			++first2;
		}
	}
}

int main() {
	std::ios_base::sync_with_stdio(false);
//	std::cin.tie(nullptr);
	int n;
	std::cin >> n;
	std::vector<v3> quests;

	for (int i = 0; i < n; ++i) {
		v3 q;
		std::cin >> q.values[0] >> q.values[1] >> q.values[2];
		quests.push_back(q);
	}

	auto const generate_all_possible_sums = [](std::vector<Found> &out, auto quest_first, auto quest_last, auto adaptor) {
		std::vector<int> slots;
		slots.resize(std::distance(quest_first, quest_last));

		std::vector<int> maxes;
		maxes.resize(slots.size());
		std::fill(maxes.begin(), maxes.end(), 3);

		int order = 0;
		IterativeNestedLoop(slots.begin(), slots.end(), maxes.cbegin(), [&]() {
			v3 partial_sum {{{0, 0, 0}}};
			for (size_t i = 0; i < slots.size(); ++i) {
				int const missing = slots[i];
				v3 quest = *(quest_first + i);
				quest.values[missing] = 0;
				partial_sum = partial_sum + quest;
			}

			out.push_back({adaptor(partial_sum), partial_sum.values[0], order});
			++order;
		});
	};

	auto const middle = quests.cbegin() + (n / 2);
	auto const first_half_size = std::distance(quests.cbegin(), middle);
	auto const second_half_size = std::distance(middle, quests.cend());

	std::vector<Found> first_half;
	first_half.reserve(1024 * 1024);
	generate_all_possible_sums(first_half, quests.cbegin(), middle, meet_in_the_middle_condition_first_half);
	if (first_half.size() == 0) {
		first_half.push_back({{0, 0}, 0, 0});
	}

	std::vector<Found> second_half;
	second_half.reserve(1024 * 1024);
	generate_all_possible_sums(second_half, middle, quests.cend(), meet_in_the_middle_condition_second_half);

	auto const compare_partial_and_magnitude = [](Found const &a, Found const &b) {
		if (a.partial == b.partial) {
			return a.x > b.x;
		}
		return a.partial < b.partial;
	};

	auto const compare_partial = [](Found const &a, Found const &b) {
		return a.partial < b.partial;
	};

	auto const equal_partial = [](Found const &a, Found const &b) {
		return a.partial == b.partial;
	};

	std::sort(first_half.begin(), first_half.end(), compare_partial_and_magnitude);
	first_half.erase(std::unique(first_half.begin(), first_half.end(), equal_partial), first_half.end());

	std::sort(second_half.begin(), second_half.end(), compare_partial_and_magnitude);
	second_half.erase(std::unique(second_half.begin(), second_half.end(), equal_partial), second_half.end());

	bool any = false;
	std::pair<Found, Found> best;

	auto const output_those_that_are_equal = [&any, &best](Found const &a, Found const &b) {
		if (not any or (best.first.x + best.second.x) < (a.x + b.x)) {
			best = {a, b};
			any = true;
		}
	};

	set_intersection_out(first_half.cbegin(), first_half.cend(), second_half.cbegin(), second_half.cend(), output_those_that_are_equal, compare_partial);

//	for (auto const &e : first_half) {
//		std::cout << e.partial.first << " " << e.partial.second << " " << e.x << "\n";
//	}
//
//	std::cout << "=========\n";
//
//	for (auto const &e : second_half) {
//		std::cout << e.partial.first << " " << e.partial.second << " " << e.x << "\n";
//	}

	if (not any) {
		std::cout << "Impossible";
		return 0;
	}

	auto const print_missing = [](int companion) {
		if (companion == 0) {
			std::cout << "MW\n";
		} else if (companion == 1) {
			std::cout << "LW\n";
		} else {
			std::cout << "LM\n";
		}
	};

	auto const print_half = [&print_missing](auto size, int order) {
		for (int i = 0; i < static_cast<int>(size); ++i) {
			print_missing(order % 3);
			order /= 3;
		}
	};

	print_half(first_half_size, best.first.order);
	print_half(second_half_size, best.second.order);

	return 0;
}
