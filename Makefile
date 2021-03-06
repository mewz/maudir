all: maudir

WARNINGS1 :=					\
	-Wall					\
	-Werror					\
	-Wredundant-decls			\
	-Wmissing-noreturn			\
	-Wshadow				\
	-Wcast-align				\
	-Wwrite-strings				\
	-Winline				\
	-Wformat-nonliteral			\
	-Wformat-security			\
	-Wswitch-enum				\
	-Wswitch-default			\
	-Winit-self				\
	-Wmissing-include-dirs			\
	-Wundef					\
	-Waggregate-return			\
	-Wmissing-format-attribute		\
	$(NULL)

INCLUDES =					\
	-I/usr/include				\
	-I/usr/include/mysql			\
	-I/usr/include/mysql++			\
	$(shell pkg-config --cflags libevent)	\
	$(NULL)

LDFLAGS =					\
	-L/usr/lib				\
	-lmysqlpp				\
	-lmemcached				\
	$(shell pkg-config --libs libevent)	\
	-lwwwhttp				\
	-lwwwxml				\
	-lxmltok				\
	-lxmlparse				\
	-lwwwinit				\
	-lwwwapp				\
	-lwwwhtml				\
	-lwwwtelnet				\
	-lwwwnews				\
	-lwwwhttp				\
	-lwwwmime				\
	-lwwwgopher				\
	-lwwwftp				\
	-lwwwfile				\
	-lwwwdir				\
	-lwwwcache				\
	-lwwwstream				\
	-lwwwmux				\
	-lwwwtrans				\
	-lwwwcore				\
	-lwwwutils				\
	-lmd5					\
	$(NULL)

SOURCES =					\
	string_utils.cpp			\
	create_short_url.cpp			\
	redirect_url.cpp			\
	base62.cpp				\
	main.cpp				\
	event_utils.cpp				\
	data_store.cpp				\
	$(NULL)

%.o: %.cpp
	@echo "  C++\t$@"
	@g++ -g -c -o $*.o -fPIC $(WARNINGS) $(INCLUDES) $*.cpp

maudir: $(SOURCES:.cpp=.o)
	@echo "  LINK\t$@"
	@g++ -g -o $@ -fPIC $(WARNINGS) $(SOURCES:.cpp=.o) $(LDFLAGS)

clean:
	rm -f maudir *.o

