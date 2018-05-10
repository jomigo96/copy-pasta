MAIN_DIR = src
TEST_DIR = test

.PHONY: main test doc

all:
	$(MAKE) -C $(MAIN_DIR)

test:
	$(MAKE) -C $(TEST_DIR)

doc:
	$(MAKE) -C doc doc

clean:
	$(MAKE) -C $(MAIN_DIR) clean
	rm -rf doc/html doc/latex
	$(MAKE) -C $(TEST_DIR) clean

run-test:
	$(MAKE) -C $(TEST_DIR) run
