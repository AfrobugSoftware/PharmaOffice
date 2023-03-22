#pragma once
#include <string>
#include <cstdint>
#include <boost/uuid/uuid.hpp>
#include <boost/noncopyable.hpp>

#include <nlohmann/json.hpp>
#include <memory>

#include "DataModel.h"

using namespace std::literals::string_literals;
namespace nl = nlohmann;
namespace pof{
	class Pharmacy : private boost::noncopyable {
	public:
		using employeModel_t = std::shared_ptr<pof::DataModel<std::uint64_t, std::string, std::string>>;
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

		nl::json Pack() const;
		void UnPack(const nl::json& js);

	private:
		boost::uuids::uuid pharmacyID;
		boost::uuids::uuid branchID; //this particular branch
		std::string name;
		address addy;
		

		//datamodels
		employeModel_t employeModel;

	};
};