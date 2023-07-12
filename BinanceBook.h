#pragma once


#include <vector>
#include <string>
#include <ranges>
#include <iterator>
#include "BookTicker.h"
#include "BookPriceQuantity.h"
#include "BookDepth.h"
#include <sstream>
#include <format>

namespace Pulsar
{
    /**
    * @brief Class that allows adding bids and asks, applying BBO updates, clearing existing book, 
    * and printing the book to the output.
    */
    template <typename ContainerGl> requires std::ranges::contiguous_range<ContainerGl>
    class BinanceBook
    {
        BookPriceQuantity<ContainerGl> m_vAsks;
        BookPriceQuantity<ContainerGl> m_vBids;
    public:

        BinanceBook(){}
        
        /**
        * @brief Clearing existing bids and asks.
        */
        void clear(void)
        {
            m_vAsks.clear();
            m_vBids.clear();
        }

        /**
        * @brief Return true if the book is empthy, otherwise false.
        */
        bool is_empty(void) const
        {
            return (m_vAsks.size() == 0) && (m_vBids.size() == 0);
        }

        /**
        * @brief Replaces the entire contents of book with given bids and asks(assumed to be in canonical order).
        * @note Bonus: accept any suitable container of 
        * https://stackoverflow.com/questions/7728478/c-template-class-function-with-arbitrary-container-type-how-to-define-it
        */ 
        template <typename Container>  requires std::ranges::contiguous_range<Container>
        void replace(const Container& vBids, const Container& vAsks)
        {
            m_vAsks.replace(vAsks);
            m_vBids.replace(vBids);
        }

        /**
        * @brief Replaces the entire contents of book with given bids and asks(assumed to be in canonical order).
        */
        template <typename Container> requires std::ranges::contiguous_range<Container>
        void replace(const BookDepth<Container>& bookDepth)
        {
            m_vBids.replace(bookDepth.m_vBids);
            m_vAsks.replace(bookDepth.m_vAsks);
        }
        
        /**
        * @brief Apply a new best bid / ask.
        */ 
        void update_bbo(const PriceQuantity& newBid, const PriceQuantity& newAsk)
        {
            m_vAsks.template cut<QUANTITY::LESS>(newAsk);
            m_vBids.template cut<QUANTITY::GREATER>(newBid);
        }

        
        /** 
        * @brief Retrieve the book(in canonical order). This should output something similar to the input for `replace()`.
        */ 
        // 
        const std::pair<const std::vector<PriceQuantity>, const std::vector<PriceQuantity>> extract(void)
        {
            std::vector<PriceQuantity> vAsks;
            std::vector<PriceQuantity> vBids;
            
            vBids = m_vBids.getBookCopy();
            vAsks = m_vAsks.getBookCopy();

            return std::make_pair(std::move(vBids), std::move(vAsks));
        }
        
        /**
        * @brief convert to string for output.
        */ 
        const std::string to_string(void) const
        {
            std::stringstream ss;
            size_t nBids = m_vBids.size();
            size_t nAsks = m_vAsks.size();
            size_t maxEntries = std::max(nBids, nAsks);
            for (size_t idx = 0; idx < maxEntries; idx++)
            {
                ss << "[" << std::format("{:3}", idx + 1) << "] ";
                if (idx < nBids)
                {
                    ss << "[" << std::format("{: 10.5f}", m_vBids[nBids - idx - 1].quantity) << "]";
                    ss << " " << std::format("{:<10.3f}", m_vBids[nBids - idx - 1].price) << " | ";
                }
                else
                {
                    ss << "[" << std::format("{:<10}", "----------") << "]";
                    ss << " " << std::format("{:<10}", "----------") << "]";
                }
                if (idx < nAsks)
                {
                    ss << " " << std::format("{:10.3f}", m_vAsks[nAsks - idx - 1].price);
                    ss << "[" << std::format("{:<10.5f}", m_vAsks[nAsks - idx - 1].quantity) << "]";
                }
                else
                {
                    ss << " " << std::format("{:<10}", "----------") << "";
                    ss << "[" << std::format("{:<10}", "----------") << "]";
                }
                ss << std::endl;
            }
            
            return ss.str();
        }

        friend std::ostream& operator<<(std::ostream& out, const BinanceBook& book)
        {
            out << book.to_string();
            return out;
        }
        
    };
}

