#
INSTALL := install
PACKAGE := libmei
HEADERS := libmei.h
#
MEI := mei
LIBS := lib$(MEI).so

all: $(LIBS)

INCLUDES := -I. -I./include
CXXFLAGS=-Wall
CFLAGS += -Wall -ggdb $(INCLUDES) -fPIC -O2
LDFLAGS += -Wl,-rpath=.

ifeq ($(ARCH),i386)
CFLAGS += -m32
LDFLAGS += -m32
endif

LIBDIR ?= /usr/local/lib
INCDIR ?= /usr/include/$(MEI)

lib%.so: %.o
	$(CC) $(LDFLAGS) --shared $^ -o $@

clean:
	$(RM) $(PROGS) $(LIBS) *.o

pack: ver=$(shell git describe)
pack:
	git archive --format=tar --prefix=$(PACKAGE)-$(ver)/ HEAD | gzip > $(PACKAGE)-$(ver).tar.gz

tags: $(wildcard *.[ch])
	ctags $^

install_lib: $(LIBS)
	$(INSTALL) -D $^ $(LIBDIR)/$^

install_headers: $(HEADERS)
	$(INSTALL) -m 0644 -D $^ $(INCDIR)/$^

.PHONY: clean tags
