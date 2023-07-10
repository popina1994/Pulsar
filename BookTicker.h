#pragma once

#include "PriceQuantity.h"

namespace Pulsar
{
    struct BookTicker
    {
        Price bestBidPrice{};
        Quantity bestBidQty{};
        Price bestAskPrice{};
        Quantity bestAskQty{};
    };
}