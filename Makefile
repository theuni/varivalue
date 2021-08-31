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

VARIVALUE_TEST_JSON = varivalue_test_json
VARIVALUE_TEST_JSON_OBJS = test/test_json.o

VARIVALUE_TEST_NONUL = varivalue_test_nonul
VARIVALUE_TEST_NONUL_OBJS = test/no_nul.o

VARIVALUE_TEST_OBJECT = varivalue_test_object
VARIVALUE_TEST_OBJECT_OBJS = test/object.o

VARIVALUE_TEST_UNITEST = varivalue_test_unitester
VARIVALUE_TEST_UNITEST_OBJS = test/unitester.o
$(VARIVALUE_TEST_UNITEST_OBJS): CPPFLAGS += -DJSON_TEST_SRC=\"test\"


OBJS  += $(VARIVALUE_OBJS) $(VARIVALUE_TEST_JSON_OBJS) $(VARIVALUE_TEST_NONUL_OBJS) $(VARIVALUE_TEST_OBJECT_OBJS) $(VARIVALUE_TEST_UNITEST_OBJS)
PROGS += $(VARIVALUE_TEST_JSON) $(VARIVALUE_TEST_NONUL) $(VARIVALUE_TEST_OBJECT) $(VARIVALUE_TEST_UNITEST)

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

$(VARIVALUE_TEST_JSON): $(VARIVALUE_TEST_JSON_OBJS) $(VARIVALUE_OBJS)
	$(notat)echo LINK $@
	$(at)$(CXX) $(CXXFLAGS_INT) $(CXXFLAGS) $(LDFLAGS_INT) $(LDFLAGS) $^ -o $@

$(VARIVALUE_TEST_NONUL): $(VARIVALUE_TEST_NONUL_OBJS) $(VARIVALUE_OBJS)
	$(notat)echo LINK $@
	$(at)$(CXX) $(CXXFLAGS_INT) $(CXXFLAGS) $(LDFLAGS_INT) $(LDFLAGS) $^ -o $@

$(VARIVALUE_TEST_OBJECT): $(VARIVALUE_TEST_OBJECT_OBJS) $(VARIVALUE_OBJS)
	$(notat)echo LINK $@
	$(at)$(CXX) $(CXXFLAGS_INT) $(CXXFLAGS) $(LDFLAGS_INT) $(LDFLAGS) $^ -o $@

$(VARIVALUE_TEST_UNITEST): $(VARIVALUE_TEST_UNITEST_OBJS) $(VARIVALUE_OBJS)
	$(notat)echo LINK $@
	$(at)$(CXX) $(CXXFLAGS_INT) $(CXXFLAGS) $(LDFLAGS_INT) $(LDFLAGS) $^ -o $@


%.o: %.cpp
	$(notat)echo CXX $<
	$(at)$(CXX) $(CPPFLAGS_INT) $(CPPFLAGS) $(CXXFLAGS_INT) $(CXXFLAGS) $(CXXFLAGS_EXTRA) -c -MMD -MP -MF .deps/$@.Tpo $< -o $@

test/%.o: test/%.cpp
	$(notat)echo CXX $<
	$(at)$(CXX) $(CPPFLAGS_INT) $(CPPFLAGS) $(CXXFLAGS_INT) $(CXXFLAGS) -c -MMD -MP -MF .deps/$(@F).Tpo $< -o $@

clean:
	-rm -f $(PROGS)
	-rm -f $(OBJS)
	-rm -f $(DEPS)
	-rm -rf $(DEPDIR)
