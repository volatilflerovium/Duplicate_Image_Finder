cmake_minimum_required(VERSION 3.13.4)

# Project's name
project(wxWidgets_DuplicateImg)

set(OpenCV_STATIC ON)
# Requires OpenCV 
FIND_PACKAGE(OpenCV 4.0.0 REQUIRED) 

# Show a message with the opencv version detected 
MESSAGE("OpenCV version : ${OpenCV_VERSION}") 

find_package (Threads)

###################################
###################################
###################################

# Location where cmake first looks for cmake modules.
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

##---------------------------------------------------
## Please set your wxWidgets configuration here
##--------------------------------------------------- 
 
# Here you can define what libraries of wxWidgets you need for your
# application. You can figure out what libraries you need here;
# https://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
# We need the Find package for wxWidgets to work
# NOTE: if you're using aui, include aui in this required components list.

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS core base REQUIRED)
 
##---------------------------------------------------
## Actual config file starts here
##--------------------------------------------------- 
 
# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )


###################################
###################################
###################################


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(release 0)

if(release)
	add_definitions(-DNDEBUG)
endif()

set(SOURCES
	main.cpp
	duplicate_img_gui.cpp
	wx_imagew.cpp
	wx_stacked_image.cpp
	scroller.cpp
	opencv_utilities.cpp
	histogram_loader.cpp
	worker.cpp
	scheduler.cpp
	data_logger.cpp
	file_manager.cpp
	picture_viewer.cpp
   wx_worker.cpp
   progress_bar.cpp
   notification_module.cpp
)

add_executable(wxDuplicateImgFinder ${SOURCES})

# link required libs
target_link_libraries(wxDuplicateImgFinder ${wxWidgets_LIBRARIES})

target_link_libraries(wxDuplicateImgFinder ${OpenCV_LIBS})
target_link_libraries(wxDuplicateImgFinder ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries(wxDuplicateImgFinder stdc++fs )

# install resources
add_subdirectory(resources)
