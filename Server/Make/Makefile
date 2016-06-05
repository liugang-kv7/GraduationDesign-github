############################################################
#some variables
############################################################
CC	= gcc
CXX	= g++
CPP	= g++
AR	= ar

###########################################################
#some flags
###########################################################
ARFLAGS = cr
CPPFLAGS= -fPIC -g -gdwarf-2 -ggdb3 -rdynamic -Wall -Werror $(PUBLIC_INC)
LDFLAGS = -g -L/usr/lib/mysql -lmysqlclient_r -L/usr/lib64/mysql -lpthread
PUBLIC_INC	= -I/usr/include/mysql
SRC = $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.cxx)
OBJS = $(addprefix $(OBJ_DIR)/,$(subst .cpp,.o,$(SRC))) 

BINARY := ../server
SRCDIR := ..
.PHONY: all clean
all: cleantarget $(BINARY)
LOGICSERVER_OBJS_DIR = .objs


# Analyze project
CPP_FILES := $(shell find $(SRCDIR) -name "*.cpp")
INC_DIR   := $(addprefix -I,$(shell find $(SRCDIR) -type d | egrep -v '\.\.$$|\.svn'))
OBJ_FILES := $(subst /,-,$(subst $(SRCDIR)/,,$(CPP_FILES:%.cpp=%.o)))
DEP_FILES := $(subst /,-,$(subst $(SRCDIR)/,,$(CPP_FILES:%.cpp=%.d)))

CPPFLAGS +=  $(INC_DIR)  -I../Include -I../Frame -I../proto_out \
			-I../google/protobuf

LDFLAGS += ../google/libprotobuf.a ../proto_out/libproto_out.a

ARFLAGS = cr

# Generate target 

VERSION_STR = --- Build Project success!! ---
LOGIC_BIN_DIR = ../Bin


# Generate dependence, including rules to generate object files
%.d:
	@echo Generate $@ ...
	@g++ -MM $(CPPFLAGS) $(SRCDIR)/$(subst -,/,$*).cpp > $@.$$$$; \
	sed 's,.*:,$*.o:,g' < $@.$$$$ >$@; \
	echo '	g++ -c $(CPPFLAGS) $(SRCDIR)/$(subst -,/,$*).cpp -o $*.o' >>$@; \
	echo '$@: $(SRCDIR)/$(subst -,/,$*).cpp' >> $@; \
	rm -f $@.$$$$;


# Clean project
cleantarget:
	-rm -rf $(BINARY)

install:
	cp -f ../server $(LOGIC_BIN_DIR)

clean:
	-rm -rf $(BINARY) $(OBJ_FILES) $(DEP_FILES) *.d.* *.d *.o

