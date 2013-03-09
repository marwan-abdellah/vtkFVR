#########################################################
#! Minimum required CMake version
#########################################################
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

#########################################################
#! Project name
#########################################################
PROJECT (vtkFVR)

#########################################################
#! Add CMake customized modules
#########################################################
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

#########################################################
#! Find packages
#########################################################
# @VTK
INCLUDE(Find_VTK)

# @FFTW
INCLUDE(Find_FFTW)

#########################################################
#! Compiler Flags
#########################################################
set(CMAKE_C_FLAGS " -Wno-deprecated -fno-gnu-keywords -w")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")


#########################################################
#! # Executable
#########################################################
ADD_EXECUTABLE(vtkFVR Src/vtkFVR.cxx)
ADD_EXECUTABLE(vtkRayCaster Src/vtkRayCaster.cxx)
ADD_EXECUTABLE(rawToVTK Src/rawToVTK.cxx)

