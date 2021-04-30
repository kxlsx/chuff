# make				> Build executable with debug flags.
# make run			> Build executable with debug flags, then run it.
# make release		> Build executable with release flags.
# make release run	> Build executable with release flags, then run it.
# make clean		> Remove everything in the OUTPUT directory.

# compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Wshadow -Wundef
CFLAGS_DEBUG := -g3
CFLAGS_RELEASE := -O3
CFLAGS_RUNTIME := $(CFLAGS_DEBUG) $(CFLAGS)

# directories
SRC_DIR := src
INCLUDE_DIR := include
OBJECTS_DIR = output$(PATH_SEP)objects
OUTPUT_DIR := output

EXECUTABLE_NAME := huffc

# FIXME: i don't believe it actually works on windows
ifeq ($(OS),Windows_NT)
EXECUTABLE_NAME := $(EXECUTABLE_NAME).exe
PATH_SEP := $(subst /,\,/)
SRC_DIRS := $(SRC_DIR)
RM := del /q /f
RMDIR := rmdir /q /s
MKDIR := mkdir
else
PATH_SEP := /
SRC_DIRS := $(shell find $(SRC_DIR) -type d)
RM := rm -f
RMDIR := rm -drf
MKDIR := mkdir -p
endif

# files
SRCS := $(wildcard $(patsubst %,%$(PATH_SEP)*.c, $(SRC_DIRS)))
INCLUDES := -I$(INCLUDE_DIR)
OBJECTS := $(addprefix $(OBJECTS_DIR)$(PATH_SEP),$(subst $(PATH_SEP),_,$(patsubst $(SRC_DIR)$(PATH_SEP)%.c,%.o,$(SRCS))))
EXECUTABLE := $(OUTPUT_DIR)$(PATH_SEP)$(EXECUTABLE_NAME)

.PHONY: all
all: $(EXECUTABLE)
	@echo Building complete.

.PHONY: release
release: _set_release_flags all
.PHONY: _set_release_flags
_set_release_flags:
	$(eval CFLAGS_RUNTIME :=  $(CFLAGS_RELEASE) $(CFLAGS))

.PHONY: run
run: all
	.$(PATH_SEP)$(EXECUTABLE)
	@echo Executing complete.

.PHONY: clean
clean:
	$(RMDIR) $(OBJECTS_DIR)
	$(RM) $(EXECUTABLE)
	@echo Cleaning complete.

$(OUTPUT_DIR):
	$(MKDIR) $(OUTPUT_DIR)

$(OBJECTS_DIR): $(OUTPUT_DIR)
	$(MKDIR) $(OBJECTS_DIR)

$(OBJECTS): $(OBJECTS_DIR)
	$(CC) $(CFLAGS_RUNTIME) $(INCLUDES) -c \
		$(subst _,$(PATH_SEP),$(patsubst $(OBJECTS_DIR)$(PATH_SEP)%.o,$(SRC_DIR)$(PATH_SEP)%.c,$@)) \
		-o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS_RUNTIME) $(INCLUDES) \
		$(OBJECTS) \
		-o $(EXECUTABLE)
