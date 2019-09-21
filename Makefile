#
# Tiny BASIC Interpreter and Compiler Project
# Makefile
# 
# Released as Public Domain by Damian Gareth Walker 2019
# Created: 18-Aug-2019
#

# Target
TARGET = tinybasic

# Paths and extensions
SRCDIR := src
INCDIR := inc
DOCDIR := doc
BUILDDIR := obj
TARGETDIR := bin
INSTALLDIR := /usr/local
SRCEXT := c
HDREXT := h
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

# Cleanup
clean:
	if [ -e $(BUILDDIR)/*.$(OBJEXT) ]; then rm $(BUILDDIR)/*.$(OBJEXT); fi
	if [ -e $(TARGETDIR)/$(TARGET) ]; then rm $(TARGETDIR)/$(TARGET); fi

# Installation (Unix)
install: $(TARGETDIR)/$(TARGET)
	cp $(TARGETDIR)/$(TARGET) $(INSTALLDIR)/bin
	mkdir -p $(INSTALLDIR)/share/man/man1
	cp $(DOCDIR)/tinybasic.man $(INSTALLDIR)/share/man/man1/tinybasic.1