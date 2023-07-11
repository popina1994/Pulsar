#pragma once

#include <vector>
#include <algorithm>
#include "PriceQuantity.h"

namespace Pulsar
{
	/**
	* @brief Basic book that passes bids and asks to the Binance book.
	*/
	template <typename Container> requires std::ranges::contiguous_range<Container>
	struct BookDepth
	{
		Container m_vBids;
		Container m_vAsks; // or could be e.g. boost::static_vector/small_vector
		

		BookDepth(const Container& vBids, const Container& vAsks)
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
