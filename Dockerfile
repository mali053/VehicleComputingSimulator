# Use the Ubuntu 20.04 image as the base
FROM ubuntu:20.04

# Install CMake, Google Test dependencies, and g++
RUN apt-get update && apt-get install -y \
    cmake \
    libgtest-dev \
    g++ \
    make

# Set the working directory inside the container
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . .

# Create a build directory
RUN mkdir build

# Change to the build directory
WORKDIR /app/build

# Install Google Test
RUN cd /usr/src/gtest && cmake CMakeLists.txt && make && \
    find /usr/src/gtest -name "*.a" -exec cp {} /usr/lib \;

# Build the project using CMake and Make
RUN cmake .. -DUSE_SYCL=OFF
RUN make

# Command to run tests (optional)
CMD ["./DPCPPExample"]
