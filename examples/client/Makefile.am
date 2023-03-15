noinst_PROGRAMS = client

client_SOURCES = \
	client.cpp \
	1_TestRequest.cpp \
	8_ExecutionReport.cpp \
  x_SecurityListRequest.cpp \
  y_SecurityList.cpp \
	D_NewOrderSingle.cpp \
	V_MarketDataRequest.cpp \
  W_MarketDataIncrementalRefresh.cpp \
	Application.h \
	Application.cpp

client_LDADD = $(top_builddir)/src/C++/libquickfix.la 

INCLUDES = -I$(top_builddir)/include -I..

all-local:
	rm -f ../../bin/client
	ln -s ../examples/client/client ../../bin/client
