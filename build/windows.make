TARGET := cce.exe
CC ?= gcc

POST := build\.post-windows.bat
TEST := test.exe

CFLAGS := -g -O2 -W -Wvarargs -Wall -Werror -Wno-unused-const-variable -Wno-unused-parameter -Wno-missing-field-initializers -Werror=vla
DEPS := m
INCLUDE := src engine\src test\src

ENGINE_OBJFILES := memory.o logger.o engine.o clock.o array.o string.o event.o input.o math.o test.o memory_linear_allocator.o memory_dynamic_allocator.o freelist.o platform.o filesystem.o
TARGET_OBJFILES := main.o application.o chess_string.o chess_bitboard.o chess_attack.o chess_board.o chess_fen.o chess_move.o chess_perft.o
TEST_OBJFILES := test_main.o test_memory_linear_allocator.o  test_memory_dynamic_allocator.o

################################################################################

INCFLAGS := $(foreach x,$(INCLUDE), $(addprefix -I,$(x)))
OBJFLAGS := $(CFLAGS) -c
LDFLAGS := $(foreach x,$(DEPS), $(addprefix -L,$(x)))

ENGINE_OBJ := $(foreach x,$(ENGINE_OBJFILES), $(addprefix obj\,$(x)))
TARGET_UNIQUE_OBJ := $(foreach x,$(TARGET_OBJFILES), $(addprefix obj\,$(x)))
TARGET_OBJ :=  $(TARGET_UNIQUE_OBJ) $(ENGINE_OBJ)
TEST_UNIQUE_OBJ := $(foreach x,$(TEST_OBJFILES), $(addprefix obj\,$(x)))
TEST_OBJ :=  $(TEST_UNIQUE_OBJ) $(ENGINE_OBJ)

CLEAN := bin\$(TARGET) bin\$(TEST) $(ENGINE_OBJ) $(TARGET_UNIQUE_OBJ) $(TEST_UNIQUE_OBJ)

bin\$(TARGET): $(TARGET_OBJ)
	$(CC) $(INCFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@$(POST)

bin\$(TEST): $(TEST_OBJ)
	$(CC) $(INCFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@bin\$(TEST)

$(TARGET_OBJ):
	$(CC) $(INCFLAGS) $(OBJFLAGS) -o $@ $<
$(TEST_OBJ):
	$(CC) $(INCFLAGS) $(OBJFLAGS) -o $@ $<

# Target objects.
obj\main.o:								src\main.c
obj\application.o:						src\cce\application.c
obj\chess_string.o:						src\chess\string.c
obj\chess_bitboard.o:					src\chess\bitboard.c
obj\chess_attack.o:						src\chess\attack.c
obj\chess_board.o:						src\chess\board.c
obj\chess_fen.o:						src\chess\fen.c
obj\chess_move.o:						src\chess\move.c
obj\chess_perft.o:						src\chess\test\perft.c

# Test objects.
obj\test_main.o:						test\src\main.c
obj\test_test.o:						test\src\test.c
obj\test_memory_linear_allocator.o:		test\src\memory\test_linear_allocator.c
obj\test_memory_dynamic_allocator.o:	test\src\memory\test_dynamic_allocator.c

# Engine objects.
obj\memory.o:							engine\src\core\memory.c
obj\logger.o:							engine\src\core\logger.c
obj\engine.o:							engine\src\core\engine.c
obj\clock.o:							engine\src\core\clock.c
obj\event.o:							engine\src\core\event.c
obj\input.o:							engine\src\core\input.c
obj\string.o:							engine\src\core\string.c
obj\array.o:							engine\src\container\array.c
obj\freelist.o:							engine\src\container\freelist.c
obj\math.o:								engine\src\math\math.c
obj\test.o:								engine\src\test\test.c
obj\memory_linear_allocator.o:			engine\src\memory\linear_allocator.c
obj\memory_dynamic_allocator.o:			engine\src\memory\dynamic_allocator.c
obj\platform.o:							engine\src\platform\windows.c
obj\filesystem.o:						engine\src\platform\filesystem.c

.PHONY: app
app: mkdir clean bin\$(TARGET)

.PHONY: all
all: mkdir clean app run

.PHONY: test
test: mkdir clean bin\$(TEST) app run

.PHONY: mkdir
mkdir:
	@if not exist bin mkdir bin
	@if not exist obj mkdir obj

.PHONY: clean
clean:
	@echo del $(CLEAN)
	@del $(CLEAN) 2>NUL

.PHONY: run
run:
	@bin\$(TARGET)