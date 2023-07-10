#pragma once

#include <vector>
#include "PriceQuantity.h"

namespace Pulsar
{
	struct BookDepth
	{
		std::vector<PriceQuantity> bids;
		std::vector<PriceQuantity> asks; // or could be e.g. boost::static_vector/small_vector
	};
}
