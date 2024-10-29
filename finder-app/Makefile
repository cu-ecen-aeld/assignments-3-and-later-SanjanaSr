#Variable definition
CC := $(CROSS_COMPILE)
CFLAGS := -Wall -Wextra -Werror
TARGET :=writer
SRC :=writer.c
OBJ :=$(SRC:.c=.o)

#Build writer application
all: $(TARGET)
#Build from object file
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
#Compile source file
%.o: %.c
	$(CC) $(CFLAGS) -c $<  -o $@
#Clean target
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean 

