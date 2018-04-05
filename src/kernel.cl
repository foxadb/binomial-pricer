/* kernel.cl
 * American Option Binomial Pricing
 * Device code
 */

__kernel void initial(
        const int N,
        const double X0,
        const double K,
        const double d,
        __global double* prices
        ) {
    int i = get_global_id(0);

    // i times u and N - 1 - i times d => N - 1 - 2 * i times d
    double mult = pow(d, N - 1 - 2 * i);

    // Set price value
    prices[i] = fmax(K - X0 * mult, 0);
}

__kernel void linear(
        const double X0,
        const double K,
        const double d,
        const double p,
        const double discountFactor,
        const int groupSize,
        __global double* pricesIn,
        __global double* pricesOut,
        const int nodesNb
        ) {
    int startIndex = get_global_id(0) * groupSize;
    int endIndex = min(startIndex + groupSize, nodesNb);

    for (int i = startIndex; i < endIndex; ++i) {
        // Compute multiplier
        double mult = pow(d, nodesNb - 1 - 2 * i);

        // Compute payoff
        double payoff = fmax(K - X0 * mult, 0);

        // Compute price
        double price = fmax(discountFactor
                * (pricesIn[i] * (1 - p) + pricesIn[i + 1] * p),
                payoff);

        // Storage price in output buffer
        pricesOut[i] = price;
    }
}
