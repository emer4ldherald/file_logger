CXX = g++
CXXFLAGS = -O3 -fPIC

LIB_NAME = lib/liblogger.so

SRCS = logger.cc
OBJS = $(SRCS:.cc=.o)

all: $(LIB_NAME)

$(LIB_NAME): $(OBJS) 
	$(CXX) -shared $(OBJS) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB_NAME)

