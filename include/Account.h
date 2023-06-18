#pragma once
#include <string>
#include <chrono>
#include <boost/signals2.hpp>
#include <nlohmann/json.hpp>
#include <array>
#include <cstdint>
#include <boost/noncopyable.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <bitset>
#include <fmt/format.h>

namespace nl = nlohmann;
namespace pof {
	class Account : private boost::noncopyable {
	public:
		using clock_t = std::chrono::system_clock;
		using datetime_t = std::chrono::system_clock::time_point;
		using signal_t = boost::signals2::signal<void(const Account&)>;
		using privilage_set_t = std::bitset<8>;
		constexpr static const size_t max_account_type = 7;
		constexpr static const std::array<std::string_view, max_account_type> account_type_string = {
			"PHARMACIST", "PHARMTECH", "DISPENSER", "SALES_ASSISTANT", "INTERN_PHARMACIST", "STUDENT_PHARMACIST", 
				"MANAGER"
		};
		enum class Privilage: std::uint8_t {
				PHARMACIST = 1,
				PHARMTECH = 1 << 1,
				DISPENSER = 1 << 2,
				SALES_ASSISTANT = 1 << 3,
				INTEN_PHARMCIST = 1 << 4,
				STUDENT_PHARMACIST = 1 << 5,
				MANAGER = 1 << 6
		};
		
		Account();
		~Account();


		nl::json Pack() const;
		void UnPack(const nl::json& package);

		inline constexpr const std::string& GetName() const { return name; }
		inline constexpr std::uint64_t GetID() const { return accountID; }
		inline constexpr const std::string& GetEmail() const { return email; }
		inline constexpr const std::string& GetPhonenumber() const { return phonenumber; }
		inline constexpr const std::string& GetRegnumber() const { return regnumber; }
		inline constexpr const boost::uuids::uuid& GetSessionUUID() const { return sessionID; }

		std::string GetAccountTypeString();
		inline constexpr datetime_t GetSignInTime() const { return signintime; }
		inline bool TestPriv(Privilage p) const {
			return priv.test(std::underlying_type_t<Privilage>(p));
		}

		boost::uuids::uuid sessionID;
		privilage_set_t priv;
		datetime_t signintime;
		std::uint64_t accountID = 0;
		std::string name;
		std::string lastname;
		std::string username;
		std::string email;
		std::string phonenumber;
		std::string regnumber;
		std::string passhash;
		bool isLoccum = false; 
		//account details?
	};
};