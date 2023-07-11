#pragma once

#include <vector>
#include <algorithm>
#include <type_traits>
#include <concepts>
#include "PriceQuantity.h"

namespace Pulsar
{	
	/**
	* @brief L denotes the direction in which the largest 
	*/
	struct BookPriceQuantity
	{
		static constexpr uint32_t NUM_BIDS_TO_KEEP = 21;

		std::vector<PriceQuantity> m_vBook; // or could be e.g. boost::static_vector/small_vector
		size_t m_nEntrs = 0;
	public:
		enum class QUANTITY
		{
			GREATER = 0, 
			LESS = 1
		};

		BookPriceQuantity() : m_vBook(NUM_BIDS_TO_KEEP) {}

		BookPriceQuantity(const std::vector<PriceQuantity>& vPriceQuant) :
			BookPriceQuantity()
		{
			uint32_t idx = 0;
			for (const auto& priceQuant : vPriceQuant)
			{
				m_vBook[idx] = priceQuant;
				idx++;
			}
			m_nEntrs = vPriceQuant.size();
		}

		void clear(void) {
			m_nEntrs = 0;
			std::fill(m_vBook.begin(), m_vBook.end(), PriceQuantity{ 0.0, 0.0 });
		}

		void replace(const std::vector<PriceQuantity>& vPriceQuantity)
		{
			size_t idx = 0;
			for (const auto& priceQuant : vPriceQuantity)
			{
				m_vBook[idx] = priceQuant;
				idx++;
			}
			m_nEntrs = vPriceQuantity.size();
		}

		size_t getNumEntries(void) const
		{
			return m_nEntrs;
		}

		void setNumEntries(size_t numEntries)
		{
			m_nEntrs = numEntries;
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
			
			for (idxCut = m_nEntrs; idxCut > 0; idxCut--)
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

			if (isCut)
			{
				if (idxCut == 0)
				{
					if (m_nEntrs == m_vBook.size())
					{
						m_vBook.push_back({ 0.0, 0.0 });
					}
					for (int32_t idx = (int32_t)m_nEntrs - 1; idx >= (int32_t)idxCut; idx--)
					{
						m_vBook[idx - idxCut + 1] = m_vBook[idx];
					}
					m_nEntrs++;
				}
				else
				{
					//101 | 103 | 105
					//0    1      2    3
					//101 | 103 | 105 | 106
					for (int32_t idx = (int32_t)idxCut; idx < (int32_t)m_nEntrs; idx++)
					{
						m_vBook[idx - idxCut + 1] = m_vBook[idx];
					}
					m_nEntrs = m_nEntrs - idxCut + 1; 
				}
				m_vBook[0] = priceQuantity;
			}
		}
	};
}
