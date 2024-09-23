# PacketParser README

## Purpose of the Code

The purpose of this code is to provide a versatile and robust tool for parsing and interpreting packet data based on a specified JSON format. This tool is designed to handle various types of data fields and bit fields, allowing for flexible and efficient extraction of information from raw data buffers. The primary goal is to enable accurate and customizable data parsing, suitable for a range of applications that involve complex data structures.

## Key Features

### PacketParser Class

- **Overview**: The PacketParser class is central to this implementation. It is designed to read and interpret data packets based on a JSON configuration file that defines the structure and types of fields.
  
- **Constructor**: The constructor of the PacketParser class takes a file path to a JSON configuration and a pointer to a raw data buffer. It initializes the parser with the provided configuration and data.
  
- **Field Access**: The class provides methods to retrieve values for specific fields and to get all field values from the data buffer. This allows users to extract and inspect data from packets in a structured manner.
  
- **Field Types**: It supports a variety of field types, including unsigned integers, signed integers, character arrays, floating-point numbers (fixed and mantissa formats), doubles, and booleans.
  
- **Bit Field Handling**: The class can handle bit fields, which are complex fields that consist of multiple sub-fields. It supports the extraction and decoding of these sub-fields according to their definitions in the JSON configuration.
  
- **Endianness**: The class takes into account the byte order (endianness) specified in the JSON file, which can be either "little" or "big." This ensures that data is interpreted correctly based on the specified endianness.
  
- **Error Handling and Validation**: The PacketParser includes methods for validating field sizes and ensuring that data conforms to the expected formats. This helps prevent errors and ensures that parsed data is accurate.

## JSON Configuration File

- **Purpose**: The JSON configuration file defines the structure and types of fields in the data packets. It specifies details such as field names, types, sizes, and nested bit field structures.
  
- **Structure**: The JSON file includes descriptions and constraints for each field type and provides examples of how fields and bit fields should be defined. This configuration file serves as a blueprint for how the PacketParser interprets the raw data buffer.
  
- **Field Definitions**: The JSON allows for the definition of various field types, including character arrays, floating-point numbers, and bit fields with nested sub-fields. It also specifies the size of each field and provides rules for valid field sizes.

## Testing

- **Objective**: Tests have been created to ensure the functionality and accuracy of the PacketParser class. These tests verify that the class correctly parses and interprets different types of data fields and bit fields.
  
- **Scope**: The tests include cases from the image processing team’s codebase, ensuring that the parser handles real-world data scenarios effectively.

## Summary

In summary, this code introduces a generic and flexible packet parsing tool that leverages JSON configuration for defining data structures. The PacketParser class provides methods to extract and interpret various types of data from raw buffers, handling both simple and complex field types. The included JSON file format allows for detailed configuration of fields and their properties, while the comprehensive tests ensure the robustness and reliability of the parsing logic.

This implementation is valuable for applications that need to process and interpret structured data packets, offering a customizable and efficient solution for data parsing tasks.