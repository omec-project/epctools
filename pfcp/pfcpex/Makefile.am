############################################################################
# Copyright (c) 2009-2019 Brian Waters
# Copyright (c) 2019 Sprint
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
############################################################################

#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example #usr#lib
#bin_PROGRAMS = bin/pfcpex
noinst_PROGRAMS = bin/pfcpex

#######################################
# Build information for each library

# Sources for epc
bin_pfcpex_DEPENDENCIES = \
   $(top_builddir)/modules/libpfcp/lib/libpfcp.so \
   $(top_builddir)/src/libepc.a \
   $(top_builddir)/src/libpfcpr15.a

bin_pfcpex_SOURCES = \
   src/main.cpp \
   src/pfcpex.cpp

# Linker options libTestProgram
bin_pfcpex_LDFLAGS = -Wl,-rpath,../../modules/libpfcp/lib

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
bin_pfcpex_CPPFLAGS = -Wall -std=c++14 -I./include -I../../include/epc -I../../modules/spdlog/include -I../../modules/libpfcp/include
bin_pfcpex_LDADD = -L../../modules/libpfcp/lib -L../../src -lpfcpr15 -lpfcp -lepc -lpthread -lrt
