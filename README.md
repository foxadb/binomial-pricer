# binomial-pricer

Binomial American Option Pricer with OpenCL

## Dependencies

- C++11
- cmake >= 3.6.3
- gcc >= 7.2

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

```
./src/pricer [ X0 ] [ K ] [ r ] [ sigma ] [ T ] [ N ]
```

Example:

```
./src/pricer 100 100 0.05 0.2 3 5000
```
Should return 8.65 approximately
