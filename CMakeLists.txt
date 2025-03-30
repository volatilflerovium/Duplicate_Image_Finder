cmake_minimum_required(VERSION 3.20)
project("wxWidgets_DuplicateImg")

# --------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------------------------

if(NDEBUG)
	add_definitions(-DNDEBUG=1)
endif() 

if(NOT(CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
	add_definitions(-DDEBUG)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_compile_options("-Wno-comment")
	add_definitions(-DDEBUG_DIR="${CMAKE_SOURCE_DIR}")
endif()

add_compile_options("-O3")

##--------------------------------------------------------------------

find_package(Threads REQUIRED)

##--------------------------------------------------------------------

if(DEFINED "ENV{OPENCV_PATH}")
	set(OpenCV_DIR "$ENV{OPENCV_PATH}/${CMAKE_BUILD_TYPE}/lib/cmake/opencv4")
endif()

find_package(OpenCV 4.0.0 REQUIRED COMPONENTS imgproc highgui core) 

message("OpenCV version : ${OpenCV_VERSION}") 

#--------------------------------------------------------------------- 

set(WXCOMPONENTS "core base webview")

if(DEFINED "ENV{WXWIDGETS_PATH}")
	find_package(wxWidgets REQUIRED 
		PATHS "$ENV{WXWIDGETS_PATH}" NO_DEFAULT_PATH
		COMPONENTS core base webview
	)
else()
	find_package(wxWidgets COMPONENTS core base webview REQUIRED)
	include("${wxWidgets_INCLUDE_DIRS}")
endif()

message("wxWidgets version: ${wxWidgets_VERSION}")

#--------------------------------------------------------------------- 

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
add_library(Debug_lib INTERFACE)

target_include_directories(
	Debug_lib
	INTERFACE
	"${PARENT_DIR}/Libraries/utilities/include"
)
#--------------------------------------------------------------------- 

add_executable(wxDuplicateImgFinder
	src/main.cpp
	src/duplicate_img_gui.cpp
	src/image_panel.cpp
	src/wx_stacked_image.cpp
	src/image_scrolled_panel.cpp
	src/opencv_utilities.cpp
	src/histogram_loader.cpp
	src/worker.cpp
	src/scheduler.cpp
	src/data_logger.cpp
	src/file_manager.cpp
	src/picture_viewer.cpp
   src/wx_worker.cpp
   src/progress_bar.cpp
   src/notification_module.cpp
   src/event_definitions.cpp
   src/extended_popup.cpp
   src/settings_manager.cpp
	src/directory_scrolled_window.cpp
	src/scrolled_window_base.cpp
)

target_include_directories(
	wxDuplicateImgFinder
	PRIVATE
	"include"
)

target_link_libraries(
	wxDuplicateImgFinder 
	PRIVATE
	Threads::Threads
	"${wxWidgets_LIBRARIES}"
	"${OpenCV_LIBS}"
	Debug_lib
)

if("${wxWidgets_VERSION_MAJOR}.${wxWidgets_VERSION_MINOR}.${wxWidgets_VERSION_PATCH}" VERSION_LESS "3.3.0")
	target_compile_definitions(
		wxDuplicateImgFinder 
		PRIVATE
		WX_STRING_ARRAY
	)
endif()

# install resources
add_subdirectory(resources)
