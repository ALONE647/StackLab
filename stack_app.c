/*
 * ============================================================
 *   STACKLAB  -  Applications of the Stack Data Structure
 * ============================================================
 *   Course  : Data Structures and Algorithms (Lab)
 *   Author  : Dinesh Aryan M  (RA2511026010282)
 *   Lang    : C (C99)  -  works in Dev-C++, GCC, Clang
 *
 *   Modules
 *     1. Stack Playground      - push / pop / peek / display
 *     2. Balanced Brackets     - checks (), {}, []
 *     3. Infix  -> Postfix     - operator precedence conversion
 *     4. Postfix Evaluation    - evaluates multi-digit expressions
 *     5. String Reversal       - reverse a word / sentence
 *     6. Undo / Redo Editor    - two-stack text history
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX 100
#define LINE 256

/* ---------- Colours (ANSI) ---------- */
#define RESET  "\033[0m"
#define BOLD   "\033[1m"
#define DIM    "\033[2m"
#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"
#define BLUE   "\033[34m"
#define CYAN   "\033[36m"

/* ============================================================
 *                    CORE STACK (array based)
 * ============================================================ */
typedef struct {
    int top;
    long items[MAX];
} Stack;

void init(Stack *s)            { s->top = -1; }
int  isEmpty(const Stack *s)   { return s->top == -1; }
int  isFull(const Stack *s)    { return s->top == MAX - 1; }

int push(Stack *s, long value) {
    if (isFull(s)) return 0;            /* overflow  */
    s->items[++s->top] = value;
    return 1;
}

int pop(Stack *s, long *out) {
    if (isEmpty(s)) return 0;           /* underflow */
    *out = s->items[s->top--];
    return 1;
}

int peek(const Stack *s, long *out) {
    if (isEmpty(s)) return 0;
    *out = s->items[s->top];
    return 1;
}

/* ============================================================
 *                        UI HELPERS
 * ============================================================ */
void enableColours(void) {
#ifdef _WIN32
    /* Turns on ANSI colour support in the Windows 10/11 console */
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | 0x0004); /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
#endif
}

void clearScreen(void) { printf("\033[2J\033[H"); }

void readLine(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\n")] = '\0';
}

void pauseScreen(void) {
    char tmp[8];
    printf(DIM "\n  Press Enter to return to the menu..." RESET);
    if (!fgets(tmp, sizeof tmp, stdin)) return;
}

void header(const char *title) {
    clearScreen();
    printf(CYAN BOLD "\n  +------------------------------------------+\n");
    printf("  |  %-40s|\n", title);
    printf("  +------------------------------------------+\n\n" RESET);
}

void ok(const char *msg)   { printf(GREEN "  [OK] %s\n" RESET, msg); }
void err(const char *msg)  { printf(RED   "  [!!] %s\n" RESET, msg); }

/* Draws the stack vertically, top element first */
void drawStack(const Stack *s, int asChars) {
    int i;
    if (isEmpty(s)) {
        printf(DIM "        (stack is empty)\n" RESET);
        printf("        +--------+\n");
        return;
    }
    for (i = s->top; i >= 0; i--) {
        if (asChars) printf("        |   %c    |", (char)s->items[i]);
        else         printf("        | %6ld |", s->items[i]);
        if (i == s->top) printf(YELLOW "  <- TOP" RESET);
        printf("\n");
    }
    printf("        +--------+\n");
    printf(DIM "        size: %d / %d\n" RESET, s->top + 1, MAX);
}

/* ============================================================
 *  1. STACK PLAYGROUND
 * ============================================================ */
void playground(void) {
    Stack s;
    char buf[LINE];
    long v;
    init(&s);

    for (;;) {
        header("1. STACK PLAYGROUND");
        drawStack(&s, 0);
        printf("\n  " BOLD "1" RESET " Push   " BOLD "2" RESET " Pop   "
               BOLD "3" RESET " Peek   " BOLD "0" RESET " Back\n");
        readLine("\n  Choice > ", buf, LINE);

        if (buf[0] == '1') {
            readLine("  Value to push > ", buf, LINE);
            v = strtol(buf, NULL, 10);
            if (push(&s, v)) ok("Pushed.");
            else err("Stack Overflow! Cannot push.");
        } else if (buf[0] == '2') {
            if (pop(&s, &v)) printf(GREEN "  [OK] Popped %ld\n" RESET, v);
            else err("Stack Underflow! Nothing to pop.");
        } else if (buf[0] == '3') {
            if (peek(&s, &v)) printf(GREEN "  [OK] Top element is %ld\n" RESET, v);
            else err("Stack is empty.");
        } else if (buf[0] == '0') {
            return;
        } else {
            err("Invalid choice.");
        }
        pauseScreen();
    }
}

/* ============================================================
 *  2. BALANCED BRACKETS
 * ============================================================ */
int matches(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '{' && close == '}') ||
           (open == '[' && close == ']');
}

void balancedBrackets(void) {
    Stack s;
    char expr[LINE];
    long top;
    int i;
    init(&s);

    header("2. BALANCED BRACKETS CHECKER");
    printf(DIM "  Example: {a + (b * c)} - [d / e]\n\n" RESET);
    readLine("  Enter expression > ", expr, LINE);
    printf("\n");

    for (i = 0; expr[i]; i++) {
        char c = expr[i];
        if (c == '(' || c == '{' || c == '[') {
            push(&s, c);
            printf("  push '%c'\n", c);
        } else if (c == ')' || c == '}' || c == ']') {
            if (!pop(&s, &top)) {
                printf("\n");
                printf(RED "  NOT BALANCED" RESET " - extra '%c' at position %d\n", c, i + 1);
                pauseScreen();
                return;
            }
            if (!matches((char)top, c)) {
                printf("\n");
                printf(RED "  NOT BALANCED" RESET " - '%c' closed by '%c' at position %d\n",
                       (char)top, c, i + 1);
                pauseScreen();
                return;
            }
            printf("  pop  '%c'  matched with '%c'\n", (char)top, c);
        }
    }

    printf("\n");
    if (isEmpty(&s)) printf(GREEN BOLD "  BALANCED  :)\n" RESET);
    else             printf(RED "  NOT BALANCED" RESET " - %d bracket(s) left unclosed\n", s.top + 1);
    pauseScreen();
}

/* ============================================================
 *  3. INFIX -> POSTFIX
 * ============================================================ */
int precedence(char op) {
    switch (op) {
        case '^':           return 3;
        case '*': case '/': case '%': return 2;
        case '+': case '-': return 1;
        default:            return 0;
    }
}

int isOperator(char c) { return strchr("+-*/%^", c) != NULL && c != '\0'; }

/* Converts infix to postfix. Tokens in output are space-separated.
 * Returns 1 on success, 0 on mismatched parentheses. */
int infixToPostfix(const char *in, char *out, int verbose) {
    Stack s;
    long top;
    int i, k = 0;
    init(&s);

    for (i = 0; in[i]; i++) {
        char c = in[i];
        if (isspace((unsigned char)c)) continue;

        if (isalnum((unsigned char)c)) {
            /* read a full operand (multi-digit number or variable) */
            while (isalnum((unsigned char)in[i])) out[k++] = in[i++];
            out[k++] = ' ';
            i--;
        } else if (c == '(') {
            push(&s, c);
        } else if (c == ')') {
            while (peek(&s, &top) && top != '(') {
                pop(&s, &top);
                out[k++] = (char)top; out[k++] = ' ';
            }
            if (!pop(&s, &top)) return 0;            /* no '(' found */
        } else if (isOperator(c)) {
            /* '^' is right-associative, others left-associative */
            while (peek(&s, &top) && isOperator((char)top) &&
                   (precedence((char)top) > precedence(c) ||
                   (precedence((char)top) == precedence(c) && c != '^'))) {
                pop(&s, &top);
                out[k++] = (char)top; out[k++] = ' ';
            }
            push(&s, c);
        }

        if (verbose) {
            int j;
            char st[MAX + 1];
            for (j = 0; j <= s.top; j++) st[j] = (char)s.items[j];
            st[j] = '\0';
            out[k] = '\0';
            printf("  %-3c | %-12s | %s\n", c, st, out);
        }
    }

    while (pop(&s, &top)) {
        if (top == '(') return 0;                    /* unclosed '(' */
        out[k++] = (char)top; out[k++] = ' ';
    }
    out[k] = '\0';
    return 1;
}

void infixMenu(void) {
    char in[LINE], out[LINE * 2];

    header("3. INFIX  ->  POSTFIX");
    printf(DIM "  Example: A + B * (C - D) / E\n\n" RESET);
    readLine("  Enter infix expression > ", in, LINE);
    printf("\n  " BOLD "tok | stack        | output\n" RESET);
    printf("  ----+--------------+----------------------\n");

    if (infixToPostfix(in, out, 1))
        printf("\n  " GREEN BOLD "Postfix: %s\n" RESET, out);
    else
        err("Mismatched parentheses.");
    pauseScreen();
}

/* ============================================================
 *  4. POSTFIX EVALUATION
 * ============================================================ */
long ipow(long base, long exp) {
    long r = 1;
    while (exp-- > 0) r *= base;
    return r;
}

void postfixEval(void) {
    Stack s;
    char expr[LINE];
    char *tok;
    long a, b, r;
    init(&s);

    header("4. POSTFIX EVALUATION");
    printf(DIM "  Separate tokens with spaces.  Example: 12 3 4 * + 6 -\n\n" RESET);
    readLine("  Enter postfix expression > ", expr, LINE);
    printf("\n");

    for (tok = strtok(expr, " "); tok; tok = strtok(NULL, " ")) {
        if (isdigit((unsigned char)tok[0]) ||
           (tok[0] == '-' && isdigit((unsigned char)tok[1]))) {
            push(&s, strtol(tok, NULL, 10));
            printf("  push %ld\n", s.items[s.top]);
        } else if (strlen(tok) == 1 && isOperator(tok[0])) {
            if (!pop(&s, &b) || !pop(&s, &a)) {
                err("Invalid expression - not enough operands.");
                pauseScreen();
                return;
            }
            switch (tok[0]) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '^': r = ipow(a, b); break;
                case '/': case '%':
                    if (b == 0) { err("Division by zero."); pauseScreen(); return; }
                    r = (tok[0] == '/') ? a / b : a % b;
                    break;
                default: r = 0;
            }
            push(&s, r);
            printf("  %ld %c %ld = " YELLOW "%ld" RESET "\n", a, tok[0], b, r);
        } else {
            printf(RED "  [!!] Unknown token '%s'\n" RESET, tok);
            pauseScreen();
            return;
        }
    }

    if (s.top == 0) printf("\n  " GREEN BOLD "Result: %ld\n" RESET, s.items[0]);
    else            err("Invalid expression - too many operands.");
    pauseScreen();
}

/* ============================================================
 *  5. STRING REVERSAL
 * ============================================================ */
void reverseString(void) {
    Stack s;
    char str[LINE];
    long c;
    int i;
    init(&s);

    header("5. STRING REVERSAL");
    readLine("  Enter text > ", str, MAX);

    for (i = 0; str[i]; i++) push(&s, str[i]);

    printf("\n  Stack after pushing every character:\n\n");
    drawStack(&s, 1);

    printf("\n  Reversed: " GREEN BOLD);
    while (pop(&s, &c)) putchar((char)c);
    printf("\n" RESET);
    pauseScreen();
}

/* ============================================================
 *  6. UNDO / REDO TEXT EDITOR
 *     Before every change, a snapshot of the document is pushed
 *     onto the UNDO stack. Undo pops it back and saves the current
 *     text on the REDO stack; redo does the reverse.
 * ============================================================ */
#define DOC 1024
#define HIST 50

typedef struct {
    int top;
    char snap[HIST][DOC];
} TextStack;

void tsInit(TextStack *t) { t->top = -1; }

int tsPush(TextStack *t, const char *s) {
    if (t->top == HIST - 1) {                  /* drop oldest when full */
        memmove(t->snap[0], t->snap[1], (size_t)(HIST - 1) * DOC);
        t->top--;
    }
    strcpy(t->snap[++t->top], s);
    return 1;
}

int tsPop(TextStack *t, char *out) {
    if (t->top == -1) return 0;
    strcpy(out, t->snap[t->top--]);
    return 1;
}

void editor(void) {
    static TextStack undo, redo;               /* static: large, keep off the call stack */
    char doc[DOC] = "", buf[LINE];
    tsInit(&undo);
    tsInit(&redo);

    for (;;) {
        header("6. UNDO / REDO EDITOR");
        printf("  Document: " BOLD "\"%s\"\n\n" RESET, doc);
        printf(DIM "  undo stack: %d   redo stack: %d\n\n" RESET, undo.top + 1, redo.top + 1);
        printf("  " BOLD "1" RESET " Type   " BOLD "2" RESET " Undo   "
               BOLD "3" RESET " Redo   " BOLD "0" RESET " Back\n");
        readLine("\n  Choice > ", buf, LINE);

        if (buf[0] == '1') {
            readLine("  Text to add > ", buf, LINE);
            if (strlen(doc) + strlen(buf) + 1 >= DOC) { err("Document is full."); pauseScreen(); continue; }
            tsPush(&undo, doc);
            tsInit(&redo);                     /* new action clears redo */
            if (doc[0]) strcat(doc, " ");
            strcat(doc, buf);
        } else if (buf[0] == '2') {
            char prev[DOC];
            if (tsPop(&undo, prev)) { tsPush(&redo, doc); strcpy(doc, prev); }
            else { err("Nothing to undo."); pauseScreen(); }
        } else if (buf[0] == '3') {
            char next[DOC];
            if (tsPop(&redo, next)) { tsPush(&undo, doc); strcpy(doc, next); }
            else { err("Nothing to redo."); pauseScreen(); }
        } else if (buf[0] == '0') {
            return;
        }
    }
}

/* ============================================================
 *                          MAIN MENU
 * ============================================================ */
void banner(void) {
    clearScreen();
    printf(CYAN BOLD);
    printf("\n   ____  _             _      _           _     \n");
    printf("  / ___|| |_ __ _  ___| | __ | |    __ _ | |__  \n");
    printf("  \\___ \\| __/ _` |/ __| |/ / | |   / _` || '_ \\ \n");
    printf("   ___) | || (_| | (__|   <  | |__| (_| || |_) |\n");
    printf("  |____/ \\__\\__,_|\\___|_|\\_\\ |_____\\__,_||_.__/ \n");
    printf(RESET DIM "        Applications of the Stack  |  LIFO in action\n\n" RESET);
}

int main(void) {
    char buf[LINE];
    enableColours();

    for (;;) {
        banner();
        printf("   " YELLOW "1" RESET "  Stack Playground       " DIM "push / pop / peek" RESET "\n");
        printf("   " YELLOW "2" RESET "  Balanced Brackets      " DIM "( ) { } [ ]" RESET "\n");
        printf("   " YELLOW "3" RESET "  Infix  -> Postfix      " DIM "A+B*C  ->  A B C * +" RESET "\n");
        printf("   " YELLOW "4" RESET "  Postfix Evaluation     " DIM "2 3 4 * +  ->  14" RESET "\n");
        printf("   " YELLOW "5" RESET "  String Reversal        " DIM "hello  ->  olleh" RESET "\n");
        printf("   " YELLOW "6" RESET "  Undo / Redo Editor     " DIM "two-stack history" RESET "\n");
        printf("   " RED "0" RESET "  Exit\n");
        readLine("\n  Select > ", buf, LINE);

        switch (buf[0]) {
            case '1': playground();       break;
            case '2': balancedBrackets(); break;
            case '3': infixMenu();        break;
            case '4': postfixEval();      break;
            case '5': reverseString();    break;
            case '6': editor();           break;
            case '0':
                printf(GREEN "\n  Bye! Keep pushing :)\n\n" RESET);
                return 0;
            default: break;
        }
    }
}
