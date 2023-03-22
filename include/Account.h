#pragma once
#include <string>
#include <chrono>
#include <boost/signals2.hpp>
#include <nlohmann/json.hpp>
#include <array>
#include <cstdint>
#include <boost/noncopyable.hpp>
#include <bitset>

namespace nl = nlohmann;
namespace pof {
	class Account : private boost::noncopyable {
	public:
		using clock_t = std::chrono::system_clock;
		using datetime_t = std::chrono::system_clock::time_point;
		using signal_t = boost::signals2::signal<void(const Account&)>;
		using privilage_set_t = std::bitset<8>;

		enum class Privilage: std::uint8_t {
				PHARMACIST = 1,
				PHARMTECH = 1 << 1,
				DISPENSER = 1 << 2,
				CASHER = 1 << 3,
				SALES_ASSISTANT = 1 << 4,
				INTER_PHARMCIST = 1 << 5,
				STUDENT_PHARMACIST = 1 << 6
		};
		
		Account();
		~Account();


		
		void SetName(const std::string& n);
		void SetEmail(const std::string& e);
		void SetPhoneNumber(const std::string& pn);
	
		const std::string& GetName() const;
		const std::string& GetEmail() const;
		const std::string& GetPhoneNumber() const;

		nl::json Pack() const;
		void UnPack(const nl::json& package);

	private:
		privilage_set_t priv;
		datetime_t signintime;
		std::uint64_t accountID;
		std::string name;
		std::string email;
		std::string phonenumber;

		//account details?


	};
};