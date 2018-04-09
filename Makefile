MAIN_DIR = src
TEST_DIR = test

.PHONY: main test doc

all: main

main:
	$(MAKE) -C $(MAIN_DIR)

test:
	$(MAKE) -C $(TEST_DIR)

doc:
	$(MAKE) -C $(MAIN_DIR) doc

clean:
	$(MAKE) -C $(MAIN_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean

run-main:
	$(MAKE) -C $(MAIN_DIR) run

run-test:
	$(MAKE) -C $(TEST_DIR) run
