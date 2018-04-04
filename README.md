# binomial-pricer

Binomial American Option Pricer with OpenCL

## Dependencies

- C++11
- cmake
- gcc

### PNL

This project requires [PNL](https://github.com/pnlnum/pnl)
You have to clone and build this library.

Then set the `PNL_DIR` in the root `CMakeLists.txt`
This directory is the PNL build directory containing the lib file.

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

*For memory allocation safety, do not use N value higher than 10000 !*

You can run the GPU Pricer (using OpenCL) with:
```
./gpu-pricer [ X0 ] [ K ] [ r ] [ sigma ] [ T ] [ N ]
```

Do not forget to enable your graphic card. For laptop, use tools like `optirun`:
```
optirun ./gpu-pricer [ X0 ] [ K ] [ r ] [ sigma ] [ T ] [ N ]
```
