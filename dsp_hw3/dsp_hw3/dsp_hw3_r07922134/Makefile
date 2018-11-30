# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
SRIPATH ?= /Users/chenhunghao/Documents/Courses/DSP/dsp_hw3/srilm-1.5.10
MACHINE_TYPE ?= macosx
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w --std=c++11
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
.PHONY: all clean map run

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<
run:
	@#TODO How to run your code toward different txt? 
	@for i in $(shell seq 1 10) ; do \
	    echo "Running $$i.txt"; \
	    ./mydisambig -text testdata/$$i.txt -map $(TO) -lm $(LM) -order 2 > result2/$$i.txt; \
	done;
runtrigram:
	@#TODO How to run your code toward different txt? 
	@for i in $(shell seq 1 10) ; do \
	    echo "Running $$i.txt"; \
	    ./mydisambig -text testdata/$$i.txt -map $(TO) -lm trigram.lm -order 3 > result3/$$i.txt; \
	done;
map:
	@echo "Mapping!"
	@python3 mapping.py $(FROM) $(TO)
clean:
	$(RM) $(OBJ) $(TARGET)

