#####################################################################
# Copyright © 2011-2012,
# Marwan Abdellah: <abdellah.marwan@gmail.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
#####################################################################

# VTK Finder
FIND_PACKAGE(VTK REQUIRED)

# Includes
INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})

# Where to find the libraries
SET(SEARCH_PATHS	/usr/lib
                        /usr/local/lib
                        /opt/local/lib
)

# Required VTK shared object libraries
SET(VTK_REQUIRED_LIBS	vtkCharts
                        vtkChartsPythonD
                        vtkChartsTCL
                        vtkCommon
                        vtkCommonPythonD
                        vtkCommonTCL
                        vtkDICOMParser
                        vtkFiltering
                        vtkFilteringPythonD
                        vtkFilteringTCL
                        vtkGenericFiltering
                        vtkGenericFilteringPythonD
                        vtkGenericFilteringTCL
                        vtkGeovis
                        vtkGeovisPythonD
                        vtkGeovisTCL
                        vtkGraphics
                        vtkGraphicsPythonD
                        vtkGraphicsTCL
                        vtkHybrid
                        vtkHybridPythonD
                        vtkHybridTCL
                        vtkIO
                        vtkIOPythonD
                        vtkIOTCL
                        vtkImaging
                        vtkImagingPythonD
                        vtkImagingTCL
                        vtkInfovis
                        vtkInfovisPythonD
                        vtkInfovisTCL
                        vtkParallel
                        vtkParallelPythonD
                        vtkParallelTCL
                        vtkPythonCore
                        vtkRendering
                        vtkRenderingPythonD
                        vtkRenderingPythonTkWidgets
                        vtkRenderingTCL
                        vtkViews
                        vtkViewsPythonD
                        vtkViewsTCL
                        vtkVolumeRendering
                        vtkVolumeRenderingPythonD
                        vtkVolumeRenderingTCL
                        vtkWidgets
                        vtkWidgetsPythonD
                        vtkWidgetsTCL
                        vtkalglib
                        vtkexoIIc
                        vtkftgl
                        vtkgdcm
                        vtkmetaio
                        vtkproj4
                        vtksys
                        vtkverdict
)

# Found VTK libraries
SET(VTK_FOUND_LIBS "")

# Find all the required VTK libraries
FOREACH(VTK_LIB ${VTK_REQUIRED_LIBS})
    FIND_LIBRARY(FOUND_${VTK_LIB} NAMES ${VTK_LIB}
                    HINTS ${VTK_LIBRARY_DIRS}
                    PATHS ${SEARCH_PATHS})
    LIST(APPEND VTK_FOUND_LIBS ${FOUND_${VTK_LIB}})
ENDFOREACH(VTK_LIB)

# FindPackageHandleStandardArgs
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VTK DEFAULT_MSG
	VTK_FOUND_LIBS
	VTK_INCLUDE_DIRS
)

# Linking all the found libraries
LINK_LIBRARIES(${VTK_FOUND_LIBS})
