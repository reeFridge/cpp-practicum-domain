#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <cassert>

using namespace std;

class Domain {
public:
	static constexpr char delimiter = '.';

	Domain(const std::string& str) : str_reversed_(str) {
		reverse(str_reversed_.begin(), str_reversed_.end());
		str_reversed_.push_back(delimiter);
	}

	bool operator==(const Domain& rhs) {
		return str_reversed_ == rhs.str_reversed_;
	}

	bool IsSubdomain(const Domain& parent) const {
		if (str_reversed_.size() < parent.str_reversed_.size()) {
			return false;
		}

		auto res = std::mismatch(parent.str_reversed_.cbegin(), parent.str_reversed_.cend(), str_reversed_.cbegin());

		return res.first == parent.str_reversed_.end();
	}

	const string& GetReversedString() const {
		return str_reversed_;
	}

private:
	string str_reversed_;
};

class DomainChecker {
public:
	template<typename Iterator>
	DomainChecker(Iterator first, Iterator last) : forbidden_domains_(first, last) {
		sort(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& lhs, const Domain& rhs){
			return lhs.GetReversedString() < rhs.GetReversedString();
		});

		last = std::unique(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& lhs, const Domain& rhs){
			return rhs.IsSubdomain(lhs);
		});

		forbidden_domains_.erase(last, forbidden_domains_.end());
	}

	bool IsForbidden(const Domain& domain) const {
		auto it = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domain, [](const Domain& lhs, const Domain& rhs){
			return lhs.GetReversedString() < rhs.GetReversedString();
		});

		return it != forbidden_domains_.begin() && domain.IsSubdomain(*prev(it));
	}

private:
	vector<Domain> forbidden_domains_;
};

vector<Domain> ReadDomains(istream& input, size_t count) {
	vector<Domain> domains;
	domains.reserve(count);

	string line;
	for (size_t i = 0; i < count; ++i) {
		getline(input, line);

		domains.push_back(line);
	}

	return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
	string line;
	getline(input, line);

	Number num;
	std::istringstream(line) >> num;

	return num;
}

void TestDomainEqualOp() {
	assert(Domain("a.b.c"s) == Domain("a.b.c"s));
	assert(!(Domain("a.b.c"s) == Domain("A.B.C")));
}

void TestDomainIsSubdomain() {
	{
		const Domain domain("a.b.c"s);
		assert(domain.IsSubdomain(Domain("b.c"s)));
	}

	{
		const Domain domain("gdz.com"s);
		assert(domain.IsSubdomain(Domain("com"s)));
	}

	{
		const Domain domain("gdz.org"s);
		assert(!domain.IsSubdomain(Domain("com"s)));
	}
}

void TestDomainChecker() {
	const std::vector<Domain> forbidden_domains{
		"gdz.ru"s,
		"maps.me"s,
		"m.gdz.ru"s,
		"com"s
	};

	const DomainChecker domain_checker(forbidden_domains.cbegin(), forbidden_domains.cend());

	const std::vector<Domain> test_domains{
		"gdz.ru"s,
		"gdz.com"s,
		"m.maps.me"s,
		"alg.m.gdz.ru"s,
		"maps.com"s,
		"maps.ru"s,
		"gdz.ua"s
	};

	assert(domain_checker.IsForbidden(test_domains[0]));
	assert(domain_checker.IsForbidden(test_domains[1]));
	assert(domain_checker.IsForbidden(test_domains[2]));
	assert(domain_checker.IsForbidden(test_domains[3]));
	assert(domain_checker.IsForbidden(test_domains[4]));
	assert(!domain_checker.IsForbidden(test_domains[5]));
	assert(!domain_checker.IsForbidden(test_domains[6]));
}

void RunTests() {
	TestDomainEqualOp();
	TestDomainIsSubdomain();
	TestDomainChecker();
}

int main() {
	RunTests();

	const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
	DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

	const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
	for (const Domain& domain : test_domains) {
		cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
	}
}
