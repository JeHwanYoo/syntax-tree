# macro
TARGET = lrparser
SOURCES = main lrparser
HEADERS = lrparser.h
OBJECTS = $(foreach s, $(SOURCES), $(s).o)
CFLAGS = -c
COMPILE = $(CC) $(CFLAGS) $< -o $@

# call macro
OBJ = $(word $(1), $(SOURCES)).o
CPP = $(word $(1), $(SOURCES)).c
HEADER = $(word $(1), $(HEADERS))

# debug
ifeq ($(debug), 1)
	CFLAGS += -g
	TARGET = lrparser-debug
	OBJECTS = $(foreach s, $(SOURCES), debug/$(s).o)
	OBJ = debug/$(word $(1), $(SOURCES)).o
endif

# DEPendencies
all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@

$(call OBJ, 1): $(call CPP, 1) $(call HEADER, 1) 
	$(COMPILE)
$(call OBJ, 2): $(call CPP, 2) $(call HEADER, 1)
	$(COMPILE)

# utilities
clean:
	$(RM) *.o $(TARGET)

cleand:
	$(RM) debug/*.o $(TARGET)-debug

# test macro
echo:
	echo $(call OBJ, 1) && echo $(OBJECTS)
