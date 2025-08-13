#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

// ===== ANSI color codes =====
#define ANSI_RESET      "\x1b[0m"
#define ANSI_GREEN      "\x1b[32m"
#define ANSI_RED        "\x1b[31m"
#define ANSI_YELLOW     "\x1b[33m"
#define ANSI_BLUE       "\x1b[34m"
#define ANSI_MAGENTA    "\x1b[35m"
#define ANSI_CYAN       "\x1b[36m"
#define ANSI_WHITE      "\x1b[37m"
#define ANSI_GRAY       "\x1b[90m"
#define ANSI_BOLD       "\x1b[1m"

#define MAX_CARDS 100
#define MAX_LEN   200

// ===== Data types =====
typedef struct {
    char question[MAX_LEN];
    char answer[MAX_LEN];
} FlashCard;

static FlashCard deck[MAX_CARDS];
static int totalCards = 0;

// ===== Forward declarations =====
void clearScreen(void);
void hideCursor(void);
void showCursor(void);
void displayHeader(void);
void displayMenuMain(int selected);
void displayHeaderFlash(void);
void displayMenuFlash(int selected);

void showTypeWriter(void);
void showFlash(void);
void showplayGuessingGame(void);
void showChatbot(void);

// flash subfeatures
void showAddCard(void);
void showViewCards(void);
void showQuiz(void);
void showStats(void);

// typing-test helpers
void print_colored_char(char c, const char* color);
void draw_text(char** words, int num_words, int current_word_index, const char* typed_word, double wpm, double accuracy, int time_left);

// guessing game (simulation)
typedef struct {
    char target[30];
    int  gamesPlayed;
} GameData;

void saveGameData(GameData data);
GameData loadGameData(void);
char getSingleChar(void);
void playGuessingGame(void);

// ===== Utilities =====
static int enableVTMode(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return 0;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return 0;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    return SetConsoleMode(hOut, dwMode);
}

void clearScreen(void) {
    system("cls");
}

void hideCursor(void) {
    CONSOLE_CURSOR_INFO info;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(h, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(h, &info);
}

void showCursor(void) {
    CONSOLE_CURSOR_INFO info;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(h, &info);
    info.bVisible = TRUE;
    SetConsoleCursorInfo(h, &info);
}

// ===== Menus & headers =====
void displayHeader(void) {
    printf(ANSI_BOLD ANSI_MAGENTA "              Education Reimagined              \n" ANSI_RESET);
    printf("                     Menu                       \n\n");
}

void displayMenuMain(int selected) {
    const char* options[] = {
        "Type Writer",
        "Flash Cards",
        "playGuessingGame",
        "Chatbot Questions",
        "Exit"
    };
    for (int i = 0; i < 5; ++i) {
        if (i == selected) printf(ANSI_GREEN "-> " ANSI_WHITE "%s" ANSI_RESET "\n", options[i]);
        else               printf(ANSI_CYAN  "   %s" ANSI_RESET "\n", options[i]);
    }
}

void displayHeaderFlash(void) {
    printf(ANSI_BOLD ANSI_MAGENTA "              Flash Card Education System              \n" ANSI_RESET "\n");
}

void displayMenuFlash(int selected) {
    const char* options[] = {
        "Add New Flash Card",
        "View All Questions",
        "Take Interactive Quiz",
        "View Statistics",
        "Back"
    };
    for (int i = 0; i < 5; ++i) {
        if (i == selected) printf(ANSI_GREEN "-> " ANSI_WHITE "%s" ANSI_RESET "\n", options[i]);
        else               printf(ANSI_CYAN  "   %s" ANSI_RESET "\n", options[i]);
    }
    printf("\n" ANSI_YELLOW "  Cards in deck: %d/%d\n" ANSI_RESET, totalCards, MAX_CARDS);
}

// ===== Flash Cards =====
void showAddCard(void) {
    showCursor();
    clearScreen();
    printf(ANSI_BOLD ANSI_CYAN "=== ADD NEW FLASH CARD ===\n\n" ANSI_RESET);
    if (totalCards >= MAX_CARDS) {
        printf(ANSI_RED "Flash card limit reached! (%d/%d)\n\n" ANSI_RESET, totalCards, MAX_CARDS);
        printf("Press any key to return to menu..."); _getch(); hideCursor(); return;
    }
    printf("Enter your question:\n" ANSI_CYAN "> " ANSI_RESET);
    fgets(deck[totalCards].question, MAX_LEN, stdin);
    deck[totalCards].question[strcspn(deck[totalCards].question, "\n")] = '\0';
    if (deck[totalCards].question[0] == '\0') { printf(ANSI_RED "\nQuestion cannot be empty!\n" ANSI_RESET); _getch(); hideCursor(); return; }

    printf("\nEnter the answer:\n" ANSI_CYAN "> " ANSI_RESET);
    fgets(deck[totalCards].answer, MAX_LEN, stdin);
    deck[totalCards].answer[strcspn(deck[totalCards].answer, "\n")] = '\0';
    if (deck[totalCards].answer[0] == '\0') { printf(ANSI_RED "\nAnswer cannot be empty!\n" ANSI_RESET); _getch(); hideCursor(); return; }

    totalCards++;
    printf(ANSI_GREEN "\n✓ Card added successfully!\n\n" ANSI_RESET);
    printf("Press any key to return to menu..."); _getch(); hideCursor();
}

void showViewCards(void) {
    clearScreen();
    printf(ANSI_BOLD ANSI_BLUE "=== YOUR FLASH CARDS ===\n\n" ANSI_RESET);
    if (totalCards == 0) {
        printf(ANSI_YELLOW "No flash cards available yet!\n\n" ANSI_RESET);
        printf("Press any key to return to menu..."); _getch(); return;
    }
    for (int i = 0; i < totalCards; ++i) {
        printf(ANSI_MAGENTA "Card #%d:\n" ANSI_RESET, i + 1);
        printf(ANSI_WHITE   "Q: %s\n" ANSI_RESET, deck[i].question);
        printf(ANSI_GREEN   "A: %s\n\n" ANSI_RESET, deck[i].answer);
        if ((i + 1) % 5 == 0 && i + 1 < totalCards) { printf("Press any key to see more..."); _getch(); printf("\n"); }
    }
    printf("Press any key to return to menu..."); _getch();
}

void showQuiz(void) {
    showCursor();
    clearScreen();
    printf(ANSI_BOLD ANSI_MAGENTA "=== INTERACTIVE QUIZ ===\n\n" ANSI_RESET);
    if (totalCards == 0) { printf(ANSI_YELLOW "No flash cards to quiz on!\n\n" ANSI_RESET); _getch(); hideCursor(); return; }

    int score = 0; char userAnswer[MAX_LEN];
    for (int i = 0; i < totalCards; ++i) {
        printf(ANSI_YELLOW "Question %d of %d:\n" ANSI_RESET "%s\n\n", i + 1, totalCards, deck[i].question);
        printf("Your answer: ");
        fgets(userAnswer, MAX_LEN, stdin);
        userAnswer[strcspn(userAnswer, "\n")] = '\0';
        if (strcmp(userAnswer, deck[i].answer) == 0) { printf(ANSI_GREEN "✓ Correct!\n\n" ANSI_RESET); score++; }
        else { printf(ANSI_RED "✗ Wrong!\n" ANSI_RESET ANSI_GREEN "Correct answer: %s\n\n" ANSI_RESET, deck[i].answer); }
        if (i < totalCards - 1) { printf("Press any key for next question..."); _getch(); printf("\n"); }
    }

    double pct = (double)score / totalCards * 100.0;
    printf("==============================\n" ANSI_BOLD ANSI_CYAN "QUIZ RESULTS\n" ANSI_RESET "==============================\n");
    printf("Your Score: " ANSI_WHITE ANSI_BOLD "%d/%d" ANSI_RESET " (%.1f%%)\n\n", score, totalCards, pct);
    printf("Press any key to return to menu..."); _getch(); hideCursor();
}

void showStats(void) {
    clearScreen();
    printf(ANSI_BOLD ANSI_GREEN "=== LEARNING STATISTICS ===\n\n" ANSI_RESET);
    if (totalCards == 0) { printf(ANSI_YELLOW "No statistics available yet!\n\n" ANSI_RESET); printf("Press any key to return..."); _getch(); return; }

    int tq = 0, ta = 0; for (int i = 0; i < totalCards; ++i) { tq += (int)strlen(deck[i].question); ta += (int)strlen(deck[i].answer); }
    printf(ANSI_CYAN "Learning Progress:\n" ANSI_RESET);
    printf("Total Cards: " ANSI_WHITE ANSI_BOLD "%d" ANSI_RESET " / %d\n", totalCards, MAX_CARDS);
    printf("Capacity Used: " ANSI_YELLOW "%.1f%%" ANSI_RESET "\n", (double)totalCards / MAX_CARDS * 100.0);
    printf("Avg Question Length: %d characters\n", tq / totalCards);
    printf("Avg Answer Length: %d characters\n\n", ta / totalCards);

    printf("Progress Bar: [");
    int bars = (totalCards * 25) / MAX_CARDS;
    for (int i = 0; i < 25; ++i) putchar(i < bars ? '#' : '-');
    printf("]\n\nPress any key to return to menu..."); _getch();
}

// ===== Typing test =====
void print_colored_char(char c, const char* color) { printf("%s%c%s", color, c, ANSI_RESET); }

void draw_text(char** words, int num_words, int current_word_index, const char* typed_word, double wpm, double accuracy, int time_left) {
    clearScreen();
    for (int i = 0; i < num_words; ++i) {
        if (i < current_word_index) {
            printf("%s%s %s", ANSI_GREEN, words[i], ANSI_RESET);
        } else if (i == current_word_index) {
            const char* current_word = words[i];
            int typed_len = (int)strlen(typed_word);
            int word_len  = (int)strlen(current_word);
            for (int j = 0; j < typed_len; ++j) {
                if (j < word_len && typed_word[j] == current_word[j]) print_colored_char(current_word[j], ANSI_GREEN);
                else print_colored_char(typed_word[j], ANSI_RED);
            }
            if (typed_len < word_len) printf("%s%s%s", ANSI_GRAY, current_word + typed_len, ANSI_RESET);
            printf(" ");
        } else {
            printf("%s%s %s", ANSI_GRAY, words[i], ANSI_RESET);
        }
    }
    printf("\n\n" ANSI_BOLD "WPM: %.2f | Accuracy: %.2f%% | Time: %ds" ANSI_RESET "\n\n", wpm, accuracy, time_left);
    printf(ANSI_BOLD "Start typing..." ANSI_RESET);
}

void showTypeWriter(void) {
    clearScreen();
    const char* word_bank[] = {
        "the","quick","brown","fox","jumps","over","lazy","dog",
        "programming","is","fun","keyboard","typing","speed","accuracy",
        "test","developer","console","interface","language","code",
        "compiler","library","function","variable","string","loop",
        "condition","statement","simple","example","application",
        "system","input","output","character","time","test"
    };
    int word_bank_size = sizeof(word_bank)/sizeof(word_bank[0]);

    srand((unsigned)time(NULL));
    int num_test_words = 50;
    char* test_words[50];
    for (int i = 0; i < num_test_words; ++i) test_words[i] = (char*)word_bank[rand() % word_bank_size];

    int current_word_index = 0; char typed_word[256] = ""; int typed_word_len = 0;
    int correct_chars = 0; int total_keystrokes = 0; int test_duration = 60; int started = 0;
    clock_t start_time = 0;

    draw_text(test_words, num_test_words, 0, typed_word, 0.0, 0.0, test_duration);

    while (1) {
        if (_kbhit()) {
            if (!started) { start_time = clock(); started = 1; }
            char ch = _getch();
            if (ch == 27) break; // ESC to quit
            if (ch == ' ' && typed_word_len > 0) {
                if (current_word_index < num_test_words) {
                    int word_len = (int)strlen(test_words[current_word_index]);
                    int len_to_compare = typed_word_len < word_len ? typed_word_len : word_len;
                    for (int i = 0; i < len_to_compare; ++i) if (typed_word[i] == test_words[current_word_index][i]) correct_chars++;
                    current_word_index++; typed_word_len = 0; typed_word[0] = '\0';
                }
            } else if (ch == 8) { // backspace
                if (typed_word_len > 0) { typed_word[--typed_word_len] = '\0'; }
            } else if (ch >= 32 && ch <= 126) {
                total_keystrokes++;
                if (typed_word_len < (int)sizeof(typed_word) - 1) { typed_word[typed_word_len++] = ch; typed_word[typed_word_len] = '\0'; }
            }
            if (current_word_index >= num_test_words) break;
            double elapsed = started ? (double)(clock() - start_time) / CLOCKS_PER_SEC : 0.0;
            if (elapsed > test_duration) break;
            double wpm = elapsed > 0 ? ((double)correct_chars / 5.0) * (60.0 / elapsed) : 0.0;
            double acc = total_keystrokes > 0 ? ((double)correct_chars / (double)total_keystrokes) * 100.0 : 0.0;
            draw_text(test_words, num_test_words, current_word_index, typed_word, wpm, acc, test_duration - (int)elapsed);
        }
    }

    double elapsed = started ? (double)(clock() - start_time) / CLOCKS_PER_SEC : 1.0;
    double final_wpm = ((double)correct_chars / 5.0) * (60.0 / elapsed);
    double final_acc = total_keystrokes > 0 ? ((double)correct_chars / (double)total_keystrokes) * 100.0 : 0.0;

    clearScreen();
    printf(ANSI_BOLD ANSI_GREEN "===============================================================================\n");
    printf("                                    Test Complete!                             \n");
    printf("===============================================================================\n" ANSI_RESET);
    printf(ANSI_BOLD "Final WPM: %.2f\n" ANSI_RESET, final_wpm);
    printf("Accuracy:  %.2f%%\n", final_acc);
    printf("Total Keystrokes: %d\n", total_keystrokes);
    printf("Correct Keystrokes: %d\n", correct_chars);
    printf("===============================================================================\n");
    printf("\nPress any key to return to menu..."); _getch();
}

// ===== playGuessingGame (Guessing Game) =====
void saveGameData(GameData data) {
    FILE *fp = fopen("word_game_data.bin", "wb");
    if (!fp) return;
    fwrite(&data, sizeof(GameData), 1, fp);
    fclose(fp);
}

GameData loadGameData(void) {
    GameData data; FILE *fp = fopen("word_game_data.bin", "rb");
    if (!fp) {
        srand((unsigned)time(NULL));
        const char *wordList[] = {"computer","programming","hangman","developer","keyboard","puzzle"};
        strcpy(data.target, wordList[rand() % 6]);
        data.gamesPlayed = 0; saveGameData(data);
    } else { fread(&data, sizeof(GameData), 1, fp); fclose(fp); }
    return data;
}

char getSingleChar(void) {
    char input[100];
    while (1) {
        printf("Enter a letter: ");
        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = '\0';
            if (strlen(input) == 1 && ((input[0] >= 'a' && input[0] <= 'z') || (input[0] >= 'A' && input[0] <= 'Z'))) {
                char ch = input[0]; if (ch >= 'A' && ch <= 'Z') ch = (char)(ch + 32); return ch;
            }
            printf("Invalid input! Please enter a single letter.\n");
        }
    }
}

void playGuessingGame(void) {
    clearScreen();
    printf(ANSI_BOLD ANSI_CYAN "=== WORD GUESSING GAME ===\n\n" ANSI_RESET);

    GameData game = loadGameData();
    int wordLen = (int)strlen(game.target);

    int choice = 0; int chances = 0;
    printf("Select difficulty:\n1. EASY (10 wrong guesses)\n2. MEDIUM (7 wrong guesses)\n3. HARD (5 wrong guesses)\nChoice: ");
    if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); choice = 1; }
    while (getchar() != '\n'); // flush

    if      (choice == 1) chances = 10;
    else if (choice == 2) chances = 7;
    else if (choice == 3) chances = 5;
    else chances = 10;

    char display[30]; for (int i = 0; i < wordLen; ++i) display[i] = '_'; display[wordLen] = '\0';
    int guessed[26] = {0};

    while (chances > 0) {
        printf("\n========================================\n");
        printf("Word: "); for (int i = 0; i < wordLen; ++i) printf("%c ", display[i]);
        printf("\nChances left: %d\n", chances);
        printf("Guessed letters: "); int any = 0; for (int i = 0; i < 26; ++i) if (guessed[i]) { putchar('a'+i); putchar(' '); any = 1; }
        if (!any) printf("(none)"); printf("\n\n");

        char guess = getSingleChar();
        if (guessed[guess-'a']) { printf("You already guessed '%c'!\n", guess); continue; }
        guessed[guess-'a'] = 1;

        int found = 0; for (int i = 0; i < wordLen; ++i) if (game.target[i] == guess) { display[i] = guess; found = 1; }
        if (found) printf("Good guess! '%c' is in the word.\n", guess);
        else { printf("Wrong guess! '%c' is not in the word.\n", guess); chances--; }

        if (strcmp(display, game.target) == 0) {
            printf("\nCongratulations! You guessed the word '%s'!\n", game.target);
            const char *wordList[] = {"computer","programming","hangman","developer","keyboard","puzzle"};
            strcpy(game.target, wordList[rand() % 6]);
            saveGameData(game);
            printf("\nPress any key to return to menu..."); _getch();
            return;
        }
    }

    printf("\nOut of chances! The word was '%s'.\n", game.target);
    game.gamesPlayed++; saveGameData(game);
    printf("Press any key to return to menu..."); _getch();
}

void showplayGuessingGame(void) {
    // Currently offers the Word Guessing Game
    playGuessingGame();
}

void showChatbot(void) {
    clearScreen();
    printf(ANSI_BOLD ANSI_CYAN "=== CHATBOT QUESTIONS FEATURE ===\n" ANSI_RESET);
    printf("This feature is coming soon!\n\nPress any key to return to menu..."); _getch();
}

void showFlash(void) {
    int selected = 0; const int numOptions = 5; int running = 1; hideCursor();
    while (running) {
        clearScreen();
        displayHeaderFlash();
        displayMenuFlash(selected);
        printf("\n" ANSI_YELLOW "Use arrows or WASD to navigate, ENTER to select, ESC to go back\n" ANSI_RESET);
        int key = _getch();
        switch (key) {
            case 72: case 'w': case 'W': selected = (selected - 1 + numOptions) % numOptions; break; // up
            case 80: case 's': case 'S': selected = (selected + 1) % numOptions; break; // down
            case 27: running = 0; break; // ESC
            case 13: // Enter
                if      (selected == 0) showAddCard();
                else if (selected == 1) showViewCards();
                else if (selected == 2) showQuiz();
                else if (selected == 3) showStats();
                else running = 0;
                break;
        }
    }
}

// ===== Entry point =====
int main(void) {
    enableVTMode();
    hideCursor();

    int selected = 0; const int numOptions = 5; int running = 1;
    while (running) {
        clearScreen();
        displayHeader();
        displayMenuMain(selected);
        printf("\n" ANSI_YELLOW "Use arrows or WASD to navigate, ENTER to select, ESC to exit\n" ANSI_RESET);
        int key = _getch();
        switch (key) {
            case 72: case 'w': case 'W': selected = (selected - 1 + numOptions) % numOptions; break;
            case 80: case 's': case 'S': selected = (selected + 1) % numOptions; break;
            case 27: running = 0; break;
            case 13:
                if      (selected == 0) showTypeWriter();
                else if (selected == 1) showFlash();
                else if (selected == 2) showplayGuessingGame();
                else if (selected == 3) showChatbot();
                else running = 0;
                break;
        }
    }

    showCursor();
    clearScreen();
    printf(ANSI_RED "Thanks for using our menu system!\n" ANSI_RESET);
    return 0;
}
