# Copyright (c) 2013 Naohisa Sakamoto
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of KVS nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Copyright (c) 2022 Japan Atomic Energy Agency
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

#*****************************************************************************
#  $Id: Makefile.$make 62 2009-03-19 06:58:44Z naohisa.sakamoto $
#*****************************************************************************

#=============================================================================
#  Include.
#=============================================================================
include ../../kvs.conf
include ../../Makefile.def

#=============================================================================
#  INCLUDE_PATH, LIBRARY_PATH, LINK_LIBRARY, INSTALL_DIR.
#=============================================================================
INCLUDE_PATH := -I.. $(VTK_INCLUDE_OPTION)
LIBRARY_PATH := $(VTK_LIBRARY_OPTION)
LINK_LIBRARY := $(VTK_LINK_LIBRARY)
INSTALL_DIR  := $(KVS_DIR)


#=============================================================================
#  Include build rule.
#=============================================================================
include BuildRule.mk


#=============================================================================
#  Project name.
#=============================================================================
TARGET_LIB := $(OUTDIR)/$(LIB_KVS_SUPPORT_EXTENDED_FILE_FORMAT)


#=============================================================================
#  Build rule.
#=============================================================================
$(TARGET_LIB): $(OBJECTS)
	$(AR) $@ $^
	$(RANLIB) $@


#=============================================================================
#  build.
#=============================================================================
build:: $(TARGET_LIB)


#=============================================================================
#  clean.
#=============================================================================
clean::
	$(RMDIR) $(OUTDIR)


#=============================================================================
#  install.
#=============================================================================
install::
	$(MKDIR) $(INSTALL_DIR)/lib
	$(INSTALL) $(TARGET_LIB) $(INSTALL_DIR)/lib
	$(RANLIB) $(INSTALL_DIR)/lib/$(LIB_KVS_SUPPORT_EXTENDED_FILE_FORMAT)

#=============================================================================
#  uninstall.
#=============================================================================
uninstall:
	$(RMDIR) $(INSTALL_DIR)


#=============================================================================
#  reinstall.
#=============================================================================
reinstall: uninstall install
