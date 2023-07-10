/*

A book for Binance, or any other similar protocol.
We get updates which are up to the maximum 20 levels every 100ms, in canonical order,
ie bids highest->lowest, asks lowest->highest.
Then we get incremental best bid/offer (BBO) updates, so we need to update top of book and uncross.
Terminology: "bid" is an order to buy at that price, "offer" or "ask" is an order to sell.

API docs for reference - note you can assume this data has been fetched and parsed into some appropriate data structures
like std::vector.
https://github.com/binance/binance-spot-api-docs/blob/master/web-socket-streams.md#partial-book-depth-streams
https://github.com/binance/binance-spot-api-docs/blob/master/web-socket-streams.md#individual-symbol-book-ticker-streams

Incoming JSON for reference is:
Full book update:
{"lastUpdateId":34698491742,"bids":[["20078.54000000","0.00431000"],["20078.39000000","0.00100000"],
["20078.27000000","0.00070000"],["20078.21000000","0.00066000"],["20077.91000000","0.03781000"],
["20077.90000000","0.00110000"],["20077.86000000","0.00070000"],["20077.80000000","0.00650000"],
["20077.73000000","0.00055000"],["20077.71000000","0.00100000"],["20077.69000000","0.00984000"],
["20077.66000000","0.00066000"],["20077.61000000","0.04000000"],["20077.60000000","0.02484000"],
["20077.56000000","0.05481000"],["20077.52000000","0.28882000"],["20077.51000000","0.00064000"],
["20077.45000000","0.00070000"],["20077.43000000","0.05181000"],["20077.39000000","0.00689000"]],
"asks":[["20078.91000000","0.03437000"],["20078.95000000","0.00100000"],["20078.99000000","0.00498000"],
["20079.01000000","0.04981000"],["20079.09000000","0.00070000"],["20079.15000000","0.24902000"],
["20079.30000000","0.04110000"],["20079.31000000","0.00066000"],["20079.35000000","0.00864000"],
["20079.42000000","0.00100000"],["20079.44000000","0.09402000"],["20079.46000000","0.09402000"],
["20079.49000000","0.00100000"],["20079.50000000","0.00070000"],["20079.51000000","0.17430000"],
["20079.53000000","0.09602000"],["20079.60000000","0.00100000"],["20079.61000000","0.22853000"],
["20079.62000000","0.08741000"],["20079.66000000","0.00400000"]]}

BBO update:
{"u":34698491814,"s":"BTCUSDT","b":"20078.54000000","B":"0.00431000","a":"20078.91000000","A":"0.03497000"}

Note the "lastUpdateId"/"u" field we ignore here.
Assume the JSON has been parsed and ends up something like:

struct BookDepth
{
    std::vector<PriceQuantity> bids, asks; // or could be e.g. boost::static_vector/small_vector
};

struct BookTicker
{
    Price bestBidPrice{};
    Quantity bestBidQty{};
    Price bestAskPrice{};
    Quantity bestAskQty{};
};

Printing the book would look something like:
[ 1] [ 0.00431] 20078.540 | 20078.910 [0.03497 ]
[ 2] [   0.001] 20078.390 | 20078.950 [0.001   ]
[ 3] [  0.0007] 20078.270 | 20078.990 [0.00498 ]
[ 4] [ 0.00066] 20078.210 | 20079.010 [0.04981 ]
[ 5] [ 0.03781] 20077.910 | 20079.090 [0.0007  ]
[ 6] [  0.0011] 20077.900 | 20079.150 [0.24902 ]
[ 7] [  0.0007] 20077.860 | 20079.300 [0.0411  ]
[ 8] [  0.0065] 20077.800 | 20079.310 [0.00066 ]
[ 9] [ 0.00055] 20077.730 | 20079.350 [0.00864 ]
[10] [   0.001] 20077.710 | 20079.420 [0.001   ]
[11] [ 0.00984] 20077.690 | 20079.440 [0.09402 ]
[12] [ 0.00066] 20077.660 | 20079.460 [0.09402 ]
[13] [    0.04] 20077.610 | 20079.490 [0.001   ]
[14] [ 0.02484] 20077.600 | 20079.500 [0.0007  ]
[15] [ 0.05481] 20077.560 | 20079.510 [0.1743  ]
[16] [ 0.28882] 20077.520 | 20079.530 [0.09602 ]
[17] [ 0.00064] 20077.510 | 20079.600 [0.001   ]
[18] [  0.0007] 20077.450 | 20079.610 [0.22853 ]
[19] [ 0.05181] 20077.430 | 20079.620 [0.08741 ]
[20] [ 0.00689] 20077.390 | 20079.660 [0.004   ]

Price levels in the book are unique.
If you see an update with the same price that should replace current entry.
You may assume there is no issue with floating-point comparisons in this context.
There is no need to sanitize incoming data, you may assume it's well-formed and in range.
The book must be maintained in a valid state, ie the invariant that each price is unique, and each side is on order,
ie top bid is the highest and decreases, top offer is the lowest and increases, and best bid < best offer.

One tricky thing is is that when we get a BookTicker (BBO) update, that only tells us the new top of book. To maintain
the invariant we may have to infer that some entries no longer exist ("uncrossing"),
e.g. if we had prices (ignoring quantities for now):
99  | 101
97  | 103
95  | 105
And we got a BBO update of 100|102, we would have:
100 | 102
99  | 103
97  | 105
95  |  -
So 100 is new best bid, and just others move down. 102 is new best ask, and we have inferred 101 was removed.

You can (and probably should) use >= C++20, however it's not a requirement.
Correctness is the main thing, however book building (except to_string) is on the hot path so efficiency is a concern
also. Comment your code appropriately, and explain any non-trivial logic, particular choices you made any why, etc.
Thread safety is not a requirement.
You may use 3rd party libraries such as Boost if you wish.

*/

#include <iostream>


using Price = double;
using Quantity = double;

struct PriceQuantity
{
    Price price{};
    Quantity quantity{};

    auto operator<=>(const PriceQuantity&) const = default;
};

// template?
class BinanceBook
{
public:
    // Clear the book
    // clear()

    // Test whether book is empty.
    // bool is_empty()

    // Replace entire contents of book with given bids / asks (assumed to be in canonical order).
    // Bonus: accept any suitable container of PriceQuantity
    // replace(bids, asks)

    // Apply a new best bid / ask.
    // update_bbo(new_best_bid, new_best_ask)

    // Retrieve the book (in canonical order).
    // This should output something similar to the input for `replace()`.
    // extract()

    // to_string() - convert to string for output.
    // This should be efficient but isn't performance critical.
};


int main()
{
    // Some tests / display of the above class in action.
    // Hard-code some values for input to the class, extract them back out, assert and/or print the results.
}
