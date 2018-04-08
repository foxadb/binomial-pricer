# binomial-pricer

Binomial American Option Pricer with OpenCL

## Dependencies

- c++11
- cmake
- gcc
- opencl

## Build

```
mkdir build
cd build
cmake ..
make
```

## Run

Run the CPU Pricer with:
```
./cpu-pricer [ X0 ] [ K ] [ r ] [ sigma ] [ T ] [ N ]
```

Example:

```
./cpu-pricer 100 100 0.05 0.2 3 5000
```
Should return 8.71 approximately

You can run the GPU Pricer (using OpenCL) with:
```
./gpu-pricer [ X0 ] [ K ] [ r ] [ sigma ] [ T ] [ N ] [ work group size ]
```

Try with small value of work group size at first (between 1 and 10) to find the optimal one.

Do not forget to enable your graphic card. For laptop, use tools like `optirun`:
```
optirun ./gpu-pricer [ X0 ] [ K ] [ r ] [ sigma ] [ T ] [ N ] [ work group size ]
```

Example:

```
optirun ./gpu-pricer 100 100 0.05 0.2 3 1000 1
optirun ./gpu-pricer 100 100 0.05 0.2 3 5000 5
optirun ./gpu-pricer 100 100 0.05 0.2 3 10000 10
```
