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

# FFTW root
MARK_AS_ADVANCED(FFTW_ROOT)

# Where to find the libraries
SET(LIB_SEARCH_PATHS	/usr/lib
                        /usr/local/lib
                        /opt/local/lib
)

# Where to find the libraries
SET(INC_SEARCH_PATH     /usr/include
                        /usr/local/include
                        /opt/local/include
)

# Required VTK shared object libraries
SET(FFTW_REQUIRED_LIBS	fftw3f
                        fftw3
)


# Interface
FIND_PATH(FFTW_INCLUDE_DIR "fftw3.h"
    HINTS ${FFTW_ROOT}/include
    PATH ${INC_SEARCH_PATH}
)

# Found FFTW libraries
SET(FFTW_FOUND_LIBS "")

# Find all the required VTK libraries
FOREACH(FFTW_LIB ${FFTW_REQUIRED_LIBS})
    FIND_LIBRARY(FOUND_${FFTW_LIB} NAMES ${FFTW_LIB}
                    HINTS ${FFTW_ROOT}/lib
                    PATHS ${LIB_SEARCH_PATHS})
    LIST(APPEND FFTW_FOUND_LIBS ${FOUND_${FFTW_LIB}})
ENDFOREACH(FFTW_LIB)


# FindPackageHandleStandardArgs
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW DEFAULT_MSG
        FFTW_FOUND_LIBS
        FFTW_INCLUDE_DIR)

# Linking all the found libraries
LINK_LIBRARIES(${FFTW_FOUND_LIBS})
