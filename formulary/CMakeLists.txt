project(formulary VERSION 0.0.1 LANGUAGES CXX)

file(GLOB incs ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB srcs ${PROJECT_SOURCE_DIR}/src/*.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_library(formulary SHARED ${incs} ${srcs})
target_include_directories(formulary PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${CONAN_INCLUDE_DIRS} PRIVATE ${CMAKE_SOURCE_DIR}/base/include) 
target_link_libraries(formulary PRIVATE sqlite3 base)