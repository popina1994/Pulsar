#include "pch.h"
#include "../BinanceBook.h"
#include "../BookDepth.h"

constexpr double ROUNDING_ERROR = 1e-15;
TEST(BinanceBook, Initial) {
	using namespace Pulsar;
	BinanceBook book;
	EXPECT_EQ(book.is_empty(), true);
	book.clear();
	EXPECT_EQ(book.is_empty(), true);
}

TEST(BinanceBook, SimpleInsertion) {
	using namespace Pulsar;
	BinanceBook book;

	std::vector<PriceQuantity> vBids = { {20078.54, 0.00431} };
	std::vector<PriceQuantity> vAsks = { {20078.91000000,0.03437000}};

	BookDepth bd(vBids, vAsks);
	EXPECT_EQ(book.is_empty(), true);
	book.clear();
	EXPECT_EQ(book.is_empty(), true);
	book.replace(bd);
	EXPECT_EQ(book.is_empty(), false);
}

TEST(BinanceBook, UpdateBbo) {
	using namespace Pulsar;
	BinanceBook book;

	std::vector<PriceQuantity> vBids = { {99, 1}, {97, 2}, {95, 3} };
	std::vector<PriceQuantity> vAsks = { {101, 4}, {103, 5}, {105, 6} };

	BookDepth bd(vBids, vAsks);
	EXPECT_EQ(book.is_empty(), true);
	book.replace(bd);
	EXPECT_EQ(book.is_empty(), false);
	const auto& [bids, asks] = book.extract();
	EXPECT_EQ(bids.getNumEntries(), 3);
	EXPECT_NEAR(bids[0].price, 99, ROUNDING_ERROR);
	EXPECT_NEAR(bids[1].price, 97, ROUNDING_ERROR);
	EXPECT_NEAR(bids[2].price, 95, ROUNDING_ERROR);

	EXPECT_NEAR(bids[0].quantity, 1, ROUNDING_ERROR);
	EXPECT_NEAR(bids[1].quantity, 2, ROUNDING_ERROR);
	EXPECT_NEAR(bids[2].quantity, 3, ROUNDING_ERROR);

	EXPECT_EQ(asks.getNumEntries(), 3);
	EXPECT_NEAR(asks[0].price, 101, ROUNDING_ERROR);
	EXPECT_NEAR(asks[1].price, 103, ROUNDING_ERROR);
	EXPECT_NEAR(asks[2].price, 105, ROUNDING_ERROR);

	EXPECT_NEAR(asks[0].quantity, 4, ROUNDING_ERROR);
	EXPECT_NEAR(asks[1].quantity, 5, ROUNDING_ERROR);
	EXPECT_NEAR(asks[2].quantity, 6, ROUNDING_ERROR);

	book.update_bbo({ 100, 7 }, { 102, 8 });
	const auto& [bidsAfter, asksAfter] = book.extract();

	EXPECT_EQ(bidsAfter.getNumEntries(), 4);
	EXPECT_NEAR(bidsAfter[0].price, 100, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[1].price, 99, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[2].price, 97, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[3].price, 95, ROUNDING_ERROR);

	EXPECT_NEAR(bidsAfter[0].quantity, 7, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[1].quantity, 1, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[2].quantity, 2, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[3].quantity, 3, ROUNDING_ERROR);

	EXPECT_EQ(asksAfter.getNumEntries(), 3);
	EXPECT_NEAR(asksAfter[0].price, 102, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter[1].price, 103, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter[2].price, 105, ROUNDING_ERROR);

	EXPECT_NEAR(asksAfter[0].quantity, 8, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter[1].quantity, 5, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter[2].quantity, 6, ROUNDING_ERROR);
}