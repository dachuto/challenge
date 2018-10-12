#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>
#include <stack>
#include <vector>

typedef std::bitset<26> prefix_sum_t;

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

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	int number_of_nodes;
	int queries;
	std::cin >> number_of_nodes >> queries;
	std::vector<Node> nodes;
	nodes.resize(number_of_nodes);
	std::vector<std::vector<int>> nodes_by_depth;
	nodes_by_depth.resize(number_of_nodes + 1);

	nodes[0].depth = 1;

	for (int i = 1; i < number_of_nodes; ++i) {
		int parent;
		std::cin >> parent;
		--parent;
		nodes[i].depth = nodes[parent].depth + 1;
		nodes[parent].children.push_back(i);
	}

	std::string letters;
	std::cin >> letters;

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
		std::cin >> vertex >> depth;
		--vertex;

		auto const first_in_subtree = std::lower_bound(nodes_by_depth[depth].cbegin(), nodes_by_depth[depth].cend(), nodes[vertex].dfs_order_pre, [&nodes](int index, int value) {
			return nodes[index].dfs_order_pre < value;
		});

		auto const last_in_subtree = std::upper_bound(nodes_by_depth[depth].cbegin(), nodes_by_depth[depth].cend(), nodes[vertex].dfs_order_post, [&nodes](int value, int index) {
			return value < nodes[index].dfs_order_pre;
		});

		if (first_in_subtree == last_in_subtree) {
			std::cout << "Yes\n";
			continue;
		}

		{
			auto const b = last_in_subtree - 1;
			auto const number_of_letters = std::distance(first_in_subtree, last_in_subtree);
			auto letters_parity = nodes[*first_in_subtree].prefix_sum ^ nodes[*b].prefix_sum;
			size_t const letter_type = letters[*b] - 'a';
			letters_parity.flip(letter_type);

			//std::cout << " #" << letters << " " << letters_parity;

			if (letters_parity.count() == static_cast<size_t>(number_of_letters % 2)) {
				std::cout << "Yes\n";
			} else {
				std::cout << "No\n";
			}
		}

	}

	return 0;
}
