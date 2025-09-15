
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(SodiumIodideMC)

find_package(Geant4 REQUIRED)

## Get the ROOT config
find_package(ROOT REQUIRED)
set(INC_ROOT ${ROOT_INCLUDE_DIRS})
set(LIB_ROOT ${ROOT_LIBRARIES})

#include_directories(${Geant4_INCLUDE_DIRS} ${INC_ROOT})

add_definitions(${Geant4_CXX_FLAGS})
set(CMAKE_CXX_FLAGS ${Geant4_CXX_FLAGS})

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(GEANT4_INSTALL_DATA "Data" ON)
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include ${INC_ROOT})
#link_directories($

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cc ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

