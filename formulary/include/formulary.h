#include <boost/noncopyable.hpp>
#include <Data.h>

#include <string>
#include <cstdint>
#include <chrono>

#include <list>

namespace ch = std::chrono;
using namespace std::literals::string_literals;
namespace pof
{
	class formulary : public boost::noncopyable
	{
	public:
		using clock_t = ch::system_clock;
		using datetime_t = ch::system_clock::time_point;
		formulary() = default;

	private:
		datetime_t created;
		std::string name;
		std::string insitution;


	};
};