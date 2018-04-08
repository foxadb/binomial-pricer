/* kernel.cl
 * American Option Binomial Pricing
 * Device code
 */

__kernel void initial(
        const int N,
        const float X0,
        const float K,
        const float d,
        __global float* prices
        ) {
    int i = get_global_id(0);

    // i times u and N - 1 - i times d => N - 1 - 2 * i times d
    float mult = pow(d, N - 1 - 2 * i);

    // Set price value
    prices[i] = fmax(K - X0 * mult, 0);
}

__kernel void pingPong(
        const float X0,
        const float K,
        const float d,
        const float p,
        const float discountFactor,
        const int groupSize,
        __global float* pricesIn,
        __global float* pricesOut,
        const int nodesNb
        ) {
    int startIndex = get_global_id(0) * groupSize;
    int endIndex = min(startIndex + groupSize, nodesNb);

    for (int i = startIndex; i < endIndex; ++i) {
        // Compute multiplier
        float mult = pow(d, nodesNb - 1 - 2 * i);

        // Compute payoff
        float payoff = fmax(K - X0 * mult, 0);

        // Compute price
        float price = fmax(discountFactor
                * (pricesIn[i] * (1 - p) + pricesIn[i + 1] * p),
                payoff);

        // Storage price in output buffer
        pricesOut[i] = price;
    }
}

__kernel void branchClimbing(
        const int N,
        const float X0,
        const float K,
        const float d,
        const float p,
        const float discountFactor,
        __local float* prices,
        __global float* finalPrice
        ) {
    int id = get_local_id(0);

    // Iterate over tree rows
    for (int row = N; row > 0; --row) {
        float mult = pow(d, row - 1 - 2 * id);
        float payoff = fmax(K - X0 * mult, 0);
        float price;

        // Synchronize work-items to avoid memory conflicts
        barrier(CLK_LOCAL_MEM_FENCE);

        // Initialize tree leaves
        if (row == N) {
            price = payoff;
        }
        // Lift the tree by branch
        else if (id < row) {
            price = fmax(discountFactor
                    * (prices[id] * (1 - p) + prices[id + 1] * p),
                    payoff);
        }

        // Synchronize work-items to avoid memory conflicts
        barrier(CLK_LOCAL_MEM_FENCE);

        // Set price value
        prices[id] = price;
    }

    // Write result only one time
    if (id == 0) {
        *finalPrice = prices[0];
    }
}
