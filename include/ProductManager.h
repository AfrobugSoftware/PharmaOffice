#pragma once
#include <boost/noncopyable.hpp>
#include "DataModel.h"

namespace pof {
	class ProductManager : private boost::noncopyable {
	public:
		ProductManager() {}
		~ProductManager() {}

		inline const pof::DataModel& GetBaseProductData() const { return mProductBaseData; }
		inline const pof::DataModel& GetAdditionalProductData() const { return mProductAdditionalData; }
		const pof::DataModel& GetInventoryForProduct(pof::DataModel::const_iterator ProductItem) const;

	private:
		pof::DataModel mProductBaseData;
		pof::DataModel mProductAdditionalData;
		pof::DataModel mInventoryData;
	};

};