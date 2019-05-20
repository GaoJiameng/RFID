SRC := #
SRC += server.c
SRC += rfid_lib.c
SRC += uart_init.c
SRC += sqlite3.c

#OBJ := $(subst .c,.o,$(SRC))
OBJ = $(SRC:%.c=%.o)

CC = arm-linux-gcc
FLAG = -Wall
OPTION = -lpthread -ldl 
EXEC_NAME = server
EXEC_PATH = .

.PHONY:clean demo

demo:$(OBJ)
	@echo make ...
	$(CC) $^ -o $(EXEC_PATH)/$(EXEC_NAME) $(FLAG) $(OPTION)
	@echo make over
	@echo Execute target is $(EXEC_PATH)/$(EXEC_NAME)
$(OBJ):%.o:%.c
	$(CC) -c -o $@ $< $(FLAG)
clean:
	@echo clean ...
	rm  *.o -rf
	@echo clean over
	# $(EXEC_PATH)/$(EXEC_NAME)
