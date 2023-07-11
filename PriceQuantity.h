#pragma once
#include <compare>

namespace Pulsar
{
	using Price = double;
	using Quantity = double;
	constexpr double ROUNDING_ERROR = 1e-15;

	struct PriceQuantity
	{
		Price price{};
		Quantity quantity{};

		auto operator<=>(const PriceQuantity&) const = default;
	};
}

