//TODO: Not finished! Codeforces 3(1/2)x

#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <stack>
#include <vector>

enum class TokenType {
	BRACE_LEFT,
	BRACE_RIGHT,
	PARSE_EXPRESSION,
	PARSE_OP,
};

struct ParserState {
	TokenType token;
	int parent;
};

using dynamic_t = std::array<std::array<int, 16>, 2>;

template <int MOD>
struct Mod {
	static int add(int a, int b) {
		return (a + b >= MOD) ? (a + b - MOD) : (a + b);
	}

	static int sub(int a, int b) {
		return (a - b < 0) ? (a - b + MOD) : (a - b);
	}

	static int mul(int a, int b) {
		return (static_cast<long long>(a) * b) % MOD;
	}
};

int constexpr MAGIC_PRIME = int(1e9) + 7;
using M = Mod<MAGIC_PRIME>;

struct Token {
	bool variable;
	dynamic_t number_of_ways_to_fill;

	char op_char = 'x';
	int parent = -1;
	int child_left = -1;
	int child_right = -1;
};

struct Rule {
	std::bitset<4> variables;
	bool value;
};

bool variable_matches_rule(size_t which_variable, Rule const &rule) {
	size_t const index = which_variable / 2;
	bool const negation = which_variable % 2;
	bool const match = (rule.variables[index] != rule.value) xor negation;
	return match;
}

template <typename ForwardIt>
void parse_generic(ForwardIt first, ForwardIt last, std::vector<Rule> const &rules, std::vector<Token> &tokens) {
	std::stack<ParserState> parser;
	parser.push({TokenType::PARSE_EXPRESSION, -1});

	static std::string const VARIABLE_CHARS = "AaBbCcDd?";
	static std::string const OP_CHARS = "&|?";

	while (first != last and not parser.empty()) {
		ParserState const state = parser.top();
		parser.pop();

		switch (state.token) {
		case TokenType::BRACE_LEFT:
			assert(*first == '(');
			break;
		case TokenType::BRACE_RIGHT:
			assert(*first == ')');
			break;
		case TokenType::PARSE_EXPRESSION: {
			size_t const which_variable = VARIABLE_CHARS.find(*first);
			if (which_variable != std::string::npos) {
				dynamic_t number_of_ways;

				for (int b = 0; b < 2; ++b) {
					for (unsigned v = 0; v < 16; ++v) {
						std::bitset<4> const bits(v);
						if (which_variable == 8) {
							for (size_t i = 0; i < 8; ++i) {
								for (Rule const &r : rules) {
									if (variable_matches_rule(which_variable, r)) {
										number_of_ways[b][v] += 1;
									}
								}
							}
						} else {
							number_of_ways[b][v] = variable_matches_rule(which_variable, Rule{bits, static_cast<bool>(b)});
						}
					}
				}

				if (state.parent >=0) {
					if (tokens[state.parent].child_left > 0) {
						tokens[state.parent].child_right = tokens.size();
					} else {
						tokens[state.parent].child_left = tokens.size();
					}
				}
				tokens.push_back({true, number_of_ways});
			} else if (*first == '(') {
				int const parent = tokens.size();
				parser.push({TokenType::BRACE_RIGHT, parent});
				parser.push({TokenType::PARSE_EXPRESSION, parent});
				parser.push({TokenType::BRACE_LEFT, parent});
				parser.push({TokenType::PARSE_OP, parent});
				parser.push({TokenType::BRACE_RIGHT, parent});
				parser.push({TokenType::PARSE_EXPRESSION, parent});

				Token token;
				token.variable = false;
				token.parent = state.parent;
				//token.child_left = parent + 1;
				tokens.push_back(token);
			} else {
				assert(false);
			}
		}
			break;
		case TokenType::PARSE_OP:
			size_t const it = OP_CHARS.find(*first);
			if (it != std::string::npos) {
				tokens[state.parent].op_char = OP_CHARS[it];
			} else {
				assert(false);
			}
			break;
		}

		{
			int parent = state.parent;
			while (parent >= 0) {
				Token &exp_token = tokens[parent];
				if (exp_token.child_right < 0) {
					break;
				}

				for (int b = 0; b < 2; ++b) {
					for (unsigned v = 0; v < 16; ++v) {
						auto const temp = [&](int a, int b) {
							return M::mul(tokens[exp_token.child_left].number_of_ways_to_fill[a][v], tokens[exp_token.child_right].number_of_ways_to_fill[b][v]);
						};

						if (exp_token.op_char == '&') {
							exp_token.number_of_ways_to_fill[b][v] =  b ? temp(1, 1) : M::add(temp(0,0), M::add(temp(0, 1), temp(1, 0)));
						} else if (exp_token.op_char == '|') {
							exp_token.number_of_ways_to_fill[b][v] =  b ? M::add(temp(1, 1), M::add(temp(0, 1), temp(1, 0))) : temp(0, 0);
						} else if (exp_token.op_char == '?') {
							exp_token.number_of_ways_to_fill[b][v] =  M::add(M::add(temp(0, 0), temp(1, 1)), M::add(temp(0, 1), temp(1, 0)));
						} else {
							assert(false);
						}
					}
				}

				parent = exp_token.parent;
			}
		}

		++first;
	}

	assert(first == last and parser.empty());
}

int main() {
	std::ios_base::sync_with_stdio(false);
//	std::cin.tie(nullptr);
	std::string expression;
	std::cin >> expression;

	int number_of_rules;
	std::cin >> number_of_rules;

	std::vector<Rule> rules;
	rules.resize(number_of_rules);
	for (int i = 0; i < number_of_rules; ++i) {
		int a, b, c, d, e;
		std::bitset<4> v;
		std::cin >> a >> b >> c >> d >> e;
		v[0] = a;
		v[1] = b;
		v[2] = c;
		v[3] = d;
		rules.push_back(Rule{v, static_cast<bool>(e)});
	}

	std::vector<Token> tokens;
	parse_generic(expression.cbegin(), expression.cend(), rules, tokens);

	for (size_t i = 0; i < tokens.size(); ++i) {
		std::cout << i << " => ";
		for (int b = 0; b < 2; ++b) {
			for (unsigned v = 0; v < 16; ++v) {
				std::cout << "[b:" << b << " v:" << v << " = " << tokens[i].number_of_ways_to_fill[b][v] << "]\n";
			}
		}
		std::cout << "\n";
	}

	int result = 0;
	for (int b = 0; b < 2; ++b) {
		for (unsigned v = 0; v < 16; ++v) {
			result = M::add(result, tokens[0].number_of_ways_to_fill[b][v]);
		}
	}

	std::cout << result;

	return 0;
}
