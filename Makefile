MACRO = DEBUGALL
CFLAGS+= -g -O0  -fPIC  -w -rdynamic  -Wall -Wunused -W -D$(MACRO)
SOURCES1 = socket_server_online.cpp  com_struct.cpp
SOURCES2 = socket_server_offline.cpp  com_struct.cpp
OBJS1 := $(patsubst %.cpp, %.o,$(SOURCES1))

OBJS2 := $(patsubst %.cpp, %.o,$(SOURCES2))
#MYOBJS :=$(notdir, $(OBJS))
XDS1 := $(patsubst %.cpp, %.d,$(SOURCES1))

XDS2 := $(patsubst %.cpp, %.d,$(SOURCES2))

INC= -I./basesdk/
LIBS=basesdk/libbaseservice.a -lpthread
CC = g++
AR=ar
RM=-rm
TARGET1=server_online
TARGET2=server_offline
all:$(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1)  $(LIBS)
	
$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2)  $(LIBS)

#@echo "source files:" $(SOURCES)
#@echo "object files:" $(OBJS)
#@echo "$@ depend on $^ ? $* $(*F) $^"

-include $(XDS1) 
-include $(XDS2) 
%d: %cpp
	@$(CC) -MM $(CFLAGS) $< $(INC) > $@.dd; 
	@sed -i 's,\($(*F)\)o[ :]*,$(*D)/\1o: ,g'  $@.dd ; 
	@sed 's,\($*\)o[ :]*,\1o $@ : ,g' < $@.dd > $@; 
	@echo "\t$(CC) $(CFLAGS) $(INC) -c -o $*o $< $(INC)" >>$@; 
	@echo "\t@echo '' >> $@ " >>$@; 
	@$(RM) $@.dd
clean:
	$(RM) -rf $(OBJS1) $(OBJS2)
	$(RM) -rf $(XDS1) $(XDS2)
	$(RM) -f  $(TARGET1) $(TARGET2)

