#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <map>
#include <stack>
#include <tuple>
#include <utility>
#include <vector>

template <typename Integer>
auto pow2_ge(Integer x) {
	Integer l = 1;
	while (x > l) {
		l <<= 1;
	}
	return l;
}

template <typename T, typename Integer>
class segment_tree {
public:
	explicit segment_tree(Integer size) {
			nodes.resize(size_for_number_of_leaves(size));
			first_leaf_index = nodes.size() - size;
			for (Integer i = 0; i < size; ++i) {
				nodes[leaf_index(i)].valid = true;
			}

			for (auto i = first_leaf_index - 1; i > 0; --i) {
				nodes[i].valid = (left_child(i) < nodes.size()) and nodes[left_child(i)].valid;
			}
		}

	void update_all_internal() {
		for (auto i = first_leaf_index - 1; i > 0; --i) {
			update_internal(i);
		}
	}

	void update_from_leaf_to_root(Integer l) {
		auto i = leaf_index(l);
		while (i != ROOT) {
			i = parent(i);
			update_internal(i);
		}
	}

	T const &root_value() const {
		return nodes[ROOT].value;
	}

	T const &internal_value(Integer i) const {
		return nodes[i].value;
	}

	T const &value(Integer l) const {
		return nodes[leaf_index(l)].value;
	}

	T &value(Integer l) {
		return nodes[leaf_index(l)].value;
	}

	static Integer size_for_number_of_leaves(Integer leaves) {
		return pow2_ge(leaves) + leaves;
	}

private:
	struct node {
		T value;
		bool valid = false;
	};
	std::vector<node> nodes{};
	Integer first_leaf_index = 0;

	static Integer parent(Integer i) {
		return i / 2;
	}

	static Integer left_child(Integer i) {
		return 2 * i;
	}

	static Integer right_child(Integer i) {
		return (2 * i) + 1;
	}

	static const Integer ROOT = 1;

	Integer leaf_index(Integer l) {
		return first_leaf_index + l;
	}

	void update_internal(Integer i) {
		if (nodes[i].valid) {
			if (nodes[right_child(i)].valid) {
				nodes[i].value = nodes[left_child(i)].value + nodes[right_child(i)].value;
			} else {
				nodes[i].value = nodes[left_child(i)].value;
			}
		}
	}
};

struct stats {
	// if a < c and b = lca(a, c) then distance = depth(a) - 2 * depth(b) + depth(c)
	int depth;
	int max_depth; // maximum depth of prefix in subtree
	int min_depth;
	int best_left_lca; // maximum value of depth(a) - 2 * depth(b)
	int best_right_lca; // - 2 * depth(b) + depth(c)
	int diameter; // a < b < c

	friend stats operator+(stats lhs, stats const &rhs) {
		stats const rhs_adjusted{
			rhs.depth,
			rhs.max_depth + lhs.depth,
			rhs.min_depth + lhs.depth,
			rhs.best_left_lca - lhs.depth,
			rhs.best_right_lca - lhs.depth,
			rhs.diameter
		};
		return stats{
			lhs.depth + rhs.depth,
			std::max(lhs.max_depth, rhs_adjusted.max_depth),
			std::min(lhs.min_depth, rhs_adjusted.min_depth),
			std::max({lhs.max_depth - (2 * rhs_adjusted.min_depth), lhs.best_left_lca, rhs_adjusted.best_left_lca}),
			std::max({-(2 * lhs.min_depth) + rhs_adjusted.max_depth, lhs.best_right_lca, rhs_adjusted.best_right_lca}),
			std::max({lhs.max_depth + rhs_adjusted.best_right_lca, lhs.best_left_lca + rhs_adjusted.max_depth, lhs.diameter, rhs_adjusted.diameter})
		};
	}

	static stats LEFT_BRACKET() {
		return {1, 1, 0, 0, 1, 1};
	}
	static stats RIGHT_BRACKET() {
		return {-1, 0, -1, 2, 1, 1};
	}

	friend std::ostream &operator<<(std::ostream &out, stats const &x) {
		out << "{"
		<< x.depth << ", "
		<< x.max_depth << ", "
		<< x.min_depth << ", "
		<< x.best_left_lca << ", "
		<< x.best_right_lca << ", "
		<< x.diameter << "}";
		return out;
	}
};


struct Data {
	std::vector<int> brackets;
	std::vector<std::pair<int, int>> swaps;

	void Solve(std::istream &in, std::ostream &out) {
		segment_tree<stats, int> tree{brackets.size()};

		auto const update_bracket = [&](int i) {
			tree.value(i) = (brackets[i] ? stats::LEFT_BRACKET() : stats::RIGHT_BRACKET());
		};

		for (int i = 0; i < brackets.size(); ++i) {
			update_bracket(i);
		}
		tree.update_all_internal();

		out << tree.root_value().diameter << "\n";

		for (auto const &s : swaps) {
			if (brackets[s.first] != brackets[s.second]) {
				std::swap(brackets[s.first], brackets[s.second]);
				update_bracket(s.first);
				tree.update_from_leaf_to_root(s.first);
				update_bracket(s.second);
				tree.update_from_leaf_to_root(s.second);
			}

			out << tree.root_value().diameter << "\n";
		}
	}

	friend std::ostream &operator<<(std::ostream &out, Data const &data) {
		out << ((data.brackets.size() / 2) + 1) << " " << data.swaps.size() << "\n";
		for (auto const &x : data.brackets) {
			out << (x ? '(' : ')');
		}
		out << "\n";
		for (auto const &p : data.swaps) {
			out << (p.first + 1) << " " << (p.second + 1) << "\n";
		}
		return out;
	}

};

std::istream &operator>>(std::istream &in, Data &data) {
	int n;
	int q;
	in >> n >> q;
	n = 2 * (n - 1);

	data.brackets.resize(n);
	for (int i = 0; i < n; ++i) {
		char c;
		in >> c;
		data.brackets[i] = (c == '(');
	}

	data.swaps.resize(q);
	for (int i = 0; i < q; ++i) {
		in >> data.swaps[i].first >> data.swaps[i].second;
		--data.swaps[i].first;
		--data.swaps[i].second;
	}

	return in;
}

struct Tests {
	int const size = 3;
	void DescribeTest(std::ostream& out, int index, bool input) const {
		switch (index) {
		case 0:
			if (input) {
				Data const data{{1, 1, 0, 0}, {{1, 2}}};
				out << data;
			} else {
				out << "2\n2\n";
			}
			break;
		case 1:
			if (input) {
				Data const data{{1, 1, 1, 1, 0, 0, 0, 0}, {{3, 4}, {2, 3}, {4, 5}, {2, 5}, {1, 4}}};
				out << data;
			} else {
				out << "4\n3\n3\n2\n4\n4\n";
			}
			break;
		case 2:
			if (input) {
				out << "10 16\n";
				out << "(()(((()((()))))))\n";
				out << "6 14\n4 6\n14 12\n6 13\n13 15\n8 9\n14 15\n14 15\n6 11\n8 9\n9 4\n13 4\n9 15\n17 12\n11 10\n4 6\n";
			} else {
				out << "7\n5\n7\n8\n7\n6\n6\n6\n6\n6\n6\n5\n5\n7\n6\n5\n4\n";
			}
			break;
		}
	}
};
