TARGET := chess
CC ?= gcc

POST := ./post-build
TEST := test

CFLAGS := -g -O2 -W -Wvarargs -Wall -Werror -Wno-unused-const-variable -Wno-unused-parameter -Wno-missing-field-initializers -Werror=vla
OBJFLAGS := $(CFLAGS) -c
LDFLAGS := -lvulkan -lX11 -lX11-xcb -lxcb -lm
INCFLAGS := -Isrc -Iengine/src -Itest/src

ENGINE_OBJFILES := memory.o logger.o engine.o clock.o array.o string.o event.o input.o math.o test.o memory_linear_allocator.o memory_dynamic_allocator.o freelist.o platform.o filesystem.o
TARGET_OBJFILES := main.o application.o chess.o chess_string.o chess_bitboard.o chess_attack.o board.o chess_fen.o
TEST_OBJFILES := test_main.o test_memory_linear_allocator.o  test_memory_dynamic_allocator.o

################################################################################

ENGINE_OBJ := $(foreach x,$(ENGINE_OBJFILES), $(addprefix obj/,$(x)))
TARGET_UNIQUE_OBJ := $(foreach x,$(TARGET_OBJFILES), $(addprefix obj/,$(x)))
TARGET_OBJ :=  $(TARGET_UNIQUE_OBJ) $(ENGINE_OBJ)
TEST_UNIQUE_OBJ := $(foreach x,$(TEST_OBJFILES), $(addprefix obj/,$(x)))
TEST_OBJ :=  $(TEST_UNIQUE_OBJ) $(ENGINE_OBJ)
CLEAN := bin/$(TARGET) bin/$(TEST) $(ENGINE_OBJ) $(TARGET_UNIQUE_OBJ) $(TEST_UNIQUE_OBJ)

bin/$(TARGET): $(TARGET_OBJ)
	$(CC) $(INCFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	$(POST)

bin/$(TEST): $(TEST_OBJ)
	$(CC) $(INCFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@bin/$(TEST)

$(TARGET_OBJ):
	$(CC) $(INCFLAGS) $(OBJFLAGS) -o $@ $<
$(TEST_OBJ):
	$(CC) $(INCFLAGS) $(OBJFLAGS) -o $@ $<

# Target obj
obj/main.o: 							src/main.c
obj/application.o: 						src/application.c
obj/chess.o:							src/chess/chess.c
obj/chess_string.o:						src/chess/string.c
obj/chess_bitboard.o:					src/chess/bitboard.c
obj/chess_attack.o:						src/chess/attack.c
obj/board.o:						src/chess/board.c
obj/chess_fen.o:						src/chess/fen.c

# Test obj
obj/test_main.o:						test/src/main.c
obj/test_test.o:						test/src/test.c
obj/test_memory_linear_allocator.o:		test/src/memory/test_linear_allocator.c
obj/test_memory_dynamic_allocator.o:	test/src/memory/test_dynamic_allocator.c

# Engine obj
obj/memory.o: 							engine/src/core/memory.c
obj/logger.o: 							engine/src/core/logger.c
obj/engine.o: 							engine/src/core/engine.c
obj/clock.o: 							engine/src/core/clock.c
obj/event.o: 							engine/src/core/event.c
obj/input.o: 							engine/src/core/input.c
obj/string.o: 							engine/src/core/string.c
obj/array.o: 							engine/src/container/array.c
obj/freelist.o: 						engine/src/container/freelist.c
obj/math.o: 							engine/src/math/math.c
obj/test.o:								engine/src/test/test.c
obj/memory_linear_allocator.o: 			engine/src/memory/linear_allocator.c
obj/memory_dynamic_allocator.o: 		engine/src/memory/dynamic_allocator.c
obj/platform.o: 						engine/src/platform/linux.c
obj/filesystem.o:						engine/src/platform/filesystem.c

default: makedir all

.PHONY: makedir
makedir:
	@echo mkdir -p bin obj
	@mkdir -p bin obj

.PHONY: app
app: bin/$(TARGET)

.PHONY: all
all: clean app run

.PHONY: test
test: clean bin/$(TEST) app run

.PHONY: clean
clean:
	@echo rm -f $(CLEAN)
	@rm -f $(CLEAN)

.PHONY: run
run:
	@bin/$(TARGET)
