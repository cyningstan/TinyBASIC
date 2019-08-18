#
# Tiny BASIC Interpreter and Compiler Project
# Makefile
# 
# Released as Public Domain by Damian Gareth Walker 2019
# Created: 18-Aug-2019
#

# Target
TARGET = tiny

# Paths and extensions
SRCDIR := src
INCDIR := inc
BUILDDIR := obj
TARGETDIR := bin
SRCEXT := c
OBJEXT := o

# Compiler flags
CFLAGS := -Wall
INC := -I$(INCDIR) -I/usr/local/include

# Generate file lists
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Default make
all: $(TARGETDIR)/$(TARGET)

$(TARGETDIR)/$(TARGET): $(OBJECTS)
	gcc -o $(TARGETDIR)/$(TARGET) $(OBJECTS)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	gcc $(CFLAGS) $(INC) -c -o $@ $<
