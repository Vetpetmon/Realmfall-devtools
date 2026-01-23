
cmake_minimum_required(VERSION 3.10)
project(realmfall-devtools VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Place built executables in the repository `output/` folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

# Build cJSON as a static library from the bundled sources
add_library(cjson STATIC
	cjson/cJSON.c
	cjson/cJSON_Utils.c
)

target_include_directories(cjson PUBLIC ${CMAKE_SOURCE_DIR}/cjson)

# Add ranked_builder to build
add_library(ranked_builder STATIC ranked_builder.c)

# ranked_builder depends on cjson; link it so consumers of ranked_builder
# (like the character_maker executable) get the proper link order and symbols.
target_link_libraries(ranked_builder PUBLIC cjson)

# Main executable
add_executable(character_maker character_builder.c)

# Link the ranked_builder library; it already pulls in cjson via PUBLIC linkage above.
target_link_libraries(character_maker PRIVATE ranked_builder)
