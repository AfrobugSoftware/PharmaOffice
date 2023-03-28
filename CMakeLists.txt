cmake_minimum_required(VERSION 3.20)
project(pof VERSION 0.1 LANGUAGES CXX)

#setup complier
set(CMAKE_CXX_STANDARD  20)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


file(GLOB incs ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB srcs ${PROJECT_SOURCE_DIR}/src/*.cpp)

if(${CMAKE_CXX_PLATFORM_ID} STREQUAL "Windows")
    set(APP WIN32)
    add_compile_options(/bigobj)
elseif()
    set(APP "")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/base)
add_subdirectory(${PROJECT_SOURCE_DIR}/formulary)


add_executable(pharmaoffice ${incs} ${srcs})
add_definitions(${CONAN_DEFINES})
target_include_directories(pharmaoffice PUBLIC ${CONAN_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(pharmaoffice PUBLIC ${CONAN_LIBS} base formulary)