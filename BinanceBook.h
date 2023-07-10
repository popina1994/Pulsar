#pragma once

#include "BookTicker.h"

#include <vector>
#include <string>

namespace Pulsar
{
    // template?
    class BinanceBook
    {
    public:
        // Clear the book
        void clear(void)
        {

        }

        // Test whether book is empty.
        bool is_empty(void) const
        {
            return false;
        }

        // Replace entire contents of book with given bids / asks (assumed to be in canonical order).
        // Bonus: accept any suitable container of PriceQuantity
        void replace(const std::vector<BookTicker>& vBookTicker)
        {

        }

        // Apply a new best bid / ask.
        void update_bbo(const PriceQuantity& newBid, const PriceQuantity& newAsk)
        {

        }

        // Retrieve the book (in canonical order).
        // This should output something similar to the input for `replace()`.
        std::vector<BookTicker> extract(void)
        {
            return {};
        }


        // to_string() - convert to string for output.
        // This should be efficient but isn't performance critical.
        const std::string to_string(void) const
        {
            return "";
        }
    };
}

