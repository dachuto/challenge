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

struct Data {
	struct Road {
		int a;
		int b;
	};

	struct Query {
		int start_time;
		int finish_time;
		int source;
		int destination;
	};

	int intersections;
	std::vector<Road> roads;
	std::vector<Query> queries;

	struct AvailablePath {
		int start_time;
		int finish_time;
	};

	static bool PathExists(Query const &q, std::vector<AvailablePath> const &available_paths) {
		auto it = std::lower_bound(available_paths.cbegin(), available_paths.cend(), q.start_time, [](AvailablePath const &path, int value_start_time) {
			return path.finish_time < value_start_time;
		});

		while (it != available_paths.cend()) {
			if (it->start_time >= q.start_time and it->finish_time <= q.finish_time) {
				return true;
			}
			++it;
		}
		return false;
	}

	void Solve() const {
		struct IngoingPathInformation {
			std::vector<std::vector<AvailablePath>> paths;
		};

		std::vector<IngoingPathInformation> intersection_info(intersections + 1, {std::vector<std::vector<AvailablePath>>(intersections + 1)});

		int road_number = 1;
		for (auto const &road : roads) {
			auto const update_info = [&](int source, int destination) {
				for (int i = 1; i < intersections + 1; ++i) {
					if (i == destination) {
						continue;
					}
					auto const &source_paths = intersection_info[source].paths[i];
					auto &destination_paths = intersection_info[destination].paths[i];

					bool path_available = (i == source) or (not source_paths.empty());
					AvailablePath available{road_number, road_number};
					if (not source_paths.empty()) {
						available.start_time = source_paths.back().start_time;
					}

					if (path_available and (destination_paths.empty() or destination_paths.back().finish_time < available.start_time)) {
						destination_paths.push_back(available);
					}
				}
			};

			update_info(road.a, road.b);
			update_info(road.b, road.a);

			++road_number;
		}

//		for (int i = 1; i < intersections + 1; ++i) {
//			std::cout << "==========I " << i << "\n";
//			for (int o = 1; o < intersections + 1; ++o) {
//				std::cout << o << " --> ";
//				for (auto const &p : intersection_info[i].paths[o]) {
//					std::cout << " [" << p.start_time << ", " << p.finish_time << "]";
//				}
//				std::cout << "\n";
//			}
//		}
		for (auto const &q : queries) {
			bool const path_exists = PathExists(q, intersection_info[q.destination].paths[q.source]);
			std::cout << (path_exists ? "Yes" : "No") << "\n";
		}
	}
};

std::ostream &operator<<(std::ostream &os, Data const &data) {
	os << data.intersections << " " << data.roads.size() << " " << data.queries.size() << "\n";
	for (auto const &r : data.roads) {
		os << r.a << " " << r.b << "\n";
	}
	for (auto const &q : data.queries) {
		os << q.start_time << " " << q.finish_time << " " << q.source << " " << q.destination << "\n";
	}
	return os;
}

std::istream &operator>>(std::istream &is, Data &data) {
	std::size_t r, q;
	is >> data.intersections >> r >> q;

	data.roads.resize(r);
	data.queries.resize(q);

	for (auto &r : data.roads) {
		is >> r.a >> r.b;
	}

	for (auto &q : data.queries) {
		is >> q.start_time >> q.finish_time >> q.source >> q.destination;
	}
	return is;
}

struct Tests {
	void DescribeTest(std::ostream& out, int index, bool input) const {
		if (index == 0) {
			if (input) {
				Data const data = {2, {{1, 2}}, {{1, 1, 1, 2}}};
				std::cout << data;
			} else {
				std::cout << "Yes\n";
			}
		} else if (index == 1) {
			if (input) {
				Data const data = {4, {{1, 2}, {2, 4}, {3, 4}, {1, 3}, {2, 3}}, {{1, 2, 1, 4}}};
				std::cout << data;
			} else {
				std::cout << "Yes\n";
			}
		} else if (index == 2) {
			if (input) {
				Data const data = {5, {{1, 2}, {2, 3}, {3, 4}, {3, 5}}, {{1, 3, 1, 4}, {1, 3, 2, 4}, {1, 4, 4, 5}, {1, 4, 4, 1}, {2, 3, 1, 4}, {2, 2, 2, 3}}};
				std::cout << data;
			} else {
				std::cout << "Yes\nYes\nYes\nNo\nNo\nYes\n";
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

