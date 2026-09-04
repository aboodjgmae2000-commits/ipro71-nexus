cmake_minimum_required(VERSION 3.20)
project(ipro71-nexus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /W4")

# Windows-specific
if(NOT WIN32)
    message(FATAL_ERROR "This project is Windows-only")
endif()

# Find required packages
find_package(OpenCV REQUIRED COMPONENTS core imgproc)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found. Install OpenCV and set OpenCV_DIR.")
endif()

find_package(ONNX REQUIRED)
if(NOT ONNX_FOUND)
    message(FATAL_ERROR "ONNX Runtime not found. Set ONNXRUNTIME_DIR or ONNX_ROOT.")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/capture.cpp
    src/detector.cpp
    src/input.cpp
)

set(HEADERS
    include/capture.h
    include/detector.h
    include/input.h
    include/config.h
    include/math_utils.h
    include/target_selector.h
    include/overlay.h
)

# Executable
add_executable(ipro71-nexus ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(ipro71-nexus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(ipro71-nexus PRIVATE
    ${OpenCV_LIBS}
    onnxruntime
)

# Windows-specific linking
if(WIN32)
    target_link_libraries(ipro71-nexus PRIVATE
        user32
        gdi32
        kernel32
    )
endif()

# Output directory
set_target_properties(ipro71-nexus PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Project: ipro71-nexus")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenCV: ${OpenCV_VERSION}")
message(STATUS "ONNX Runtime: Found")
