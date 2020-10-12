BASEDIR := ..
include $(BASEDIR)/Makefile.inc

IDIR = \
-I$(BASEDIR)/Inc \
-I$(LRR_INC) \
-I$(LRR_INC_STMFAKE) \
-I$(CAN_BUS_PRORO_INC)

CXX=g++
CXXFLAGS=$(IDIR) -std=c++17 -g
CC=gcc
CFLAGS=$(IDIR) -Wall -Wextra -std=c99 -pedantic -Wmissing-prototypes \
    -Wstrict-prototypes -Wold-style-definition -g


LIBS=-lboost_unit_test_framework -lpthread

BUILD_DIR = build

C_SOURCES =  \
$(BASEDIR)/Src/logic.c \
$(BASEDIR)/Src/can.c \
$(LRR_SRC)/lrr_usart.c \
$(LRR_SRC)/lrr_math.c \
$(LRR_SRC)/lrr_kty8x.c


CPP_SOURCES = \
testmain.cpp \
$(LRR_SRC_STMFAKE)/stm32_fake.cpp

# list of C program sources
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

#list of C++ program sources
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile TestLogic.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/test: $(OBJECTS) Makefile
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)
	
$(BUILD_DIR):
	mkdir $@

test: $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

.DEFAULT_GOAL := test
.PHONY: clean

clean:
	-rm -fR $(BUILD_DIR) test