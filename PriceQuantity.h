#pragma once
#include <compare>

namespace Pulsar
{
	using Price = double;
	using Quantity = double;

	struct PriceQuantity
	{
		Price price{};
		Quantity quantity{};

		auto operator<=>(const PriceQuantity&) const = default;
	};
}

