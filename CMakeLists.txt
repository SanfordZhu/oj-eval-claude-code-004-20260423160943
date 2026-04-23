cmake_minimum_required(VERSION 3.10)
project(BookstoreManagement)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output name to 'code' as required
set(CMAKE_RUNTIME_OUTPUT_NAME code)

# Add all source files
set(SOURCES
    main.cpp
    account.cpp
    book.cpp
    log.cpp
    command.cpp
    utils.cpp
)

# Add all header files
set(HEADERS
    account.h
    book.h
    log.h
    command.h
    utils.h
)

# Create executable
add_executable(code ${SOURCES} ${HEADERS})

# Add compiler flags for optimization and warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(code PRIVATE -Wall -Wextra -O2)
endif()