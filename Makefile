# generic_c_makefile v0.0.1
# Łukasz Dragon <lukasz.b.dragon@gmail.com>
# This is free software. You may redistribute copies of it under the terms of
# the GNU General Public License <https://www.gnu.org/licenses/gpl.html>.
# There is NO WARRANTY, to the extent permitted by law.
#
# Template Makefile for C projects that works on Windows and *nix.
# Although by default it's set to use gcc, you should be able to quickly
# set it up for any C, C++ or similar lang's compiler.
#
# RULES: 
# make             -> Build executable with CFLAGS_DEBUG.
# make run         -> Build executable with CFLAGS_DEBUG, then run it.
# make release     -> Build executable with CFLAGS_RELEASE.
# make release run -> Build executable with CFLAGS_RELEASE, then run it.
# make clean       -> Remove everything in OUTPUT_DIR and OBJ_DIR.
#
# GENERIC BEHAVIOUR:
# Use CC to compile every file with the SRC_SUFFIX in SRC_DIR,
# passing flags:
#     * CFLAGS
#     * CFLAGS_DEBUG or CFLAGS_RELEASE
#     * CFLAG_INCLUDE followed by INCLUDE_DIRS
#     * CFLAG_NOLINK
#     * CFLAG_OUTPUT
# and putting the resulting .OBJ_SUFFIX files into OBJ_DIR.
# Then link the files in OBJ_DIR using CC with flags:
#     * CFLAGS
#     * CFLAGS_DEBUG or CFLAGS_RELEASE
#     * CFLAG_LIBDIR followed by LIB_DIRS
#     * CFLAG_LIB followed by LIBS
#     * CFLAG_OUTPUT
# into a file called EXEC_NAME in OUTPUT_DIR.

# ========= config =========

# compiler and flags
CC     := gcc
CFLAGS := -march=native -Wall -Wextra -Wshadow -Wundef
CFLAGS_DEBUG   := -g3
CFLAGS_RELEASE := -O3

CFLAG_OUTPUT   := -o
CFLAG_NOLINK   := -c
CFLAG_INCLUDE  := -I
CFLAG_LIBDIR   := -L
CFLAG_LIB      := -l

# directories (use normal slashes)
SRC_DIR      := src
OUTPUT_DIR   := output
OBJ_DIR      := $(OUTPUT_DIR)/obj
INCLUDE_DIRS := include
LIB_DIRS     := 
LIBS         := 

# SRC and OBJ file formats
SRC_SUFFIX := .c
OBJ_SUFFIX := .o

EXEC_NAME := huffc

# ========= endconfig =========

CFLAGS_RUNTIME := $(CFLAGS_DEBUG) $(CFLAGS)

ifeq ($(OS),Windows_NT)
EXEC_NAME := $(EXEC_NAME).exe
RM    := del /q /s
MKDIR := mkdir
FIXPATH  = $(subst /,\,$1)
NORMPATH = $(subst \,/,$1)
SUBDIRS  = $(SRC_DIR) $(subst .\,, \
	$(shell PowerShell.exe -NoProfile -NonInteractive \
		"Get-ChildItem -Path $1 -Recurse -Attributes Directory | Resolve-Path -Relative"))
else
RM    := rm -rf
MKDIR := mkdir -p
FIXPATH  = $1
NORMPATH = $1
SUBDIRS  = $(shell find $1 -type d)
endif

# Every SRC is translated into an object of the same name, but placed in OBJ_DIR.
# If SRC is in a subdir, the subdir's name is pushed at the front of name,
# followed by OBJ_SUBDIR_SEP.
# As a result, OBJ_SUBDIR_SEP should not appear in any SRC.
OBJ_SUBDIR_SEP := __
SRC_TO_OBJ = $(call FIXPATH, \
	$(patsubst $(call NORMPATH,$(SRC_DIR))$(OBJ_SUBDIR_SEP)%$(SRC_SUFFIX),$(OBJ_DIR)/%$(OBJ_SUFFIX), \
		$(subst /,$(OBJ_SUBDIR_SEP),$(call NORMPATH,$1))))
OBJ_TO_SRC = $(call FIXPATH, \
	$(patsubst $(call NORMPATH,$(OBJ_DIR))/%$(OBJ_SUFFIX),$(SRC_DIR)/%$(SRC_SUFFIX), \
		$(subst $(OBJ_SUBDIR_SEP),/,$(call NORMPATH,$1))))

SRCS     := $(wildcard $(patsubst %,%/*$(SRC_SUFFIX),$(call SUBDIRS,$(SRC_DIR))))
OBJS     := $(call SRC_TO_OBJ,$(SRCS))
EXEC     := $(OUTPUT_DIR)/$(EXEC_NAME)
INCLUDES := $(patsubst %,$(CFLAG_INCLUDE)%,$(INCLUDE_DIRS))
LIB_DIRS := $(patsubst %,$(CFLAG_LIBDIR)%,$(LIB_DIRS))
LIBS     := $(patsubst %,$(CFLAG_LIB)%,$(LIBS))

# Fix backslashes just in case
SRC_DIR     := $(call FIXPATH,$(SRC_DIR))
OUTPUT_DIR  := $(call FIXPATH,$(OUTPUT_DIR))
OBJ_DIR     := $(call FIXPATH,$(OBJ_DIR))
INCLUDE_DIR := $(call FIXPATH,$(INCLUDE_DIRS))
LIB_DIRS    := $(call FIXPATH,$(LIB_DIRS))
SRCS        := $(call FIXPATH,$(SRCS))
OBJS        := $(call FIXPATH,$(OBJS))
EXEC        := $(call FIXPATH,$(EXEC))
INCLUDES    := $(call FIXPATH,$(INCLUDES))
LIBS        := $(call FIXPATH,$(LIBS))

.PHONY: all release set_release_flags run clean 

all: $(EXEC)
	@echo Building complete.

release: set_release_flags all
set_release_flags:
	$(eval CFLAGS_RUNTIME := $(CFLAGS_RELEASE) $(CFLAGS))

run: all
	$(call FIXPATH,./$(EXEC))
	@echo Executing complete.

clean:
	$(RM) $(OBJ_DIR) $(EXEC)
	@echo Cleaning complete.

# Link OBJS.
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS_RUNTIME) \
	$(LIB_DIRS) $(LIBS) \
		$(OBJS) \
		$(CFLAG_OUTPUT) $(EXEC)

# Compile SRCS without linking.
$(OBJS): $(SRCS) | $(OBJ_DIR)
	$(CC) $(CFLAGS_RUNTIME) $(CFLAG_NOLINK) \
	$(INCLUDES) \
		$(call OBJ_TO_SRC,$@) \
		$(CFLAG_OUTPUT) $@

$(OBJ_DIR): | $(OUTPUT_DIR)
	$(MKDIR) $(OBJ_DIR)

$(OUTPUT_DIR):
	$(MKDIR) $(OUTPUT_DIR)
