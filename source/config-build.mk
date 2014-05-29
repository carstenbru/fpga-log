#_______________________________________________________________________________
#
# Copyright (c) 2012 TU Dresden, Chair for Embedded Systems
# (http://www.mr.inf.tu-dresden.de) All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement: "This product includes
#    software developed by the TU Dresden Chair for Embedded Systems and
#    its contributors."
#
# 4. Neither the name of the TU Dresden Chair for Embedded Systems nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY TU DRESDEN CHAIR FOR EMBEDDED SYSTEMS AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#_______________________________________________________________________________
#

###
# build settings
# NOTE: all paths are relative to firmware home path

# source directory
SOURCE_DIR:=src

# list of include directories
INCLUDE_DIRS:=include

# main source file (including path)
MAIN:=src/main.c

# list of objects to build from 
# corresponding source (src/*.c)
OBJECTS:=*

# spartanmc library objects
# (not recompiled for each project)
LIB_OBJ_FILES:=startup peri

# spartanmc assembler library objects
# (recompiled for each project)
LIB_AS_FILES:=

# compiler flags
CFLAGS:=-O2 -fomit-frame-pointer -fno-inline -spmc-force-cc-branches=yes

# list of defines
DEFINES:=PERI_CLOCK=16000000

# linker script, leave empty for default
LINKER_SCRIPT:=

# symbols passed to linker via --defsym
# space-separated list of NAME=VALUE pairs
SYMBOLS:=

# default upload port for bootloader
DEFAULT_UPLOAD_PORT:=
