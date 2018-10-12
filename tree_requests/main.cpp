#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>
#include <stack>
#include <string>
#include <vector>

using prefix_sum_t = std::bitset<26>;

struct Node {
	std::vector<int> children;
	int dfs_order_pre;
	int dfs_order_post;
	int depth;
	prefix_sum_t prefix_sum;
};

template <typename It, typename ByDepth>
void dfs_tree(int starting_node, It nodes, ByDepth &nodes_by_depth) {
	std::stack<int> s;
	int order = 0;

	s.push(starting_node);
	nodes[starting_node].dfs_order_pre = 0;

	while (not s.empty()) {
		int const node_index = s.top();
		s.pop();
		Node &node = nodes[node_index];

		if (node.dfs_order_pre == 0) {
			++order;
			node.dfs_order_pre = order;
			nodes_by_depth[node.depth].push_back(node_index);

			s.push(node_index);
			for (auto it = node.children.crbegin(); it != node.children.crend(); ++it) {
				s.push(*it);
			}
		} else {
			++order;
			node.dfs_order_post = order;
		}
	}
}

struct Data {
	std::vector<int> parent;
	int queries;
	std::string letters;

	void Solve(std::istream &in, std::ostream &out) const {
		std::vector<Node> nodes;
		nodes.resize(parent.size());
		nodes[0].depth = 1;
		for (int i = 1; i < parent.size(); ++i) {
			nodes[parent[i]].children.push_back(i);
			nodes[i].depth = nodes[parent[i]].depth + 1;
		}

		std::vector<std::vector<int>> nodes_by_depth;
		nodes_by_depth.resize(nodes.size() + 1);

		dfs_tree(0, nodes.begin(), nodes_by_depth);

		for (auto const &nodes_at_depth : nodes_by_depth) {
			for (size_t i = 1; i < nodes_at_depth.size(); ++i) {
				auto const previous_node_index = nodes_at_depth[i -1];
				Node const &previous_node = nodes[previous_node_index];
				Node &current_node = nodes[nodes_at_depth[i]];

				auto temp = previous_node.prefix_sum;
				temp.flip(letters[previous_node_index] - 'a');
				current_node.prefix_sum = temp;
			}
		}

		for (int i = 0; i < queries; ++i) {
			int vertex;
			int depth;
			in >> vertex >> depth;
			--vertex;

			auto const first_in_subtree = std::lower_bound(nodes_by_depth[depth].cbegin(), nodes_by_depth[depth].cend(), nodes[vertex].dfs_order_pre, [&nodes](int index, int value) {
				return nodes[index].dfs_order_pre < value;
			});

			auto const last_in_subtree = std::upper_bound(nodes_by_depth[depth].cbegin(), nodes_by_depth[depth].cend(), nodes[vertex].dfs_order_post, [&nodes](int value, int index) {
				return value < nodes[index].dfs_order_pre;
			});

			if (first_in_subtree == last_in_subtree) {
				out << "Yes\n";
				continue;
			}

			{
				auto const b = last_in_subtree - 1;
				auto const number_of_letters = std::distance(first_in_subtree, last_in_subtree);
				auto letters_parity = nodes[*first_in_subtree].prefix_sum ^ nodes[*b].prefix_sum;
				size_t const letter_type = letters[*b] - 'a';
				letters_parity.flip(letter_type);

				if (letters_parity.count() == static_cast<size_t>(number_of_letters % 2)) {
					out << "Yes\n";
				} else {
					out << "No\n";
				}
			}
		}
	}
};

std::ostream &operator<<(std::ostream &out, Data const &data) {
	out << data.parent.size() << " " << data.queries << "\n";
	for (int i = 1; i < data.parent.size(); ++i) {
		out << data.parent[i] + 1 << " ";
	}
	out << "\n";
	out << data.letters << "\n";
	return out;
}

std::istream &operator>>(std::istream &in, Data &data) {
	int number_of_nodes;
	in >> number_of_nodes >> data.queries;
	data.parent.resize(number_of_nodes);

	data.parent[0] = 0;
	for (int i = 1; i < number_of_nodes; ++i) {
		int parent;
		in >> parent;
		data.parent[i] = parent - 1;
	}
	in >> data.letters;
	return in;
}

struct Tests {
	int const size = 1;
	void DescribeTest(std::ostream& out, int index, bool input) const {
		switch (index) {
		case 0:
			if (input) {
				Data const data{{0, 0, 0, 0, 2, 2}, 5, "zacccd"};
				out << data;
				out << "1 1\n3 3\n4 1\n6 1\n1 2\n";
			} else {
				out << "Yes\nNo\nYes\nYes\nYes\n";
			}
			break;
		}
	}
};
