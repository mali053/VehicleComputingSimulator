# Use an official Ubuntu as a parent image
FROM ubuntu:20.04

# Set environment variables to avoid user input prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && \
    apt-get install -y cmake g++ git wget libgtest-dev\
    build-essential \
    cmake \
    git \
    libgmp-dev \
    libgmpxx4ldbl \
    wget

# Install Google Test
RUN cd /usr/src/gtest && \
    cmake . && \
    make && \
    cp lib/*.a /usr/lib

# Create and set the working directory
WORKDIR /usr/src/myapp

# Copy the current directory contents into the container
COPY . .

# Ensure the build directory exists and clean any previous cache
RUN mkdir -p build && \
    rm -rf build/*

# Run CMake and build the project
RUN cd build && \
    cmake .. && \
    make

# Run the tests
CMD ["./build/runTests"]
