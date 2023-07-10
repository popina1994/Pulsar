#include "pch.h"
#include "../BinanceBook.h"
#include "../BookDepth.h"
TEST(BinanceBook, Initial) {
	using namespace Pulsar;
	BinanceBook book;
	EXPECT_EQ(book.is_empty(), true);
	book.clear();
	EXPECT_EQ(book.is_empty(), true);
}

TEST(BinanceBook, Heavy) {
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