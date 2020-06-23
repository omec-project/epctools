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
bin_PROGRAMS = $(top_builddir)/exampleProgram/pfcp/bin/pfcpex
#__top_builddir__bin_pfcpex_SOURCES


#######################################
# Build information for each library

# Sources for epc
__top_builddir__exampleProgram_pfcp_bin_pfcpex_SOURCE = main.cpp pfcpex.cpp pfcpr15.cpp
#pfcpex_SOURCES = main.cpp pfcpr15.cpp

# Linker options libTestProgram
#__top_builddir__exampleProgram_pfcp_bin_pfcpex_LDFLAGS = -Wl,../../libpfcp/lib/libpfcp.so

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
 #-I../modules/spdlog/include
#pfcpex_CPPFLAGS = -Wall -std=c++11 -I../include -I../../../include/epc
#pfcpex_LDADD = -L../../../src -lepc -lpthread -lrt
__top_builddir__exampleProgram_pfcp_bin_pfcpex_CPPFLAGS = -Wall -std=c++11 -I../include -I../../../include/epc -I../../libpfcp/include -I../../../modules/spdlog/include
__top_builddir__exampleProgram_pfcp_bin_pfcpex_LDADD = -Wl,../../libpfcp/lib/libpfcp.so -L../../../src -lepc -lpthread -lrt
