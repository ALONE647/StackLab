#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 8
#define VALUE_LEN 31
#define HISTORY_LIMIT 12
#define UNDO_LIMIT 20
#define LINE_LEN 128

#if defined(_WIN32)
#include <windows.h>
static void enable_ansi(void) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode))
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#else
static void enable_ansi(void) {}
#endif

#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"
#define DIM "\x1b[2m"
#define LIME "\x1b[38;5;190m"
#define CYAN "\x1b[38;5;80m"
#define ORANGE "\x1b[38;5;215m"
#define RED "\x1b[38;5;203m"
#define MUTED "\x1b[38;5;245m"

typedef struct {
    char operation[12];
    char value[VALUE_LEN + 1];
} HistoryEntry;

typedef struct {
    char items[CAPACITY][VALUE_LEN + 1];
    int size;
    HistoryEntry history[HISTORY_LIMIT];
    int history_count;
    int challenge_active;
    int challenge_solved;
} AppState;

typedef struct {
    AppState states[UNDO_LIMIT];
    int count;
} UndoStack;

static void clear_screen(void) {
    fputs("\x1b[2J\x1b[H", stdout);
}

static void pause_enter(void) {
    char line[LINE_LEN];
    printf("\n%sPress Enter to continue...%s", MUTED, RESET);
    (void)fgets(line, sizeof line, stdin);
}

static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n && s[n - 1] == '\n') s[n - 1] = '\0';
}

static int read_line(const char *prompt, char *buffer, size_t capacity) {
    printf("%s", prompt);
    if (!fgets(buffer, (int)capacity, stdin)) return 0;
    size_t n = strlen(buffer);
    if (n && buffer[n - 1] != '\n' && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return -1;
    }
    trim_newline(buffer);
    return 1;
}

static void save_undo(UndoStack *undo, const AppState *state) {
    if (undo->count == UNDO_LIMIT) {
        memmove(&undo->states[0], &undo->states[1],
                sizeof(AppState) * (UNDO_LIMIT - 1));
        undo->count--;
    }
    undo->states[undo->count++] = *state;
}

static void record(AppState *state, const char *operation, const char *value) {
    if (state->history_count == HISTORY_LIMIT) {
        memmove(&state->history[0], &state->history[1],
                sizeof(HistoryEntry) * (HISTORY_LIMIT - 1));
        state->history_count--;
    }
    HistoryEntry *entry = &state->history[state->history_count++];
    snprintf(entry->operation, sizeof entry->operation, "%s", operation);
    snprintf(entry->value, sizeof entry->value, "%s", value ? value : "");
}

static void draw(const AppState *state) {
    clear_screen();
    printf(LIME BOLD "  STACK LAB" RESET "  " MUTED "· DATA STRUCTURES IN MOTION" RESET "\n");
    printf(MUTED "  ──────────────────────────────────────────────────────────" RESET "\n\n");
    printf(BOLD "  STACK VISUALIZER" RESET "   " CYAN "LIFO · LAST IN, FIRST OUT" RESET "\n");
    printf("  Size " LIME "%d" RESET " / %d     Top index " LIME "%d" RESET "\n\n",
           state->size, CAPACITY, state->size - 1);

    if (state->size == 0) {
        printf("          " MUTED "┌────────────────────────┐" RESET "\n");
        printf("          " MUTED "│      AWAITING INPUT    │" RESET "\n");
        printf("          " MUTED "└────────────────────────┘" RESET "\n");
    } else {
        for (int i = state->size - 1; i >= 0; --i) {
            printf("          " MUTED "│" RESET "  %-20s  " MUTED "│" RESET,
                   state->items[i]);
            if (i == state->size - 1) printf("  " LIME "← TOP" RESET);
            printf("\n");
        }
    }
    printf("          " MUTED "└────────────────────────┘" RESET "\n");
    printf("            " DIM "BOTTOM · INDEX 0" RESET "\n\n");

    printf(BOLD "  OPERATIONS" RESET "\n");
    printf("  " LIME "1" RESET " Push     " LIME "2" RESET " Pop      " LIME "3" RESET " Peek     "
           LIME "4" RESET " Undo     " LIME "5" RESET " Reset\n");
    printf("  " LIME "6" RESET " Challenge    " LIME "7" RESET " Hint     " LIME "8" RESET " Help     "
           LIME "0" RESET " Quit\n\n");

    printf(BOLD "  COMPLEXITY" RESET "   Push " CYAN "O(1)" RESET "   Pop " CYAN "O(1)" RESET
           "   Peek " CYAN "O(1)" RESET "   Space " CYAN "O(n)" RESET "\n");
    printf("  " BOLD "RECENT MOVES" RESET "  ");
    if (!state->history_count) printf(MUTED "No operations yet." RESET);
    for (int i = 0; i < state->history_count; ++i) {
        const HistoryEntry *e = &state->history[i];
        const char *color = strcmp(e->operation, "POP") == 0 ? ORANGE : LIME;
        printf("%s%s%s", color, e->operation, RESET);
        if (e->value[0]) printf("(%s)", e->value);
        if (i + 1 < state->history_count) printf("  ");
    }
    printf("\n");
    if (state->challenge_active && !state->challenge_solved)
        printf("\n  " CYAN "CHALLENGE" RESET "  Pop the stack. The returned value should be 9.\n");
    if (state->challenge_solved)
        printf("\n  " LIME BOLD "CHALLENGE SOLVED" RESET "  Last in, first out!\n");
    printf("\n");
}

static void push_value(AppState *state, UndoStack *undo) {
    char value[LINE_LEN];
    int result = read_line("  Value to push (1–31 characters): ", value, sizeof value);
    if (result == 0) exit(0);
    if (result < 0) {
        printf(RED "\n  Value is too long. Try again.\n" RESET);
        pause_enter();
        return;
    }
    if (!value[0]) {
        printf(RED "\n  A value cannot be empty.\n" RESET);
        pause_enter();
        return;
    }
    if (strlen(value) > VALUE_LEN) {
        printf(RED "\n  Values can contain up to %d characters.\n" RESET, VALUE_LEN);
        pause_enter();
        return;
    }
    if (state->size == CAPACITY) {
        printf(RED "\n  STACK OVERFLOW — the stack is at its capacity of %d.\n" RESET, CAPACITY);
        pause_enter();
        return;
    }
    save_undo(undo, state);
    memcpy(state->items[state->size], value, strlen(value) + 1);
    state->size++;
    record(state, "PUSH", value);
    printf(LIME "\n  Pushed %s onto the stack. O(1)\n" RESET, value);
    pause_enter();
}

static void pop_value(AppState *state, UndoStack *undo) {
    if (!state->size) {
        printf(RED "\n  STACK UNDERFLOW — there is nothing to pop.\n" RESET);
        pause_enter();
        return;
    }
    save_undo(undo, state);
    char value[VALUE_LEN + 1];
    snprintf(value, sizeof value, "%s", state->items[--state->size]);
    state->items[state->size][0] = '\0';
    record(state, "POP", value);
    printf(ORANGE "\n  pop() → %s" RESET "   " MUTED "(removed from the top · O(1))" RESET "\n", value);
    if (state->challenge_active && !state->challenge_solved) {
        if (strcmp(value, "9") == 0) {
            state->challenge_solved = 1;
            printf(LIME BOLD "  Correct! 9 was pushed last, so it came out first.\n" RESET);
        } else {
            printf(RED "  Not quite. The newest item should come out first.\n" RESET);
        }
    }
    pause_enter();
}

static void peek_value(AppState *state, UndoStack *undo) {
    if (!state->size) {
        printf(RED "\n  STACK UNDERFLOW — there is no top item to peek at.\n" RESET);
        pause_enter();
        return;
    }
    save_undo(undo, state);
    const char *value = state->items[state->size - 1];
    record(state, "PEEK", value);
    printf(CYAN "\n  peek() → %s" RESET "   " MUTED "(stack unchanged · O(1))" RESET "\n", value);
    pause_enter();
}

static void challenge(AppState *state, UndoStack *undo) {
    save_undo(undo, state);
    memset(state, 0, sizeof *state);
    snprintf(state->items[0], VALUE_LEN + 1, "7");
    snprintf(state->items[1], VALUE_LEN + 1, "3");
    snprintf(state->items[2], VALUE_LEN + 1, "9");
    state->size = 3;
    state->challenge_active = 1;
    record(state, "PUSH", "7");
    record(state, "PUSH", "3");
    record(state, "PUSH", "9");
    printf(CYAN "\n  Challenge loaded: push(7), push(3), push(9).\n"
           "  Use Pop to find the next value out.\n" RESET);
    pause_enter();
}

static void show_hint(const AppState *state) {
    if (state->challenge_active && !state->challenge_solved)
        printf(CYAN "\n  Hint: look at the TOP of the stack. The newest value comes out first.\n" RESET);
    else
        printf(CYAN "\n  Hint: LIFO means Last In, First Out. The top item leaves first.\n" RESET);
    pause_enter();
}

static void show_help(void) {
    printf("\n" BOLD "  HOW A STACK WORKS" RESET "\n");
    printf("  A stack adds and removes items at one end: its top.\n\n");
    printf("  " LIME "push(value)" RESET "  Adds a value to the top.\n");
    printf("  " ORANGE "pop()" RESET "        Removes and returns the top value.\n");
    printf("  " CYAN "peek()" RESET "       Reads the top value without removing it.\n");
    printf("  Overflow     Trying to push when all 8 slots are full.\n");
    printf("  Underflow    Trying to pop or peek when the stack is empty.\n");
    printf("  Undo         Restores the previous stack and operation history.\n");
    pause_enter();
}

int main(void) {
    enable_ansi();
    AppState state = {0};
    UndoStack undo = {0};
    char choice[LINE_LEN];

    for (;;) {
        draw(&state);
        int result = read_line("  Select an operation: ", choice, sizeof choice);
        if (result == 0) break;
        if (result < 0 || !choice[0] || choice[1] != '\0') continue;

        switch (choice[0]) {
            case '1': push_value(&state, &undo); break;
            case '2': pop_value(&state, &undo); break;
            case '3': peek_value(&state, &undo); break;
            case '4':
                if (!undo.count) {
                    printf(MUTED "\n  Nothing to undo yet.\n" RESET);
                    pause_enter();
                } else {
                    state = undo.states[--undo.count];
                    printf(LIME "\n  Previous state restored.\n" RESET);
                    pause_enter();
                }
                break;
            case '5':
                if (state.size || state.history_count) {
                    save_undo(&undo, &state);
                    memset(&state, 0, sizeof state);
                }
                break;
            case '6': challenge(&state, &undo); break;
            case '7': show_hint(&state); break;
            case '8': show_help(); break;
            case '0':
                printf("\n  Thanks for exploring Stack Lab.\n\n");
                return 0;
            default: break;
        }
    }
    printf("\n  Thanks for exploring Stack Lab.\n\n");
    return 0;
}


