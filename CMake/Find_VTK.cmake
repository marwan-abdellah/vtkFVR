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
