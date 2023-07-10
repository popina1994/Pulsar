#pragma once

#include <vector>
#include <algorithm>
#include "PriceQuantity.h"

namespace Pulsar
{
	struct BookDepth
	{
		static constexpr uint32_t NUM_BIDS_TO_KEEP = 21;

		std::vector<PriceQuantity> m_vBids;
		std::vector<PriceQuantity> m_vAsks; // or could be e.g. boost::static_vector/small_vector
		uint32_t m_nBids = 0;
		uint32_t m_nAsks = 0;
		
		BookDepth(): m_vBids(NUM_BIDS_TO_KEEP), m_vAsks(NUM_BIDS_TO_KEEP) {}

		void clear(void) { 
			m_nBids = 0;
			m_nAsks = 0;
			std::fill(m_vBids.begin(), m_vBids.end(), 0.0);
			std::fill(m_vAsks.begin(), m_vAsks.end(), 0.0);
		}
	};
}
