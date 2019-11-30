export LD_LIBRARY_PATH=

THRIFTLIB = /usr/local/lib/thrift_0_12_0_lib
BOOSTLIB = /usr/local/lib/boost_1_68_0_lib
LIBEVENTLIB = /usr/local/lib/libevent_2_1_lib
LIBCONFIGLIB = /usr/local/lib/libconfig_11_0_2_lib
MONGOCXXLIB = /usr/local/lib/mongocxx_3_3_1_lib
HIREDISLIB = /usr/local/lib/hiredis_0_14_lib
TBBLIB = /usr/local/lib/tbb_U5_lib
POCOLIB = /usr/local/lib/poco_1_9_0_lib
ZKCLIENTLIB = /usr/local/lib/cpp_zookeeper_api_lib
KAFKALIB = /usr/local/lib/kafka_1_1_0_lib
#TCMALLOCLIB = /usr/local/lib/tcmalloc_lib
HTTPSERVERLIB = /usr/local/lib/http_lib

DLIBS=-Wl,-rpath,$(THRIFTLIB):$(BOOSTLIB):$(LIBEVENTLIB):$(LIBCONFIGLIB):$(MONGOCXXLIB):$(ZKCLIENTLIB):$(HIREDISLIB):$(TBBLIB):$(LOCALLIB):$(LOCAL64LIB):$(POCOLIB):$(KAFKALIB)

POS=-L$(THRIFTLIB) -L$(BOOSTLIB) -L$(LIBEVENTLIB) -L$(LIBCONFIGLIB) -L$(MONGOCXXLIB) -L$(HIREDISLIB) -L$(TBBLIB) \
-L$(POCOLIB) -L$(ZKCLIENTLIB) -L$(KAFKALIB) -L$(HTTPSERVERLIB) -L/usr/local/lib -L/usr/lib64

LIBS= -levent -lthrift -lthriftnb -lpthread -lconfig++ -lhiredis -lCppZooKeeper \
-lboost_log_setup -lboost_log -lboost_thread -lboost_date_time -lboost_serialization -lboost_regex -lboost_system \
-lboost_chrono -lcppnetlib-uri \
-lmongocxx -lbsoncxx \
-ltbb -ltbbmalloc -ltbbmalloc_proxy \
-lPocoJSON -lPocoNet -lPocoUtil -lPocoFoundation -lPocoXML \
-lrdkafka -lrdkafka++ \
-lhttpservice \
-ldl

CXXFLAGS +=  -I../ -I../log -I../config -I../dao -I../util -I../http
CXXFLAGS +=  -I../3party/include
CXXFLAGS +=  -I../3party/http_server
CXXFLAGS +=  -I../3party/rapidjson
CXXFLAGS +=  -I../3party/rate_limiter
CXXFLAGS +=  -I../3party/CTPL

CXXFLAGS += -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0
CXXFLAGS += -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0
CXXFLAGS += -I/usr/local/include/tbb_U5
#######remove begin
#CXXFLAGS += -I/usr/local/thrift_0_12_0/include
#######remove end

# Enable C++11
CXXFLAGS += -std=c++11 -DBOOST_LOG_DYN_LINK -DTHREADED
CXXFLAGS += -O3
CXXFLAGS += -g
#CXXFLAGS += -pg

BIN=test

all: $(BIN)

clean:
	rm -f *.o \
	../log/*.o \
	../config/*.o \
	../dao/*.o \
	../util/*.o \
	../http/*.o \
	$(BIN)

LOCAL_SRC=$(wildcard \
../log/*.cpp \
../config/*.cpp \
../dao/*.cpp \
../util/*.cpp \
../http/*.cpp \
./*.cpp \
)
LOCAL_OBJ=$(patsubst %.cpp,%.o,$(LOCAL_SRC))

# Note -pthread: this enables threads!!!
test: $(LOCAL_OBJ)
	@g++ -v
	$(CXX) -o $@ $(LOCAL_OBJ)  \
	$(POS) $(LIBS) $(DLIBS) -pthread
	@g++ -v

