ARFLAGS = cr
CPPFLAGS= -g -O2 -Wall -Werror $(INC) -fPIC

TARGET	= $(DEBUG_TARGET)

DEBUG_TARGET = $(DEBUG_TARGET_DIR)/libproto_out.a
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/libproto_out.a

OBJ_DIR	= ./.objs

DEBUG_TARGET_DIR = ./
RELEASE_TARGET_DIR = ./

INC	= -I../ 
VPATH	= .:$(SERVER_DIR)

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJ_DIR)/,$(subst .cpp,.o,$(SRC)))


all : $(TARGET)

$(TARGET) : $(OBJS)
	@if [ ! -e $(DEBUG_TARGET_DIR) ]; then \
		mkdir -p $(DEBUG_TARGET_DIR); \
	fi
	@if [ ! -e $(RELEASE_TARGET_DIR) ]; then \
		mkdir -p $(RELEASE_TARGET_DIR); \
	fi
	$(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : %.cpp
	@if [ ! -e $(OBJ_DIR) ]; then \
		mkdir $(OBJ_DIR); \
	fi
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS) $(TARGET)
