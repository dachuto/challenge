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

struct Data {
	void Solve(std::istream &in, std::ostream &out) const {
	}
};

std::ostream &operator<<(std::ostream &os, Data const &data) {
	return os;
}

std::istream &operator>>(std::istream &is, Data &data) {
	return is;
}

struct Tests {
	int const size = 0;
	void DescribeTest(std::ostream& out, int index, bool input) const {
		switch (index) {
		case 0:
			if (input) {
				Data const data;
				out << data;
			} else {
				out << "solution\n";
			}
			break;
		}
	}
};
