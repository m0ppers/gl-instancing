# - Try to find the Argtable2 library
# Once done this will define
#
#  LIBARGTABLE22_FOUND - system has the Argtable2 library
#  LIBARGTABLE22_LIBS - The libraries needed to use Argtable2

# Copyright (c) 2006, Pino Toscano, <toscano.pino@tiscali.it>
# Copyright (c) 2008, Modestas Vainius, <modestas@vainius.eu>
# Copyright (c) 2010, Riku Salkia, <riksa@iki.fi>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying LICENSE.BSD file.

include(CheckIncludeFiles)

check_include_files(argtable2.h HAVE_ARGTABLE2_H)

if (HAVE_ARGTABLE2_H)
    set(LIBARGTABLE2_HEADERS_FOUND TRUE)
endif (HAVE_ARGTABLE2_H)

if (LIBARGTABLE2_HEADERS_FOUND)
    find_library(LIBARGTABLE2_LIBS NAMES argtable2 )
endif (LIBARGTABLE2_HEADERS_FOUND)

if (LIBARGTABLE2_LIBS)
    set(LIBARGTABLE2_FOUND TRUE)
    message(STATUS "LibArgtable2 found: ${LIBARGTABLE2_LIBS}")
elseif (LibARGTABLE2_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find LibARGTABLE2")
endif (LIBARGTABLE2_LIBS)
