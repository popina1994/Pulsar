#pragma once

#include <vector>
#include <algorithm>
#include <type_traits>
#include <concepts>
#include <iterator>
#include "PriceQuantity.h"

namespace Pulsar
{	
	enum class QUANTITY
	{
		GREATER = 0,
		LESS = 1
	};

	template <typename Container> requires std::ranges::contiguous_range<Container>
	struct BookPriceQuantity
	{
		static constexpr uint32_t NUM_BIDS_TO_KEEP = 21;
		Container m_vBook; // or could be e.g. boost::static_vector/small_vector
	public:
		

		BookPriceQuantity() 
		{
			m_vBook.reserve(NUM_BIDS_TO_KEEP);
		}

		BookPriceQuantity(Container& vPriceQuant) :
			BookPriceQuantity()
		{
			uint32_t idx = 0;
			for (const auto& priceQuant : vPriceQuant)
			{
				m_vBook[idx] = priceQuant;
				idx++;
			}
		}

		void clear(void)
		{
			m_vBook.clear();
		}

		template <typename Container2> requires std::ranges::contiguous_range<Container2>
		void replace(const Container2& vPriceQuantity)
		{
			m_vBook.assign(vPriceQuantity.begin(), vPriceQuantity.end());
		}

		size_t getNumEntries(void) const
		{
			return m_vBook.size();
		}


		template <typename T> requires std::integral<T>
		PriceQuantity& operator[](T idx)
		{
			return m_vBook[idx];
		}

		template <typename T> requires std::integral<T>
		const PriceQuantity& operator[](T idx) const
		{
			return m_vBook[idx];
		}

		template <QUANTITY L>
		void cut(const PriceQuantity& priceQuantity)
		{
			size_t idxCut;
			bool isCut = true;
			size_t numEntrs = m_vBook.size();
			if (numEntrs >= NUM_BIDS_TO_KEEP)
			{
				if constexpr (L == QUANTITY::GREATER)
				{
					auto priceIt = std::lower_bound(
						m_vBook.begin(), m_vBook.end(),
						priceQuantity,
						[](const PriceQuantity& info, const PriceQuantity& priceQuantityIn)
						{
							return info.price > priceQuantityIn.price;
						});
					idxCut = priceIt - m_vBook.begin();
					if ((idxCut < numEntrs) && (m_vBook[idxCut].price == priceQuantity.price))
					{
						isCut = false;
						m_vBook[idxCut].quantity = priceQuantity.quantity;
					}
				}
				else if constexpr (L == QUANTITY::LESS)
				{
					auto priceIt = std::lower_bound(
						m_vBook.begin(), m_vBook.end(),
						priceQuantity,
						[](const PriceQuantity& info, const PriceQuantity& priceQuantityIn)
						{
							return info.price < priceQuantityIn.price;
						});
					idxCut = priceIt - m_vBook.begin();
					if ((idxCut < numEntrs) && (m_vBook[idxCut].price == priceQuantity.price))
					{
						isCut = false;
						m_vBook[idxCut].quantity = priceQuantity.quantity;
					}
				}
			}
			else
			{
				for (idxCut = numEntrs; idxCut > 0; idxCut--)
				{
					if constexpr (L == QUANTITY::GREATER)
					{
						if (m_vBook[idxCut - 1].price > priceQuantity.price)
						{
							isCut = true;
							break;
						}
					}
					else if constexpr (L == QUANTITY::LESS)
					{
						if (m_vBook[idxCut - 1].price < priceQuantity.price)
						{
							isCut = true;
							break;
						}
					}

					if (m_vBook[idxCut - 1].price == priceQuantity.price)
					{
						isCut = false;
						m_vBook[idxCut - 1].quantity = priceQuantity.quantity;
						break;
					}
				}
			}
			
			if (isCut)
			{
				if (idxCut == 0)
				{
					m_vBook.push_back({ 0.0, 0.0 });
					for (int32_t idx = (int32_t)numEntrs - 1; idx >= (int32_t)idxCut; idx--)
					{
						m_vBook[idx - idxCut + 1] = m_vBook[idx];
					}
				}
				else
				{
					//101 | 103 | 105
					//0    1      2    3
					//101 | 103 | 105 | 106
					for (int32_t idx = (int32_t)idxCut; idx < (int32_t)numEntrs; idx++)
					{
						m_vBook[idx - idxCut + 1] = m_vBook[idx];
					}
					m_vBook.resize(numEntrs - idxCut + 1); 
				}
				m_vBook[0] = priceQuantity;
			}
		}
	};
}
