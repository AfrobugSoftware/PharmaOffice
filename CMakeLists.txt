cmake_minimum_required(VERSION 3.20)
project(pof VERSION 0.1 LANGUAGES CXX)

#setup complier
set(CMAKE_CXX_STANDARD  20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


file(GLOB incs ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/controls/*.h)
file(GLOB srcs ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/controls/*.cpp)

if(CMAKE_SYSTEM MATCHES Windows)
    set(APP WIN32)
    add_compile_options(/bigobj)
elseif()
    set(APP "")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/base)
add_subdirectory(${PROJECT_SOURCE_DIR}/formulary)


add_executable(pharmaoffice ${APP} ${incs} ${srcs})
add_definitions(${CONAN_DEFINES})
target_include_directories(pharmaoffice PUBLIC ${CONAN_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(pharmaoffice PUBLIC ${CONAN_LIBS} base formulary)