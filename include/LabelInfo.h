#pragma once

#include "Data.h"
#include <cstdint>

namespace pof {
	struct LabelInfo {
		pof::base::data::text_t mProductName;
		pof::base::data::text_t mDirForUse;
		pof::base::data::text_t mWarning;
		pof::base::data::text_t mStrength;
		pof::base::data::text_t mStrengthType;
		pof::base::data::text_t mFormulation;
		std::uint64_t mQuantity = 0; //quantity * package size
	};
};