MACRO = DEBUGALL
CFLAGS+= -g -O2  -fPIC  -w -rdynamic  -Wall -Wunused -W -D$(MACRO)
SOURCES = $(wildcard */*.cpp)
OBJS := $(patsubst %.cpp, %.o,$(SOURCES))
#MYOBJS :=$(notdir, $(OBJS))
XDS := $(patsubst %.cpp, %.d,$(SOURCES))
INC=-I./inc -I. 
#-I./lib/libjpeg/include
#LIBS=lib/libjpeg/lib/libjpeg.a
CC = g++
AR=ar
RM=-rm
TARGET=libbaseservice.a
$(TARGET): $(OBJS)
	$(AR) rc $(TARGET) $(OBJS)  $(LIBS)

#@echo "source files:" $(SOURCES)
#@echo "object files:" $(OBJS)
#@echo "$@ depend on $^ ? $* $(*F) $^"

-include $(XDS) 
%d: %cpp
	@$(CC) -MM $(CFLAGS) $< $(INC) > $@.dd; 
	@sed -i 's,\($(*F)\)o[ :]*,$(*D)/\1o: ,g'  $@.dd ; 
	@sed 's,\($*\)o[ :]*,\1o $@ : ,g' < $@.dd > $@; 
	@echo "\t$(CC) $(CFLAGS) $(INC) -c -o $*o $< $(INC)" >>$@; 
	@echo "\t@echo '' >> $@ " >>$@; 
	@$(RM) $@.dd

clean:
	$(RM) -rf $(OBJS)
	$(RM) -rf $(XDS)
	$(RM) -f main

