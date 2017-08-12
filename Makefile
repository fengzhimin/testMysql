EXE = testMysql

CFLAGS = -lmysqlclient -lpthread -I /usr/include/mysql -g

obj = main.o strOper.o confOper.o fileOper.o

$(EXE) : $(obj)
	gcc -o $@ $^ $(CFLAGS)

.PHONY : clean
clean :
	rm $(EXE) $(obj)
