CXX = g++
CXX_FLAGS = -O3 -I$(LOGGER_DIR)

SRCS = logger_test.cc task_executor/task_executor.cc
OBJS = $(SRCS:.cc=.o)
TARGET = logger_test

LOGGER_DIR = logger
LOGGER_LIB_DIR = $(LOGGER_DIR)/lib
LOGGER_LIB = $(LOGGER_LIB_DIR)/logger.so

all: $(TARGET)

$(TARGET): $(OBJS) $(LOGGER_LIB)
	$(CXX) $(OBJS) -L$(LOGGER_LIB_DIR) -llogger -Wl,-rpath,$(LOGGER_LIB_DIR) -o $@

%.o: %.cc
	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

$(LOGGER_LIB):
	$(MAKE) -C $(LOGGER_DIR)

clean:
	rm -f $(OBJS) $(TARGET)
	$(MAKE) -C $(LOGGER_DIR) clean
	