BUILDTYPE ?= Release

AR_FLAGS := rcs
CC_FLAGS := -Wall -Wextra -Werror -std=c++11
CC_FLAGS += -Iinclude

SRC_DIR := $(CURDIR)/src
INCLUDE_DIR := $(CURDIR)/include
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.cc' -or -name '*.h')
SRC_HEADER_FILES := $(shell find $(SRC_DIR) -type f -name '*.h')
INCLUDE_FILES := $(shell find $(INCLUDE_DIR) -type f -name '*.cc' -or -name '*.h')
CPPLINT_BIN := ./tools/cpplint/cpplint.py
CLANG_FORMAT := ./tools/node_modules/.bin/clang-format

OUT_DIR := build/$(BUILDTYPE)
OBJ_FILES := $(foreach			\
		path,			\
		$(SRC_FILES:.cc=.o),	\
		$(OUT_DIR)/$(shell realpath --relative-to $(SRC_DIR) $(path)))

ifeq ($(BUILDTYPE),Release)
	override CXXFLAGS += -O3 -DNDEBUG
else
	override CXXFLAGS += -O0 -g -DDEBUG
endif

all: $(OUT_DIR)/libnbtcc.a $(OUT_DIR)/example

$(OUT_DIR)/example: $(OUT_DIR)/libnbtcc.a example/main.cc
	$(CXX) $(CC_FLAGS) $(CURDIR)/example/main.cc -o $(OUT_DIR)/example $(OUT_DIR)/libnbtcc.a

$(OUT_DIR)/libnbtcc.a: $(OBJ_FILES)
	$(AR) $(AR_FLAGS) $@ $(OBJ_FILES)

$(OUT_DIR)/%.o: src/%.cc $(SRC_HEADER_FILES) $(INCLUDE_FILES)
	@mkdir -p $(dir $@)
	$(CXX) $(CC_FLAGS) -c $< -o $@

%.compdb_entry: %.cc
	@echo "    {" > $@
	@echo "        \"command\": \"cc $(CC_FLAGS) -c $<\","   >> $@
	@echo "        \"directory\": \"$(CURDIR)\","               >> $@
	@echo "        \"file\": \"$<\""                    >> $@
	@echo "    },"                              >> $@

COMPDB_ENTRIES = $(addsuffix .compdb_entry, $(basename $(SRC_FILES))) \
		 $(addsuffix .compdb_entry, $(basename example/main.cc))

compile_commands.json: $(COMPDB_ENTRIES)
	@echo "[" > $@.tmp
	@cat $^ >> $@.tmp
	@sed '$$d' < $@.tmp > $@
	@echo "    }" >> $@
	@echo "]" >> $@
	@rm $(COMPDB_ENTRIES)
	@rm $@.tmp

.PHONY: clean cpplint clang-format
clean:
	rm -rf build
	rm -f compile_commands.json

clang-format: .clang-format-stamp
.clang-format-stamp: $(SRC_FILES) $(SRC_HEADER_FILES) $(INCLUDE_FILES) example/main.cc
	$(CLANG_FORMAT) -i --style=file $(SRC_FILES) $(SRC_HEADER_FILES) $(INCLUDE_FILES) example/main.cc
	touch .clang-format-stamp

cpplint: .cpplint-stamp
.cpplint-stamp: $(SRC_FILES) $(SRC_HEADER_FILES) $(INCLUDE_FILES) example/main.cc
	$(CPPLINT_BIN) $(SRC_FILES) $(SRC_HEADER_FILES) $(INCLUDE_FILES) example/main.cc
	@touch $@

docs: $(INCLUDE_FILES) Doxyfile README.md
	rm -rf docs
	@doxygen Doxyfile
	@touch docs

.PHONY: publish-docs
publish-docs:
	git branch -D gh-pages
	git checkout -b gh-pages
	git pull origin gh-pages
	git merge main --no-ff
	$(MAKE) docs
	./tools/touch_gh-pages_file
	git rm -rf example || echo 0
	git rm -rf include || echo 0
	git rm -rf src || echo 0
	git rm -rf tools || echo 0
	git rm -rf .clang-format || echo 0
	git rm -rf .cpplint || echo 0
	git rm -rf .editorconfig || echo 0
	git rm -rf CPPLINT.cfg || echo 0
	mv docs/html/* docs/ && rm -rf docs/html
	git add -f docs || echo 0
	git add seed
	git commit -m "Update docs"
	git push origin gh-pages
