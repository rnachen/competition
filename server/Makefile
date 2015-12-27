PROGRAM   := ../bin/server 

SRCDIRS   := ../comm ../proto ./

OUTPUT	  := ./

DEBUG = 1 
CXXFLAGS  += -I../comm -I../proto -I./ -Wall -g -Wno-deprecated

ifeq ($(DEBUG), 0)
	CXXFLAGS += -O2 -ggdb3 -DNDEBUG -fno-strict-aliasing
endif


LDFLAGS   += -ldl


CC      = gcc

CXX		= g++


CC      = $(CXX)

CFLAGS  = $(CXXFLAGS)


RM        = rm -f


SRCEXTS   := .c .cpp


SHELL   = /bin/sh

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))

OBJS    = $(foreach x,$(SRCEXTS), \
      $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))

DEPS    = $(patsubst %.o,%.d,$(OBJS))

.PHONY : all objs clean cleanall rebuild

all : $(PROGRAM)

# Rules for creating the dependency files (.d).

#---------------------------------------------------

%.d : %.c

	@$(CC) -MM -MD $(CFLAGS) $<



%.d : %.C

	@$(CC) -MM -MD $(CXXFLAGS) $<



%.d : %.cc

	@$(CC) -MM -MD $(CXXFLAGS) $<



%.d : %.cpp

	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@



%.d : %.CPP

	@$(CC) -MM -MD $(CXXFLAGS) $<



%.d : %.c++

	@$(CC) -MM -MD $(CXXFLAGS) $<



%.d : %.cp

	@$(CC) -MM -MD $(CXXFLAGS) $<



%.d : %.cxx

	@$(CC) -MM -MD $(CXXFLAGS) $<



# Rules for producing the objects.

#---------------------------------------------------

objs : $(OBJS)



%.o : %.c

	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@


%.o : %.cpp

	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@


# Rules for producing the executable.

#----------------------------------------------

$(PROGRAM) : $(OBJS)
	$(CXX) -o ${OUTPUT}/$@ $^ $(LDFLAGS)




-include $(DEPS)



rebuild: clean all



clean :

	@$(RM) *.o *.d $(OBJS)



cleanall: clean

	@$(RM) ${OUTPUT}/$(PROGRAM)



### End of the Makefile ##  Suggestions are welcome  ## All rights reserved ###

###############################################################################
