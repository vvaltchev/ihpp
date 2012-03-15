

# Just a snippet to stop executing under other make(1) commands
# that won't understand these lines
ifneq (,)
This makefile requires GNU Make.
endif

TARGET_COMPILER?=gnu

include ../makefile.gnu.config
CXXFLAGS ?= -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT)
	

##############################################################
#
# Tools sets
#
##############################################################


TOOL_NAME = kCCF

TOOL = $(TOOL_NAME:%=$(OBJDIR)%$(PINTOOL_SUFFIX))


CPP_FILES = $(wildcard *.cpp)
_OBJS = $(patsubst %.cpp, %.o, $(CPP_FILES))
OBJECTS = $(addprefix $(OBJDIR), $(_OBJS))

##############################################################
#
# build rules
#
##############################################################
all: $(OBJDIR)
	-$(MAKE) make_all
tools: $(OBJDIR)
	-$(MAKE) make_tools

make_all: make_tools 
make_tools: $(TOOL) 




##############################################################
#
# build rules
#
##############################################################


$(OBJDIR)make-directory:
	mkdir -p $(OBJDIR)
	touch $(OBJDIR)make-directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)%.o : %.cpp $(OBJDIR)make-directory
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOL): $(PIN_LIBNAMES)

$(TOOL): %$(PINTOOL_SUFFIX) :  $(OBJECTS)
	${PIN_LD} $(PIN_LDFLAGS) $(LINK_DEBUG) $(OBJECTS) -o $(TOOL) ${PIN_LPATHS} $(PIN_LIBS) $(DBG)


## cleaning
clean:
	-rm -f $(TOOL) $(OBJDIR)*.o
