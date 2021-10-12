
CC=gcc
CMOCK_DIR=./cmock
CMOCK_TAR=$(CMOCK_DIR)/cmock.tar.gz
UNITY_TAR=$(CMOCK_DIR)/unity.tar.gz
UNITY_DIR=$(CMOCK_DIR)/CMock-2.5.3/vendor/unity
CMOCK_EXE=$(CMOCK_DIR)/CMock-2.5.3/lib/cmock.rb
UNITY_SRC=$(UNITY_DIR)/src
UNITY_SOURCES=$(UNITY_SRC)/unity.c $(UNITY_SRC)/unity.h $(UNITY_SRC)/unity_internals.h

CMOCK_SOURCES = half_duplex_uart.h
TEST_SOURCES = tests/test.c 
MOCK_SOURCES = $(addprefix mocks/Mock,$(CMOCK_SOURCES))

INCLUDE_PATH=-I./mocks -I. -I./tests -I$(UNITY_SRC)


test: mocks
	$(CC) -Wall $(INCLUDE_PATH) $(TEST_SOURCES) $(MOCK_SOURCES) $(UNITY_SOURCES) -o test


$(CMOCK_DIR):
	mkdir $(CMOCK_DIR)
	wget https://github.com/ThrowTheSwitch/CMock/archive/refs/tags/v2.5.3.tar.gz -O $(CMOCK_TAR)
	tar -xvkf $(CMOCK_TAR) -C $(CMOCK_DIR)
	wget https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.5.2.tar.gz -O $(UNITY_TAR)
	# find cmock dir and 
	tar -xvkf $(UNITY_TAR) -C $(UNITY_DIR) --strip 1
	

mocks: $(CMOCK_DIR) $(CMOCK_SOURCES)
	ruby $(CMOCK_EXE) $(CMOCK_SOURCES)

clean:
	rm -rf $(CMOCK_DIR)