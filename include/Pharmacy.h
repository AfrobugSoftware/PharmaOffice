#pragma once
#include <string>
#include <cstdint>
#include <boost/uuid/uuid.hpp>
#include <boost/noncopyable.hpp>

#include <nlohmann/json.hpp>
#include <memory>
#include <bitset>

#include "DataModel.h"

using namespace std::literals::string_literals;
namespace nl = nlohmann;
namespace pof{
	class Pharmacy : private boost::noncopyable {
	public:
		enum class PharmacyType : std::uint8_t
		{
			COMMUNITY = 1,
			HOSPITAL = 1 << 1,
			MANUFACTURE = 1 << 2,
			WHOLESALE = 1 << 3
		};
		
		using employeModel_t = std::shared_ptr<pof::DataModel>;
		struct address {
			std::string country;
			std::string state;
			std::string lga;
			std::string city;
			std::string street;
			std::string number;
			std::string postcode;

			struct geolocation {
				double longitude;
				double latitude;
			};
			geolocation location;
		};


		Pharmacy();
		virtual ~Pharmacy();

		inline const std::bitset<8>& GetPharmacyType() const { return pharmacyType; }
		inline void SetPharmacyType(const std::bitset<8>& bitset) { pharmacyType = bitset; }
		inline void SetPharmacyType(PharmacyType ty) {
			pharmacyType.set(static_cast<size_t>(std::underlying_type_t<PharmacyType>(ty)));
		}

		nl::json Pack() const;
		void UnPack(const nl::json& js);

	private:
		boost::uuids::uuid pharmacyID;
		boost::uuids::uuid branchID; //this particular branch
		std::bitset<8> pharmacyType;
		std::string name;
		address addy;
		

		//datamodels
		employeModel_t employeModel;

	};
};