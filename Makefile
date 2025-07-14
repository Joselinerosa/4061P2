CC = gcc
CFLAGS = -Wall -g -pthread
TARGET = client
TESTDIR = testcase_dir

# Default target
all: $(TARGET)

# Build the client executable directly from source
$(TARGET): client.c client.h
	$(CC) $(CFLAGS) -o $(TARGET) client.c

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Test case 1: Very small queue with many threads - tests heavy contention
test1: $(TARGET)
	@echo "Running test 1: Very small queue (2), many threads (16), large file"
	./$(TARGET) $(TESTDIR)/test1.txt 2 16 1

# Test case 2: Large queue with few threads - tests if threads can keep up  
test2: $(TARGET)
	@echo "Running test 2: Large queue (50), few threads (2), non-divisible file"
	./$(TARGET) $(TESTDIR)/test2.txt 50 2 2

# Test case 3: Queue size 1 - extreme producer-consumer synchronization
test3: $(TARGET)
	@echo "Running test 3: Queue size 1 (extreme edge case), 8 threads"
	./$(TARGET) $(TESTDIR)/test1.txt 1 8 3

# Test case 4: Single thread - tests if synchronization works with no concurrency
test4: $(TARGET)
	@echo "Running test 4: Single thread, moderate queue (10)"
	./$(TARGET) $(TESTDIR)/test2.txt 10 1 4

# Test case 5: Balanced configuration for performance comparison
test5: $(TARGET)
	@echo "Running test 5: Balanced config - queue (8), threads (8)"
	./$(TARGET) $(TESTDIR)/test1.txt 8 8 5

# Run all tests
test-all: test1 test2 test3 test4 test5

# Phony targets
.PHONY: all clean testcases test1 test2 test3 test4 test5 test-all
