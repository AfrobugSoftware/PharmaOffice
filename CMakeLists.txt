cmake_minimum_required(VERSION 3.20)
project(pof VERSION 0.1 LANGUAGES CXX)

#setup complier
set(CMAKE_CXX_STANDARD  20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(wxBUILD_SHARED ON)
set(wxUSE_GUI 1)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/wxWidgets-3.2.2.1)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/OpenXLSX)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/wxFreeChart)

add_definitions(-DWXUSINGDLL=1) 
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
add_definitions(${CONAN_DEFINES})

file(GLOB incs ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/controls/*.h)
file(GLOB srcs ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/controls/*.cpp)

if(CMAKE_SYSTEM MATCHES Windows)
    set(APP WIN32)
    add_compile_options(/bigobj)
    add_compile_options(/Zc:__cplusplus /JMC-)
elseif()
    set(APP "")
endif()

#windows application
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/base)
add_subdirectory(${PROJECT_SOURCE_DIR}/formulary)

set(wxWidgets_include_dir ${wxWidgets_root_dir}/include)
set(wxWidgets_libraries_dir ${wxWidgets_root_dir}/lib/vc_x64_dll)
if(CMAKE_SYSTEM MATCHES Windows)
	set(wxWidgets_setup_dir ${wxWidgets_libraries_dir}/mswu)
endif()

add_executable(pharmaoffice ${APP} ${incs} $<$<BOOL:${WIN32}>:pharmaoffice.rc> ${srcs} ${PROJECT_SOURCE_DIR}/main.manifest)

##set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB)
target_precompile_headers(pharmaoffice PUBLIC ${PROJECT_SOURCE_DIR}/include/PofPch.h)
target_include_directories(pharmaoffice PUBLIC ${CONAN_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
target_link_directories(pharmaoffice PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/wxWidgets-3.2.2.1/lib/vc_x64_dll)
target_link_libraries(pharmaoffice PUBLIC wxFreeCharts base formulary OpenXLSX wx::aui wx::html wx::propgrid wx::qa wx::net wx::core wx::base ${CONAN_LIBS})