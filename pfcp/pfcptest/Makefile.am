############################################################################
# Copyright (c) 2020 T-Mobile
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

include /usr/local/etc/PcapPlusPlus.mk

#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example #usr#lib
noinst_PROGRAMS = pfcptest

#######################################
# Build information for each library

pfcptest_DEPENDENCIES = \
   $(top_builddir)/modules/libpfcp/lib/libpfcp.so \
   $(top_builddir)/src/libepc.a \
   $(top_builddir)/src/libpfcpr15.a

# Sources for pfcptest
pfcptest_SOURCES =     \
   main.cpp            \
   pcpp.cpp            \
   test.cpp            \
   pcaps/pcaps.cpp     \
   wrapper/wrapper.cpp

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
pfcptest_CPPFLAGS = -Wall -std=c++14 $(PCAPPP_BUILD_FLAGS) $(PCAPPP_INCLUDES) -I$(top_builddir)/pfcp/pfcpex/include -I$(top_builddir)/include/epc -I$(top_builddir)/modules/libpfcp/include
pfcptest_LDFLAGS = -Wl,-rpath='$(top_builddir)/modules/libpfcp/lib' -static-libstdc++ 
pfcptest_LDADD = $(PCAPPP_LIBS_DIR) -L$(top_builddir)/src -L$(top_builddir)/modules/libpfcp/lib -L$(top_builddir)/pfcp/pfcpr15 $(PCAPPP_LIBS) -lpfcpr15 -lepc -lpfcp -lrt -lpthread
