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
    // template?
    template <typename ContainerGl> requires std::ranges::contiguous_range<ContainerGl>
    class BinanceBook
    {
        BookPriceQuantity<ContainerGl> m_vAsks;
        BookPriceQuantity<ContainerGl> m_vBids;
    public:

        BinanceBook(){}
        
        // Clear the book
        void clear(void)
        {
            m_vAsks.clear();
            m_vBids.clear();
        }

        // Test whether book is empty.
        bool is_empty(void) const
        {
            return (m_vAsks.getNumEntries() == 0) && (m_vBids.getNumEntries() == 0);
        }

        // Replace entire contents of book with given bids / asks (assumed to be in canonical order).
        // Bonus: accept any suitable container of 
        // TODO: https://stackoverflow.com/questions/7728478/c-template-class-function-with-arbitrary-container-type-how-to-define-it
        template <typename Container>  requires std::ranges::contiguous_range<Container>
        void replace(const Container& vAsks, const Container& vBids)
        {
            m_vAsks.replace(vAsks);
            m_vBids.replace(vBids);
        }

        template <typename Container> requires std::ranges::contiguous_range<Container>
        void replace(const BookDepth<Container>& bookDepth)
        {
            m_vAsks.replace(bookDepth.m_vAsks);
            m_vBids.replace(bookDepth.m_vBids);
        }
        
        // Apply a new best bid / ask.
        void update_bbo(const PriceQuantity& newBid, const PriceQuantity& newAsk)
        {
            m_vBids.template cut<QUANTITY::GREATER>(newBid);
            m_vAsks.template cut<QUANTITY::LESS>(newAsk);
        }

        
        // Retrieve the book (in canonical order).
        // This should output something similar to the input for `replace()`.
        const std::pair<BookPriceQuantity<ContainerGl>&, BookPriceQuantity<ContainerGl>&> extract(void)
        {
            return std::make_pair(std::ref(m_vBids), std::ref(m_vAsks));
        }
        
        // to_string() - convert to string for output.
        // This should be efficient but isn't performance critical.
        const std::string to_string(void) const
        {
            std::stringstream ss;
            size_t maxEntries = std::max(m_vAsks.getNumEntries(), m_vBids.getNumEntries());
            size_t nBids = m_vBids.getNumEntries();
            size_t nAsks = m_vAsks.getNumEntries();
            for (size_t idx = 0; idx < maxEntries; idx++)
            {
                ss << "[" << std::format("{:3}", idx + 1) << "] ";
                if (idx < nBids)
                {
                    ss << "[" << std::format("{: 10.5f}", m_vBids[idx].quantity) << "]";
                    ss << " " << std::format("{:<10.3f}", m_vBids[idx].price) << " | ";
                }
                else
                {
                    ss << "[" << std::format("{:<10}", "-") << "]";
                    ss << "[" << std::format("{:<10}", "-") << "]";
                }
                if (idx < nAsks)
                {
                    ss << " " << std::format("{:10.3f}", m_vAsks[idx].price);
                    ss << "[" << std::format("{:<10.5f}", m_vAsks[idx].quantity) << "]";
                }
                else
                {
                    ss << "[" << std::format("{:<10}", "-") << "]";
                    ss << "[" << std::format("{:<10}", "-") << "]";
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

