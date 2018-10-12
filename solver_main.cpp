int main(int argc, char *argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);
	Data data;
	std::cin >> data;
	data.Solve(std::cin, std::cout);
	return 0;
}
