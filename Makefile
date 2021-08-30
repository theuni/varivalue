CXX         = c++
AR			= ar
ARFLAGS		= -cr
CXXFLAGS    = -O2 -g -Wall -Wextra -Wno-unused-variable
LDFLAGS     =

OBJS  += $(GENCONTEXT_OBJS)
PROGS += $(GENCONTEXT_PROG)

VARIVALUE_OBJS  =
VARIVALUE_OBJS += varivalue_write.o
VARIVALUE_OBJS += varivalue_read.o
VARIVALUE_OBJS += varivalue.o
VARIVALUE_OBJS += varivalue_util.o
VARIVALUE_OBJS += varinum.o
VARIVALUE_OBJS += main.o
VARIVALUE_PROG = varivalue


OBJS  += $(VARIVALUE_OBJS)
PROGS += $(VARIVALUE_PROG)

CXXFLAGS_INT = -std=c++17
CPPFLAGS_INT = -I.
LDFLAGS_INT  = -pthread

all: $(PROGS)

V=
_notat_=@
_notat_0=$(_notat_)
_notat_1=@\#
_at_=@
_at_0=$(_at_)
_at_1=
at = $(_at_$(V))
notat = $(_notat_$(V))

DEPDIR=.deps
DEPS = $(addprefix $(DEPDIR)/,$(OBJS:.o=.o.Tpo))
DIRS = $(dir $(DEPS))
DEPDIRSTAMP=$(DEPDIR)/.stamp


-include $(DEPS)

$(DEPDIRSTAMP):
	@mkdir -p $(dir $(DEPS))
	@touch $@

$(OBJS): | $(DEPDIRSTAMP)

$(VARIVALUE_PROG): $(VARIVALUE_OBJS)
	$(notat)echo LINK $@
	$(at)$(CXX) $(CXXFLAGS_INT) $(CXXFLAGS) $(LDFLAGS_INT) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(notat)echo CXX $<
	$(at)$(CXX) $(CPPFLAGS_INT) $(CPPFLAGS) $(CXXFLAGS_INT) $(CXXFLAGS) $(CXXFLAGS_EXTRA) -c -MMD -MP -MF .deps/$@.Tpo $< -o $@

clean:
	-rm -f $(PROGS)
	-rm -f $(OBJS)
	-rm -f $(DEPS)
	-rm -rf $(DEPDIR)
