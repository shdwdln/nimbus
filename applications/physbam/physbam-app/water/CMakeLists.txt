# if you want to enforce the compile version, uncomment this. omidm
# SET(CMAKE_C_COMPILER gcc-4.5)
# SET(CMAKE_CXX_COMPILER g++-4.5)

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.7 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
###########################################################################
#Project-specific Configuration
###########################################################################

#Set a project name
SET(PROJECT_NAME Water)
PROJECT(${PROJECT_NAME})

#Manually list your cpp and h files here - do NOT use GLOB/GLOB_RECURSE
SET(${PROJECT_NAME}_cpp
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${PROJECT_SOURCE_DIR}/WATER_DRIVER.cpp
        ${PROJECT_SOURCE_DIR}/WATER_EXAMPLE.cpp)
SET(${PROJECT_NAME}_h
        ${PROJECT_SOURCE_DIR}/WATER_DRIVER.h
        ${PROJECT_SOURCE_DIR}/WATER_EXAMPLE.h)
        
#List the libraries you want to link against - including a higher-level
#library automatically includes the lower-level ones
SET(LIBRARIES_TO_LINK PhysBAM_Rendering-OpenGL)
        
LIST(APPEND LIBRARIES_TO_LINK /usr/lib/x86_64-linux-gnu/librt.so)

#you can force cache variables to be on or off here
#SET(PHYSBAM_USE_CUDA ON CACHE BOOL ON FORCE)
        
INCLUDE(${PROJECT_SOURCE_DIR}/../../physbam-lib/Scripts/CMake/Nimbus_Application.cmake)
