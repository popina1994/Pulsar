#pragma once
#include <compare>

namespace Pulsar
{
	using Price = double;
	using Quantity = double;
	constexpr double ROUNDING_ERROR = 1e-15;

	/**
	* @brief Basic wrapper that stores pair of a price and quantity.
	*/
	struct PriceQuantity
	{
		Price price{};
		Quantity quantity{};

		auto operator<=>(const PriceQuantity&) const = default;
	};
}

