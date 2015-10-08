.PHONY: clean allc all

SRCDIR := src
BINDIR := bin
 
CC := gcc
 
all: allc

CEXT := c
CSOURCES := $(shell find $(SRCDIR) -type f -name p*.$(CEXT))
COBJECTS := $(patsubst $(SRCDIR)/p%,$(BINDIR)/p%,$(CSOURCES:.$(CEXT)=.exe))
CFLAGS := -g -Wall $(shell pkg-config --cflags gtk+-3.0)
CLIBFLAGS := $(shell pkg-config --libs gtk+-3.0) -rdynamic

allc: $(COBJECTS)
	@true

# *.c
$(BINDIR)/p%.exe: $(SRCDIR)/p%.$(CEXT) $(SRCDIR)/common.h
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $< $(CLIBFLAGS)

clean:
	echo " Cleaning..."; 
	$(RM) -r $(BINDIR)

