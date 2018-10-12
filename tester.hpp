#include <iostream>
#include <string>

template <typename Tests>
int Dispatch(int argc, char *argv[], Tests const &tests) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	if (argc == 1) {
		std::cout << tests.Size() << std::endl;
	} else if (argc == 3) {
		std::string const in_or_out(argv[1]);
		int const index = std::stoi(argv[2]);
		bool input = true;
		if (in_or_out == "input") {
		} else if (in_or_out == "output") {
			input = false;
		} else {
			return 1;
		}

		if (index < 0 or index >= tests.Size()) {
			return 1;
		}

		tests.DescribeTest(std::cout, index, input);
	} else {
		return 1;
	}

	return 0;
}
