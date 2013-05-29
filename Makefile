CXXFLAGS=$(shell pkg-config --cflags libbitcoin thrift libconfig++)
LIBS=$(shell pkg-config --libs libbitcoin thrift libconfig++) -lzmq
BASE_MODULES= \
    main.o \
    block_subscribe.o \
    sync_blockchain.o \
    interface_types.o \
    query_service.o \
    service.o \
    echo.o \
    config.o
MODULES=$(addprefix obj/, $(BASE_MODULES))

default: slaved

python:
	thrift -out bcquery/ --gen py interface.thrift
	echo "from service import *\nfrom subscribe import *" > bcquery/__init__.py

obj/interface_types.o: interface.thrift
	mkdir -p src/thrift
	thrift -out src/thrift --gen cpp interface.thrift
	$(CXX) -o $@ -c src/thrift/interface_types.cpp $(CXXFLAGS)

obj/query_service.o: interface.thrift
	mkdir -p src/thrift
	thrift -out src/thrift --gen cpp interface.thrift
	$(CXX) -o $@ -c src/thrift/QueryService.cpp $(CXXFLAGS)

obj/config.o: src/config.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

obj/service.o: src/service.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

obj/sync_blockchain.o: src/sync_blockchain.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

obj/block_subscribe.o: src/block_subscribe.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

obj/echo.o: src/echo.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

obj/main.o: src/main.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

slaved: $(MODULES)
	mkdir -p obj
	$(CXX) -o slaved $(MODULES) $(LIBS)

