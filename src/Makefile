OSNAME=
ifeq ($(OS),Windows_NT)
	OSNAME=windows
else
	OSNAME = ${shell uname | tr '[A-Z]' '[a-z]'}
endif

LIBNAME = $(OSNAME)_lib
CXXFLAGS = -Wall -O3 #-static -static-libgcc -static-libstdc++ -pthread #-g -pg
INCLUDES = -I/usr/local/include -I/usr/local/lib/ -I/usr/include/cdd -I/usr/include -I/C/Boost/include/boost-1_60
LIBS = -L/usr/local/lib -L/C/Boost/lib -L/C/minGW/lib
LDFLAGS = -lcddgmp -lgmpxx -lgmp -lboost_regex-mgw49-mt-s-1_60
#LDFLAGS = -lcddgmp -lgmpxx -lgmp -lboost_regex
SRCS = rfpe.cpp RepeatedGame.cpp Automaton.cpp Belief.cpp Environment.cpp Players.cpp Equation.cpp Payoff.cpp BeliefDivision.cpp Util.cpp Reader.cpp Writer.cpp Parser.cpp Profile.cpp Loader.cpp BeliefTransition.cpp CddHandler.cpp MakeAutomaton.cpp Graph.cpp MakeSCG.cpp MakeGraph.cpp Signal.cpp MultiSignal.cpp
OBJS =		$(SRCS:.cpp=.o)
OTHER_OBJS = RepeatedGame.o Automaton.o Belief.o Environment.o Players.o Equation.o Payoff.o BeliefDivision.o Util.o Reader.o Writer.o Parser.o Profile.o Loader.o BeliefTransition.o CddHandler.o
OTHER_OBJS2 = Graph.o Util.o MakeSCG.o
OTHER_OBJS3 = Automaton.o Util.o
OTHER_OBJS4 = Profile.o Util.o
OTHER_OBJS5 = Util.o
TARGET =	rfpe
TARGET2 = 	MakeGraph
TARGET3 = MakeAutomaton
TARGET4 = Signal
TARGET5 = MultiSignal

.SUFFIXES : .cpp .o

all:	$(TARGET) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6)

$(TARGET):	$(LIBNAME)/rfpe.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(LIBNAME)/rfpe.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS)) $(LIBS) $(LDFLAGS) 

$(TARGET2): $(LIBNAME)/MakeGraph.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS2))
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o $(TARGET2) $(LIBNAME)/MakeGraph.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS2)) $(LDFLAGS)

$(TARGET3): $(LIBNAME)/MakeAutomaton.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS3))
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o $(TARGET3) $(LIBNAME)/MakeAutomaton.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS3)) $(LDFLAGS)

$(TARGET4): $(LIBNAME)/Signal.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS4))
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o $(TARGET4) $(LIBNAME)/Signal.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS4)) $(LDFLAGS)

$(TARGET5): $(LIBNAME)/MultiSignal.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS5))
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o $(TARGET5) $(LIBNAME)/MultiSignal.o $(patsubst %,$(LIBNAME)/%,$(OTHER_OBJS5)) $(LDFLAGS)

clean:
	rm -f $(patsubst %,$(LIBNAME)/%,$(OBJS)) $(TARGET) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5)

$(LIBNAME)/%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
