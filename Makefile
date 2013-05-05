#
INSTALL := cp
PACKAGE := libmei
#
MEI := mei
LIBS := lib$(MEI).so

all: $(LIBS)

CXXFLAGS=-Wall
CFLAGS += -Wall -ggdb -I. -fPIC -O2
LDFLAGS += -Wl,-rpath=.

ifeq ($(ARCH),i386)
CFLAGS += -m32
LDFLAGS += -m32
endif

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
	$(INSTALL) $^ $(LIBDIR)/

.PHONY: clean tags
