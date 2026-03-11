#include <iostream>

#include "boost/uuid/random_generator.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"

int main()
{
	auto const u = boost::uuids::uuid(boost::uuids::random_generator{}());
	std::cout << u << '\n';
	return 0;
}