#include "pch.h"
#include "../BinanceBook.h"
#include "../BookDepth.h"

using Pulsar::ROUNDING_ERROR;
TEST(BinanceBook, Initial) {
	using namespace Pulsar;
	BinanceBook<std::vector<PriceQuantity>> book;
	EXPECT_EQ(book.is_empty(), true);
	book.clear();
	EXPECT_EQ(book.is_empty(), true);
}

TEST(BinanceBook, SimpleInsertion) {
	using namespace Pulsar;
	BinanceBook<std::vector<PriceQuantity>> book;

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
	BinanceBook<std::vector<PriceQuantity>> book;

	std::vector<PriceQuantity> vBids = { {99, 1}, {97, 2}, {95, 3} };
	std::vector<PriceQuantity> vAsks = { {101, 4}, {103, 5}, {105, 6} };

	BookDepth bd(vBids, vAsks);
	EXPECT_EQ(book.is_empty(), true);
	book.replace(bd);
	EXPECT_EQ(book.is_empty(), false);
	const auto& [bids, asks] = book.extract();
	EXPECT_EQ(bids.size(), 3);
	EXPECT_NEAR(bids[0].price, 99, ROUNDING_ERROR);
	EXPECT_NEAR(bids[1].price, 97, ROUNDING_ERROR);
	EXPECT_NEAR(bids[2].price, 95, ROUNDING_ERROR);

	EXPECT_NEAR(bids[0].quantity, 1, ROUNDING_ERROR);
	EXPECT_NEAR(bids[1].quantity, 2, ROUNDING_ERROR);
	EXPECT_NEAR(bids[2].quantity, 3, ROUNDING_ERROR);

	EXPECT_EQ(asks.size(), 3);
	EXPECT_NEAR(asks[0].price, 101, ROUNDING_ERROR);
	EXPECT_NEAR(asks[1].price, 103, ROUNDING_ERROR);
	EXPECT_NEAR(asks[2].price, 105, ROUNDING_ERROR);

	EXPECT_NEAR(asks[0].quantity, 4, ROUNDING_ERROR);
	EXPECT_NEAR(asks[1].quantity, 5, ROUNDING_ERROR);
	EXPECT_NEAR(asks[2].quantity, 6, ROUNDING_ERROR);

	book.update_bbo({ 100, 7 }, { 102, 8 });
	const auto& [bidsAfter, asksAfter] = book.extract();

	EXPECT_EQ(bidsAfter.size(), 4);
	EXPECT_NEAR(bidsAfter[0].price, 100, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[1].price, 99, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[2].price, 97, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[3].price, 95, ROUNDING_ERROR);

	EXPECT_NEAR(bidsAfter[0].quantity, 7, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[1].quantity, 1, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[2].quantity, 2, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter[3].quantity, 3, ROUNDING_ERROR);

	EXPECT_EQ(asksAfter.size(), 3);
	EXPECT_NEAR(asksAfter[0].price, 102, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter[1].price, 103, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter[2].price, 105, ROUNDING_ERROR);

	EXPECT_NEAR(asksAfter[0].quantity, 8, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter[1].quantity, 5, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter[2].quantity, 6, ROUNDING_ERROR);


	book.update_bbo({ 100, 9 }, { 106, 10 });
	const auto& [bidsAfter2, asksAfter2] = book.extract();

	EXPECT_EQ(bidsAfter2.size(), 4);
	EXPECT_NEAR(bidsAfter2[0].price, 100, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter2[1].price, 99, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter2[2].price, 97, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter2[3].price, 95, ROUNDING_ERROR);

	EXPECT_NEAR(bidsAfter2[0].quantity, 9, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter2[1].quantity, 1, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter2[2].quantity, 2, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter2[3].quantity, 3, ROUNDING_ERROR);

	EXPECT_EQ(asksAfter2.size(), 1);
	EXPECT_NEAR(asksAfter2[0].price, 106, ROUNDING_ERROR);

	EXPECT_NEAR(asksAfter2[0].quantity, 10, ROUNDING_ERROR);

	book.update_bbo({ 96, 11}, { 105, 12 });
	const auto& [bidsAfter3, asksAfter3] = book.extract();

	EXPECT_EQ(bidsAfter3.size(), 2);
	EXPECT_NEAR(bidsAfter3[0].price, 96, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter3[1].price, 95, ROUNDING_ERROR);

	EXPECT_NEAR(bidsAfter3[0].quantity, 11, ROUNDING_ERROR);
	EXPECT_NEAR(bidsAfter3[1].quantity, 3, ROUNDING_ERROR);


	EXPECT_EQ(asksAfter3.size(), 2);
	EXPECT_NEAR(asksAfter3[0].price, 105, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter3[1].price, 106, ROUNDING_ERROR);

	EXPECT_NEAR(asksAfter3[0].quantity, 12, ROUNDING_ERROR);
	EXPECT_NEAR(asksAfter3[1].quantity, 10, ROUNDING_ERROR);
}


TEST(BinanceBook, UpdateBboEmpty) {
	using namespace Pulsar;
	BinanceBook<std::vector<PriceQuantity>> book;

	std::vector<PriceQuantity> vBids = { {99, 1}, {97, 2}, {95, 3} };
	std::vector<PriceQuantity> vAsks = { {101, 4}, {103, 5}, {105, 6} };
	constexpr double LIMIT = 100;

	for (double idx = 0; idx < LIMIT / 2; idx++)
	{
		book.update_bbo({idx, idx + 1 }, { LIMIT - idx, LIMIT - idx - 1 });
		const auto& [bidsAfter2, asksAfter2] = book.extract();


		EXPECT_EQ(bidsAfter2.size(), idx + 1);
		for (double idxIn = 0; idxIn < idx; idxIn++)
		{
			uint32_t idxInt = (uint32_t)idxIn;
			EXPECT_NEAR(bidsAfter2[idxInt].price, idx - idxInt, ROUNDING_ERROR);
			EXPECT_NEAR(bidsAfter2[idxInt].quantity, idx - idxInt + 1, ROUNDING_ERROR);
		}

		EXPECT_EQ(asksAfter2.size(), idx + 1);
		for (double idxIn = 0; idxIn < idx; idxIn++)
		{
			uint32_t idxInt = (uint32_t)idxIn;
			EXPECT_NEAR(asksAfter2[idxInt].price, LIMIT + idxInt  - idx, ROUNDING_ERROR);
			EXPECT_NEAR(asksAfter2[idxInt].quantity, LIMIT + idxInt - 1  - idx, ROUNDING_ERROR);
		}
	}
}