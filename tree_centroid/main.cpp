#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#ifdef TESTER_VERSION
#include "../tester.hpp"
#endif

struct Data {
	std::vector<int> parent;
	std::vector<int> query;

	void Solve() const {
		std::vector<int> number_of_children(parent.size(), 0);
		std::vector<int> subtree_size(parent.size(), 1);
		std::vector<int> biggest_subtree_root(parent.size(), 0);

		for (size_t i = 1; i < parent.size(); ++i) {
			++number_of_children[parent[i]];
		}

		std::vector<int> number_of_children_to_evalate(number_of_children.cbegin(), number_of_children.cend());

		std::queue<int> q;

		std::vector<int> centroid(parent.size(), 0);

		for (size_t i = 0; i < parent.size(); ++i) {
			if (number_of_children[i] == 0) {
				q.push(i);
			}
		}

		while (not q.empty()) {
			int const i = q.front();
			q.pop();

			if (i != 0) {
				subtree_size[parent[i]] += subtree_size[i];
				if (biggest_subtree_root[parent[i]] == 0 or subtree_size[biggest_subtree_root[parent[i]]] < subtree_size[i]) {
					biggest_subtree_root[parent[i]] = i;
				}

				--number_of_children_to_evalate[parent[i]];
				if (number_of_children_to_evalate[parent[i]] == 0) {
					q.push(parent[i]);
				}
			}

			if (number_of_children[i] == 0 or subtree_size[i] >= 2 * subtree_size[biggest_subtree_root[i]]) {
				centroid[i] = i;
			} else {
				int currently_considered_centroid = centroid[biggest_subtree_root[i]];
				while (true) {
					int const size_of_subtree_above_centroid = subtree_size[i] - subtree_size[currently_considered_centroid];
					if (subtree_size[i] >= 2 * size_of_subtree_above_centroid) {
						break;
					}
					currently_considered_centroid = parent[currently_considered_centroid];
				}
				centroid[i] = currently_considered_centroid;
			}
		}

//		for (size_t i = 0; i < parent.size(); ++i) {
//			std::cout << subtree_size[i] << " ";
//		}
//		std::cout << "\n";
//
//		for (size_t i = 0; i < parent.size(); ++i) {
//			std::cout << biggest_subtree_root[i] << " ";
//		}
//		std::cout << "\n";
//
//		for (size_t i = 0; i < parent.size(); ++i) {
//			std::cout << centroid[i] << " ";
//		}
//		std::cout << "\n";

		for (auto q : query) {
			std::cout << centroid[q] + 1 << "\n";
		}
	}
};

std::ostream &operator<<(std::ostream &os, Data const &data) {
	os << data.parent.size() << " " << data.query.size() << "\n";
	for (size_t i = 1; i < data.parent.size(); ++i) {
		std::cout << (data.parent[i] + 1) << " ";
	}

	os << "\n";

	for (auto const &q : data.query) {
		std::cout << (q + 1) << "\n";
	}
	return os;
}

std::istream &operator>>(std::istream &is, Data &data) {
	size_t n;
	size_t q;
	is >> n >> q;

	data.parent.resize(n);
	data.query.resize(q);

	data.parent[0] = 0;
	for (size_t i = 1; i < n; ++i) {
		int temp;
		is >> temp;
		data.parent[i] = temp - 1;
	}

	for (auto &q : data.query) {
		int temp;
		is >> temp;
		q = temp - 1;
	}

	return is;
}

struct Tests {
	void DescribeTest(std::ostream& out, int index, bool input) const {
		if (index == 0) {
			if (input) {
				Data const data = {{0, 0, 0, 2, 2, 4, 2}, {0, 1, 2, 4}};
				std::cout << data;
			} else {
				std::cout << "3\n2\n3\n5\n";
			}
		} else if (index == 1) {
			if (input) {
				Data const data = {{0, 0}, {0, 1}};
				std::cout << data;
			} else {
				std::cout << "1\n2\n";
			}
		} else if (index == 2) {
			if (input) {
				Data const data = {{0, 0, 1, 1, 1}, {0, 1, 2}};
				std::cout << data;
			} else {
				std::cout << "2\n2\n3\n";
			}
		} else if (index == 3) {
			int const n = 300000;
			if (input) {
				Data data;
				data.parent.resize(n);
				data.parent[0] = 0;
				std::iota(std::next(data.parent.begin()), data.parent.end(), 0);
				data.query.resize(n);
				std::iota(data.query.begin(), data.query.end(), 0);
				std::cout << data;
			} else {
				for (int i = 0; i < n; ++i) {
					int const x = (n - i);
					int k = n - (x - 1) / 2;
					if (x == 2) {
						k = n - 1;
					}
					std::cout << k << "\n";
				}
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
