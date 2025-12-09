# Makefile

CC      ?= cc
CFLAGS  ?= -Wall -Wextra -O2 -std=c11

SRCDIR  := src
BUILDDIR:= build
TARGET  := $(BUILDDIR)/assignment-sensor

SRC     := $(SRCDIR)/assignment-sensor.c

PREFIX  ?= /usr/local
BINDIR  := $(PREFIX)/bin
INSTALL ?= install

.PHONY: all clean install uninstall

all: $(TARGET)

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(TARGET): $(SRC) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BUILDDIR)

install: $(TARGET)
	$(INSTALL) -D $(TARGET) $(DESTDIR)$(BINDIR)/assignment-sensor

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/assignment-sensor
