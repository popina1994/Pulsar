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

	/*** 
	* @brief A class that encodes the logic of applying BBOs, and replacing offers and bids. 
	*/
	template <typename Container> requires std::ranges::contiguous_range<Container>
	struct BookPriceQuantity
	{
		static constexpr uint32_t SWITCH_SEARCH = 10;
		static constexpr uint32_t ALLOCATE = 10000;
		Container m_vBook; // or could be e.g. boost::static_vector/small_vector
	public:
		

		BookPriceQuantity() 
		{
			m_vBook.reserve(ALLOCATE);
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

		const Container& getBook(void) const
		{
			return m_vBook;
		}

		const Container getBookCopy(void) const
		{
			Container vOut(m_vBook.size());
			std::reverse_copy(std::begin(m_vBook), std::end(m_vBook), std::begin(vOut));
			return vOut;
		}

		void clear(void)
		{
			m_vBook.clear();
		}

		template <typename Container2> requires std::ranges::contiguous_range<Container2>
		void replace(const Container2& vPriceQuantity)
		{
			m_vBook.resize(vPriceQuantity.size());
			std::reverse_copy(std::begin(vPriceQuantity), std::end(vPriceQuantity), std::begin(m_vBook));
		}

		size_t size(void) const
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

		/**
		* Do we require this operation to have a low latency and canonical order Because if we allow results to be kept in the reverse of canonical order, 
		* we can have quite low latency, otherwise we need to shift entries every time and entries can be more than 20 which can cause high latency. 
		* I implemented the solution that keeps the canonical order in the worst case O(n), while returns in O(1). 
		* There is one more solution that allows keeping the invariant in O(log n) while extracting entries in the canonical order in O(n log n) using heap. 
		* This is dependent on the heap. 
		* This function depending on the number of elements in the book searches for the appropriate position before which elements will be cut. 
		* For large number of elements binary search is prefferd: O(log n) time complexity, 
		* while for smaller number of elements linear search: O(n) potentially. 
		* After that the elements are shifted to the position so the book invariant remains valid. If we allow the book to store the reverse of a canonical order, 
		* we can cut all elements by changing index and comparison logic and adding simple resize() which is O(1).
		*/
		template <QUANTITY L>
		void cut(const PriceQuantity& priceQuantity)
		{
			size_t idxCut;
			bool isCut = true;
			size_t numEntrs = m_vBook.size();
			if (numEntrs >= SWITCH_SEARCH)
			{
				if constexpr (L == QUANTITY::GREATER)
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
				else if constexpr (L == QUANTITY::LESS)
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
			}
			else
			{
				for (idxCut = numEntrs; idxCut > 0; idxCut--)
				{
					if constexpr (L == QUANTITY::GREATER)
					{
						if (m_vBook[idxCut - 1].price < priceQuantity.price)
						{
							isCut = true;
							break;
						}
					}
					else if constexpr (L == QUANTITY::LESS)
					{
						if (m_vBook[idxCut - 1].price > priceQuantity.price)
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
				m_vBook.resize(idxCut + 1);
				m_vBook.back() = priceQuantity;
			}
		}
	};
}
