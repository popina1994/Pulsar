#pragma once


#include <vector>
#include <string>
#include "BookTicker.h"
#include "BookDepth.h"
#include <sstream>
#include <format>

namespace Pulsar
{
    // template?
    class BinanceBook
    {
        
        BookDepth m_bookDepth;
        // I did not understand what is the minimal size of books to be held. 
        // In order to reduce the latency I would keep just a specific number of bids/offers (up to 20).
    public:

        BinanceBook(){}
        
        // Clear the book
        void clear(void)
        {
            m_bookDepth.clear();
        }

        // Test whether book is empty.
        bool is_empty(void) const
        {
            return (m_bookDepth.m_nBids == 0) && (m_bookDepth.m_nAsks == 0);
        }

        // Replace entire contents of book with given bids / asks (assumed to be in canonical order).
        // Bonus: accept any suitable container of 
        // TODO: https://stackoverflow.com/questions/7728478/c-template-class-function-with-arbitrary-container-type-how-to-define-it
        void replace(const std::vector<PriceQuantity>& vAsks, const std::vector<PriceQuantity>& vBids)
        {
            m_bookDepth.m_nAsks = vAsks.size();
            uint32_t idx = 0; 
            for (const auto& ask : vAsks)
            {
                m_bookDepth.m_vAsks[idx] = ask;
                idx++;
            }

            m_bookDepth.m_nBids = vBids.size();
            idx = 0;
            for (const auto& bid: vBids)
            {
                m_bookDepth.m_vBids[idx] = bid;
                idx++;
            }
        }

        void replace(const BookDepth& bookDepth)
        {
            m_bookDepth = bookDepth;
        }
        
        // Apply a new best bid / ask.
        
        void update_bbo(const PriceQuantity& newBid, const PriceQuantity& newAsk)
        {
            int32_t idxBid;
            int32_t idxAsk;
            bool cutBids = false;
            bool equalBid = false;
            bool cutAsks = false;
            bool equalAsk = false;
            
            for (idxBid = m_bookDepth.m_nBids; idxBid > 0; idxBid--)
            {
                if (m_bookDepth.m_vBids[idxBid].price > newBid.price)
                {
                    cutBids = true;
                    break;
                }
                if (m_bookDepth.m_vBids[idxBid].price == newBid.price)
                {
                    equalBid = true;
                    m_bookDepth.m_vBids[idxBid].quantity = newBid.quantity;
                    break;
                }
            }
            
            
            if (cutBids)
            {
                for (int32_t idx = m_bookDepth.m_nBids - 1; idx >= idxBid; idx--)
                {
                    m_bookDepth.m_vBids[idx - idxBid + 1] = m_bookDepth.m_vBids[idx];
                }
                m_bookDepth.m_vBids[idxBid] = newBid;
            }
            

            
            for (idxAsk = m_bookDepth.m_nAsks; idxAsk > 0; idxAsk--)
            {
                if (m_bookDepth.m_vAsks[idxAsk].price < newAsk.price)
                {
                    cutAsks = true;
                    break;
                }
                if (m_bookDepth.m_vAsks[idxAsk].price == newAsk.price)
                {
                    equalBid = true;
                    m_bookDepth.m_vAsks[idxAsk].quantity = newAsk.quantity;
                    break;
                }
            }
            if (cutAsks)
            {
                for (int32_t idx = m_bookDepth.m_nAsks - 1; idx >= idxAsk; idx--)
                {
                    m_bookDepth.m_vAsks[idx - idxAsk + 1] = m_bookDepth.m_vAsks[idx];
                }
                m_bookDepth.m_vAsks[idxAsk] = newAsk;
            }
            
        }

        
        // Retrieve the book (in canonical order).
        // This should output something similar to the input for `replace()`.
        
        const BookDepth& extract(void)
        {
            return m_bookDepth;
        }
        

        
        // to_string() - convert to string for output.
        // This should be efficient but isn't performance critical.
        const std::string to_string(void) const
        {
            std::stringstream ss;
            uint32_t maxEntries = std::max(m_bookDepth.m_nAsks, m_bookDepth.m_nBids);
            for (int idx = 0; idx < maxEntries; idx++)
            {
                std::format("{:<6}", 42);
                ss << "[" << std::format("{:<2}", idx + 1) << "] ";
                ss << "[" << std::format("{:<7}", m_bookDepth.m_vBids[idx].quantity) << "]";
                ss << " " << std::format("{:<7}", m_bookDepth.m_vBids[idx].price) << " | ";
                ss << " " << std::format("{:<7}", m_bookDepth.m_vAsks[idx].price);
                ss << "[" << std::format("{:<7}", m_bookDepth.m_vAsks[idx].quantity) << "]";
                ss << std::endl;
                return ss.str();
            }
        }
        
    };
}

