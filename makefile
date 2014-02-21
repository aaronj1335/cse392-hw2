CC = g++
FLAGS = -Wall -Werror
LIBRARIES = -fopenmp
Q2_TARGET = q2
Q3_TARGET = q3

ifeq ($(DEBUG), 1)
	override FLAGS += -O0 -g
endif

ifeq ($(shell uname), Darwin)
	override CC = g++-4.8
endif

SRC_DIR = src
OBJ_DIR = obj
SCRATCH ?= .
VAR_DIR = $(SCRATCH)/var
RESULTS_DIR = results

INPUTS = $(wildcard $(SRC_DIR)/*.cc)
INPUTS_TMP = $(subst $(SRC_DIR),$(OBJ_DIR),$(INPUTS))
OBJECTS = $(INPUTS_TMP:%.cc=%.o)
Q2_OBJECTS = $(filter-out $(OBJ_DIR)/$(Q3_TARGET).o,$(OBJECTS))
Q3_OBJECTS = $(filter-out $(OBJ_DIR)/$(Q2_TARGET).o,$(OBJECTS))
DEPFILES = $(OBJECTS:%.o=%.d)

PERF_FILES = $(VAR_DIR)/1d.txt $(VAR_DIR)/4d.txt

# main application
all: $(Q2_TARGET) $(Q3_TARGET)

$(Q2_TARGET): $(Q2_OBJECTS)
	$(CC) $(FLAGS) $(LIBRARIES) -o $@ $(Q2_OBJECTS)

$(Q3_TARGET): $(Q3_OBJECTS)
	$(CC) $(FLAGS) $(LIBRARIES) -o $@ $(Q3_OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -c -o $@ $<

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -M -MF $@ -MT $(@:%.d=%.o) $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)


# running, testing, etc

test: all
	@test/test

$(VAR_DIR):
	mkdir $@

$(VAR_DIR)/1d.txt: $(VAR_DIR)
	@python test/perfdata.py 1 > $@

$(VAR_DIR)/4d.txt: $(VAR_DIR)
	@python test/perfdata.py 4 > $@

# this is not used b/c it was slow and when doing perf we could care less about
# the actual values
perfdata: $(PERF_FILES)

$(RESULTS_DIR):
	mkdir $@

perf: all | $(RESULTS_DIR)
	time ./$(Q2_TARGET) -nm 1        1>/dev/null 3>$(RESULTS_DIR)/1d_0001M.txt
	time ./$(Q2_TARGET) -nm 10       1>/dev/null 3>$(RESULTS_DIR)/1d_0010M.txt
	time ./$(Q2_TARGET) -nm 100      1>/dev/null 3>$(RESULTS_DIR)/1d_0100M.txt
	time ./$(Q2_TARGET) -nm 300      1>/dev/null 3>$(RESULTS_DIR)/1d_0300M.txt
	time ./$(Q2_TARGET) -nm 1000     1>/dev/null 3>$(RESULTS_DIR)/1d_1000M.txt
	time ./$(Q2_TARGET) -nm 300 -d 4 1>/dev/null 3>$(RESULTS_DIR)/4d_0300M.txt

-include $(DEPFILES)


# cleanup

.PHONY: clean
clean:
	-rm -rf $(OBJ_DIR) $(VAR_DIR) $(Q2_TARGET)

