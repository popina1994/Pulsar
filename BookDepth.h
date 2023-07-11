#pragma once

#include <vector>
#include <algorithm>
#include "PriceQuantity.h"

namespace Pulsar
{
	struct BookDepth
	{
		std::vector<PriceQuantity> m_vBids;
		std::vector<PriceQuantity> m_vAsks; // or could be e.g. boost::static_vector/small_vector
		

		BookDepth(const std::vector<PriceQuantity>& vBids, const std::vector<PriceQuantity>& vAsks)
			: m_vBids(vBids), m_vAsks(vAsks)
		{
		}

		void clear(void) 
		{ 
			m_vBids.clear();
			m_vAsks.clear();
		}
	};
}
