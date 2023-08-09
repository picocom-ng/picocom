##
## FEATURE SELECTION
##

# Set this to 0 to disable high baudrate support
FEATURE_HIGH_BAUD ?= 1

# Set this to 0 to disable locking via flock (and use uucp-style
# locks instead)
FEATURE_USE_FLOCK ?= 1

# Set this to 0 to disable linenoise support
FEATURE_LINENOISE ?= 1

# Set this to 0 to disable help strings (saves ~ 4-6 Kb)
FEATURE_HELP ?= 1

VERSION = $(shell git describe --long)
-include version.mk

#CC ?= gcc
CPPFLAGS += -DVERSION_STR=\"$(VERSION)\"
CFLAGS += -Wall -g

LD = $(CC)
LDFLAGS ?= -g
LDLIBS ?=

OBJS = picocom.o term.o fdio.o split.o custbaud.o termios2.o custbaud_bsd.o

## This is the maximum size (in bytes) the output (e.g. copy-paste)
## queue is allowed to grow to. Zero means unlimitted.
TTY_Q_SZ = 0
CPPFLAGS += -DTTY_Q_SZ=$(TTY_Q_SZ)

ifeq ($(FEATURE_HIGH_BAUD),1)
CPPFLAGS += -DHIGH_BAUD
endif

ifeq ($(FEATURE_USE_FLOCK),1)
CPPFLAGS += -DUSE_FLOCK
else
UUCP_LOCK_DIR=/var/lock
CPPFLAGS += -DUUCP_LOCK_DIR=\"$(UUCP_LOCK_DIR)\"
endif

ifeq ($(FEATURE_LINENOISE),1)
HISTFILE = .picocom_history
CPPFLAGS += -DHISTFILE=\"$(HISTFILE)\" \
	    -DLINENOISE
OBJS += linenoise-1.0/linenoise.o
linenoise-1.0/linenoise.o : linenoise-1.0/linenoise.c linenoise-1.0/linenoise.h
endif

## Comment this in to enable (force) custom baudrate support
## even on systems not enabled by default.
#CPPFLAGS += -DUSE_CUSTOM_BAUD

## Comment this in to disable custom baudrate support
## on ALL systems (even on these enabled by default).
#CPPFLAGS += -DNO_CUSTOM_BAUD

ifeq ($(FEATURE_HELP),0)
CPPFLAGS += -DNO_HELP
endif

all: picocom

picocom : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

picocom.o : picocom.c term.h fdio.h split.h custbaud.h
term.o : term.c term.h termios2.h custbaud_bsd.h custbaud.h
split.o : split.c split.h
fdio.o : fdio.c fdio.h
termios2.o : termios2.c termios2.h termbits2.h custbaud.h
custbaud_bsd.o : custbaud_bsd.c custbaud_bsd.h custbaud.h
custbaud.o : custbaud.c custbaud.h

.c.o :
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

doc : picocom.1.html picocom.1 picocom.1.pdf

picocom.1 : picocom.1.md
	pandoc -s -t man \
	    -Vfooter="Picocom $(VERSION)" \
	    -Vadjusting='l' \
	    -Vhyphenate='' \
	    -o $@ $<

picocom.1.html : picocom.1.md
	# modern pandoc wants --embed-resources --standalone, but
	# pandoc in github ci is too old.
	pandoc -s -t html \
	    --standalone \
	    -Vversion="v$(VERSION)" \
	    -o $@ $<

picocom.1.pdf : picocom.1.html
	htmldoc -f $@ $<

clean:
	rm -f $(OBJS)
	rm -f *~
	rm -f \#*\#

distclean: clean
	rm -f picocom

realclean: distclean
	rm -f picocom.1
	rm -f picocom.1.html
	rm -f picocom.1.pdf
	rm -f CHANGES

smoketest: picocom
	bats tests/smoke

