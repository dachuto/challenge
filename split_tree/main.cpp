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

using index_t = int32_t;
using value_t = int64_t;

struct Data {
	struct node {
		index_t parent;
		value_t value;
	};

	index_t LENGTH_MAX = 0;
	value_t SUM_MAX = 0;
	std::vector<node> nodes;

	void Solve(std::istream &in, std::ostream &out) const {
		for (auto const &n: nodes) {
			if (n.value > SUM_MAX) {
				out << -1 << "\n";
				return;
			}
		}

		index_t const N = nodes.size();
		std::vector<index_t> children;
		children.resize(N);
		std::vector<std::vector<index_t>> children_list;
		children_list.resize(N);

		for (index_t i = 1; i < N; ++i) {
			++children[nodes[i].parent];
			children_list[nodes[i].parent].push_back(i);
		}

		std::vector<index_t> bottom_up_order;
		bottom_up_order.reserve(N);

		{
			std::stack<index_t> to_consider;
			to_consider.push(0);
			bottom_up_order.push_back(0);

			while (not to_consider.empty()) {
				index_t const i = to_consider.top();
				to_consider.pop();

				for (auto const &child : children_list[i]) {
					to_consider.push(child);
					bottom_up_order.push_back(child);
				}
			}
			std::reverse(bottom_up_order.begin(), bottom_up_order.end());
		}

		std::vector<std::vector<index_t>> ancestors; // ancestor[l][i] = x (x is 2^l ancestor of i)
		int const log_N = 20;
		index_t const NO_INDEX = -1;
		ancestors.resize(log_N);
		for (auto &v : ancestors) {
			v.resize(N);
		}

		ancestors[0][0] = NO_INDEX;
		for (index_t i = 1; i < N; ++i) {
			ancestors[0][i] = nodes[i].parent;
		}

		for (int level = 1; level < log_N; ++level) {
			for (index_t i = 0; i < N; ++i) {
				auto const previous_level = level - 1;
				auto const previous_level_ancestor = ancestors[previous_level][i];
				bool const previous_level_ancestor_exists = (previous_level_ancestor != NO_INDEX);
				ancestors[level][i] = previous_level_ancestor_exists ? ancestors[previous_level][previous_level_ancestor] : NO_INDEX;
			}
		}

		std::vector<value_t> sum_values_from_root;
		sum_values_from_root.resize(N);

		std::vector<index_t> depth;
		depth.resize(N);

		std::for_each(bottom_up_order.rbegin(), bottom_up_order.rend(), [&](index_t const &i) {
			sum_values_from_root[i] = sum_values_from_root[nodes[i].parent] + nodes[i].value;
			depth[i] = depth[nodes[i].parent] + 1;
		});

		std::vector<index_t> reachable_node;
		reachable_node.resize(N);
		for (index_t i = 0; i < N; ++i) {
			index_t path_end = i;
			for (int level = log_N - 1; level >= 0; --level) {
				index_t const new_path_end = ancestors[level][path_end];
				if (new_path_end == NO_INDEX) {
					continue;
				}
				auto const path_length = depth[i] - depth[new_path_end] + 1;
				auto const path_sum = sum_values_from_root[i] - sum_values_from_root[new_path_end] + nodes[new_path_end].value;
				if (path_length <= LENGTH_MAX and path_sum <= SUM_MAX) {
					path_end = new_path_end;
				}
			}
			reachable_node[i] = path_end;
		}

		std::vector<index_t> subtree_answer;
		subtree_answer.resize(N);

		std::vector<index_t> best_path_start_subtree;
		best_path_start_subtree.resize(N);
		for (auto const &i : bottom_up_order) {
			value_t sum_of_answers_in_subtrees = 0;
			for (auto const &child : children_list[i]) {
				sum_of_answers_in_subtrees += subtree_answer[child];
			}
			subtree_answer[i] = sum_of_answers_in_subtrees;

			auto const child_best_reachable_depth = [&](index_t const &x) {
				return depth[reachable_node[best_path_start_subtree[x]]];
			};
			auto const best_child_it = std::min_element(children_list[i].cbegin(), children_list[i].cend(), [&](index_t a, index_t b) {
				return child_best_reachable_depth(a) < child_best_reachable_depth(b);
			});

			if (best_child_it != children_list[i].cend() and child_best_reachable_depth(*best_child_it) <= depth[i]) {
				best_path_start_subtree[i] = best_path_start_subtree[*best_child_it];
			} else {
				++subtree_answer[i];
				best_path_start_subtree[i] = i;
			}
		}

		if (false) {
			for (auto const &i : bottom_up_order) {
				out << i << " > ";
			}
			out << "\n";
			for (index_t i = 0; i < N; ++i) {
				out << i << " depth:" << depth[i];
				out << " sum:" << sum_values_from_root[i];
				out << " reachable:" << reachable_node[i];
				out << " A:" << subtree_answer[i];
				out << " best:" << best_path_start_subtree[i];
				out << " | ";
				for (int level = 0; level < log_N; ++level) {
					auto const a = ancestors[level][i];
					if (a == NO_INDEX) {
						out << ".";
					} else {
						out << a;
					}
					out << " ";
				}
				out << "\n";
			}
		}
		out << subtree_answer[0] << "\n";
		return;
	}
};

std::ostream &operator<<(std::ostream &out, Data const &data) {
	out << data.nodes.size() << " " << data.LENGTH_MAX << " " << data.SUM_MAX << "\n";
	for (auto const &n : data.nodes) {
		out << n.value << " ";
	}
	out << "\n";
	for (index_t i = 1; i < data.nodes.size(); ++i) {
		out << data.nodes[i].parent + 1 << " ";
	}
	out << "\n";
	return out;
}

std::istream &operator>>(std::istream &in, Data &data) {
	index_t number_of_nodes = 0;
	in >> number_of_nodes >> data.LENGTH_MAX >> data.SUM_MAX;
	data.nodes.resize(number_of_nodes);
	for (index_t i = 0; i < number_of_nodes; ++i) {
		in >> data.nodes[i].value;
	}

	data.nodes[0].parent = 0;
	for (index_t i = 1; i < number_of_nodes; ++i) {
		index_t parent = 0;
		in >> parent;
		data.nodes[i].parent = parent - 1;
	}

	return in;
}

struct Tests {
	int const size = 7;
	void DescribeTest(std::ostream& out, int index, bool input) const {
		switch (index) {
		case 0:
			if (input) {
				Data const data{1, 3, {{0, 1}, {0, 2}, {0, 3}}};
				out << data;
			} else {
				out << "3\n";
			}
			break;
		case 1:
			if (input) {
				Data const data{1, 123456789012, {{0, 1234567890123}}};
				out << data;
			} else {
				out << "-1\n";
			}
			break;
		case 2:
			if (input) {
				Data const data{3, 6, {{0, 2}, {0, 2}, {0, 2}, {1, 2}, {1, 1}, {2, 3}, {3, 3}}};
				out << data;
			} else {
				out << "3\n";
			}
			break;
		case 3:
			if (input) {
				Data const data{4, 4, {{0, 1}, {0, 1}, {1, 1}, {1, 3}, {2, 1}}};
				out << data;
			} else {
				out << "2\n";
			}
			break;
		case 4:
			if (input) {
				Data const data{3, 4, {{0, 1}, {0, 2}, {0, 3}, {1, 3}, {1, 3}, {1, 3}}};
				out << data;
			} else {
				out << "5\n";
			}
			break;
		case 5:
			if (input) {
				Data const data{2, 11122233344455566, {{0, 1112223334445556}, {0, 1112223334445556}, {0, 11122233344455566}}};
				out << data;
			} else {
				out << "2\n";
			}
			break;
		case 6:
			if (input) {
				Data const data{4, 4, {{0, 1}, {3, 1}, {1, 1}, {0, 1}, {0, 1}}};
				out << data;
			} else {
				out << "2\n";
			}
			break;
		}
	}
};
