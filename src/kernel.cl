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

__kernel void pingPong(
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

__kernel void branchClimb(
        const int N,
        const double X0,
        const double K,
        const double d,
        const double p,
        const double discountFactor,
        __global double* prices,
        __local double* tempPrices
        ) {
    int id = get_global_id(0);

    // Iterate over tree row
    for (int i = N - 1; i >= 0; --i) {
        // Synchronize work-items between each time-step
        barrier(CLK_LOCAL_MEM_FENCE);

        double mult = pow(d, i - 2 * id);
        double payoff = fmax(K - X0 * mult, 0);
        double price;

        // Initialize tree leaves
        if (i == N - 1) {
            price = payoff;
        }
        // Lift the tree by branch
        else if (id <= i) {
            price = fmax(discountFactor
                    * (prices[id] * (1 - p) + prices[id + 1] * p),
                    payoff);
        }

        // Set temp price value
        tempPrices[id] = price;

        // Synchronize work-items between each time-step
        barrier(CLK_LOCAL_MEM_FENCE);
        prices[id] = tempPrices[id];
    }
}
