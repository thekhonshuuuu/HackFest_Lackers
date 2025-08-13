#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <time.h>
#include <conio.h>  
#include <windows.h> 

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

#define ANSI_RESET      "\x1b[0m"
#define ANSI_GREEN      "\x1b[32m"
#define ANSI_RED        "\x1b[31m"
#define ANSI_GRAY       "\x1b[90m"
#define ANSI_BOLD       "\x1b[1m"

#define MAX_CARDS 100
#define MAX_LEN 200

typedef struct {
    char question[MAX_LEN];
    char answer[MAX_LEN];
} FlashCard;

FlashCard deck[MAX_CARDS];
int totalCards = 0;

// Function prototypes
void clearScreen();
void setCursorPosition(int x, int y);
void hideCursor();
void showCursor();
void drawBox(int x, int y, int width, int height);
void displayHeader();
void displayMenu1(int selected);
void displayHeader1();
void displayMenu(int selected);
void getUser();
void showAddCard();
void showViewCards();
void showQuiz();
void showStats();
int showTypeWrite();
void showFlash();
void showSimulations();
void showChatbot();
void print_colored_char(char c, const char* color);
void draw_text(char** words, int num_words, int current_word_index, const char* typed_word, double wpm, double accuracy, int time_left);

void print_colored_char(char c, const char* color) {
    printf("%s%c%s", color, c, ANSI_RESET);
}

void draw_text(char** words, int num_words, int current_word_index, const char* typed_word, double wpm, double accuracy, int time_left) {
    clearScreen();
    for (int i = 0; i < num_words; ++i) {
        if (i < current_word_index) {
            printf("%s%s %s", ANSI_GREEN, words[i], ANSI_RESET);
        } else if (i == current_word_index) {
            const char* current_word = words[i];
            int typed_len = strlen(typed_word);
            int word_len = strlen(current_word);
            for (int j = 0; j < typed_len; ++j) {
                if (j < word_len && typed_word[j] == current_word[j]) {
                    print_colored_char(current_word[j], ANSI_GREEN);
                } else {
                    print_colored_char(typed_word[j], ANSI_RED);
                }
            }
            if (typed_len < word_len) {
                printf("%s%s%s", ANSI_GRAY, current_word + typed_len, ANSI_RESET);
            }
            printf(" ");
        } else {
            printf("%s%s %s", ANSI_GRAY, words[i], ANSI_RESET);
        }
    }
    printf("\n\n%sWPM: %.2f | Accuracy: %.2f%% | Time: %ds%s\n\n",
           ANSI_BOLD, wpm, accuracy, time_left, ANSI_RESET);
    printf("%sStart typing...%s", ANSI_BOLD, ANSI_RESET);
}

// Function to clear screen (cross-platform)
void clearScreen() {
    system("cls");
}

// Function to set cursor position
void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Function to hide cursor
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// Function to show cursor
void showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// Function to display menu header
void displayHeader() {
    printf(RESET);
    printf(BOLD MAGENTA "              Education Reimagined              \n" RESET);
    printf("                     Menu                       \n");
    printf("\n");
}

// Function to display menu options
void displayMenu1(int selected) {
    const char* options[] = {
        "Type Writer ",
        "Flash Cards", 
        "Simulations",
        "Chatbot Questions",
        "Exit"
    };
    int numOptions = 5;
    
    for(int i = 0; i < numOptions; i++) {
        if(i == selected) {
            printf(GREEN "->" WHITE "%s" RESET "\n", options[i]);
        } else {
            printf(CYAN "    %s" RESET "\n", options[i]);
        }
    }
}

// Function to display flash card menu header
void displayHeader1() {
    printf(RESET);
    printf(BOLD MAGENTA "              Flash Card Education System              \n" RESET);
    printf("\n");
}

// Function to display flash card menu options
void displayMenu(int selected) {
    const char* options[] = {
        "Add New Flash Card",
        "View All Questions", 
        "Take Interactive Quiz",
        "View Statistics",
        "Exit"
    };
    int numOptions = 5;
    
    for(int i = 0; i < numOptions; i++) {
        if(i == selected) {
            printf(GREEN "->" WHITE " %s" RESET "\n", options[i]);
        } else {
            printf(CYAN "    %s" RESET "\n", options[i]);
        }
    }
    
    // Show card count
    printf("\n");
    printf(YELLOW "  Cards in deck: %d/%d\n" RESET, totalCards, MAX_CARDS);
}

// Function to ask for user's name (first time only)
void getUser() {
    static int hasAsked = 0;
    if (hasAsked) return;
    
    showCursor();
    clearScreen();
    printf(CYAN BOLD "Welcome to Flash Card Learning System!\n" RESET);
    printf("Press any key to continue...");
    getch();
    hideCursor();
    hasAsked = 1;
}

// Menu option handlers
void showAddCard() {
    showCursor();
    clearScreen();
    
    printf(BOLD CYAN "=== ADD NEW FLASH CARD ===\n" RESET);
    printf("\n");
    
    if (totalCards >= MAX_CARDS) {
        printf(RED "Flash card limit reached! (%d/%d)\n" RESET, totalCards, MAX_CARDS);
        printf("You need to remove some cards before adding new ones.\n\n");
        printf("Press any key to return to menu...");
        getch();
        hideCursor();
        return;
    }
    
    printf(YELLOW "Let's create card %d\n" RESET, totalCards + 1);
    printf("----------------------------------------\n\n");
    
    printf("Enter your question:\n");
    printf(CYAN "> " RESET);
    fgets(deck[totalCards].question, MAX_LEN, stdin);
    deck[totalCards].question[strcspn(deck[totalCards].question, "\n")] = '\0';
    
    if(strlen(deck[totalCards].question) == 0) {
        printf(RED "\nError: Question cannot be empty!\n" RESET);
        printf("Press any key to try again...");
        getch();
        hideCursor();
        return;
    }
    
    printf("\nEnter the answer:\n");
    printf(CYAN "> " RESET);
    fgets(deck[totalCards].answer, MAX_LEN, stdin);
    deck[totalCards].answer[strcspn(deck[totalCards].answer, "\n")] = '\0';
    
    if(strlen(deck[totalCards].answer) == 0) {
        printf(RED "\nError: Answer cannot be empty!\n" RESET);
        printf("Press any key to try again...");
        getch();
        hideCursor();
        return;
    }
    
    totalCards++;
    
    printf(GREEN "\n? Card added successfully!\n" RESET);
    printf("You now have %d flash card(s) in your deck.\n\n", totalCards);
    
    if(totalCards == 1) {
        printf(YELLOW "Congratulations on your first flash card! :)\n" RESET);
    }
    
    printf("Press any key to return to menu...");
    getch();
    hideCursor();
}

void showViewCards() {
    clearScreen();
    
    printf(BOLD BLUE "=== YOUR FLASH CARDS ===\n" RESET);
    printf("\n");
    
    if (totalCards == 0) {
        printf(YELLOW "No flash cards available yet!\n" RESET);
        printf("Create your first flash card to get started.\n\n");
        printf("Press any key to return to menu...");
        getch();
        return;
    }
    
    printf(CYAN "Total Cards: %d\n" RESET, totalCards);
    printf("=========================================\n\n");
    
    for (int i = 0; i < totalCards; i++) {
        printf(MAGENTA "Card #%d:\n" RESET, i + 1);
        printf(WHITE "Q: %s\n" RESET, deck[i].question);
        printf(GREEN "A: %s\n" RESET, deck[i].answer);
        printf("\n");
        
        if((i + 1) % 5 == 0 && i + 1 < totalCards) {
            printf("Press any key to see more cards...");
            getch();
            printf("\n");
        }
    }
    
    printf("Press any key to return to menu...");
    getch();
}

void showQuiz() {
    showCursor();
    clearScreen();
    
    printf(BOLD MAGENTA "=== INTERACTIVE QUIZ ===\n" RESET);
    printf("\n");
    
    if (totalCards == 0) {
        printf(YELLOW "No flash cards to quiz on!\n" RESET);
        printf("Add some flash cards first to start learning.\n\n");
        printf("Press any key to return to menu...");
        getch();
        hideCursor();
        return;
    }
    
    int score = 0;
    char userAnswer[MAX_LEN];
    
    printf(CYAN "Starting quiz with %d questions...\n" RESET, totalCards);
    printf("Good luck :)\n");
    printf("=========================================\n\n");
    
    for (int i = 0; i < totalCards; i++) {
        printf(YELLOW "Question %d of %d:\n" RESET, i + 1, totalCards);
        printf("%s\n\n", deck[i].question);
        printf("Your answer: ");
        
        fgets(userAnswer, MAX_LEN, stdin);
        userAnswer[strcspn(userAnswer, "\n")] = '\0';
        
        if (strcmp(userAnswer, deck[i].answer) == 0) {
            printf(GREEN "? Correct!\n" RESET);
            score++;
        } else {
            printf(RED "? Wrong!\n" RESET);
            printf(GREEN "Correct answer: %s\n" RESET, deck[i].answer);
        }
        
        printf("\n");
        if(i < totalCards - 1) {
            printf("Press any key for next question...");
            getch();
            printf("\n");
        }
    }
    
    // Quiz Results
    printf("=========================================\n");
    printf(BOLD CYAN "QUIZ RESULTS\n" RESET);
    printf("=========================================\n");
    
    double percentage = (double)score / totalCards * 100;
    
    printf("Your Score: " WHITE BOLD "%d/%d" RESET " (%.1f%%)\n\n", score, totalCards, percentage);
    
    if(percentage >= 90) {
        printf(GREEN "Excellent! Outstanding work! :D\n" RESET);
    } else if(percentage >= 70) {
        printf(YELLOW "Great job! Keep it up! :)\n" RESET);
    } else if(percentage >= 50) {
        printf(YELLOW "Good effort! Practice makes perfect!\n" RESET);
    } else {
        printf(CYAN "Keep studying! You'll improve! :)\n" RESET);
    }
    
    printf("\nPress any key to return to menu...");
    getch();
    hideCursor();
}

void showStats() {
    clearScreen();
    
    printf(BOLD GREEN "=== LEARNING STATISTICS ===\n" RESET);
    printf("\n");
    
    if(totalCards == 0) {
        printf(YELLOW "No statistics available yet!\n" RESET);
        printf("Create some flash cards to see your progress.\n\n");
    } else {
        int totalQuestionLen = 0, totalAnswerLen = 0;
        for(int i = 0; i < totalCards; i++) {
            totalQuestionLen += strlen(deck[i].question);
            totalAnswerLen += strlen(deck[i].answer);
        }
        
        printf(CYAN "Learning Progress:\n" RESET);
        printf("-------------------------------\n");
        printf("Total Cards: " WHITE BOLD "%d" RESET " / %d\n", totalCards, MAX_CARDS);
        printf("Capacity Used: " YELLOW "%.1f%%" RESET "\n", (double)totalCards / MAX_CARDS * 100);
        printf("Avg Question Length: %d characters\n", totalQuestionLen / totalCards);
        printf("Avg Answer Length: %d characters\n", totalAnswerLen / totalCards);
        
        printf("\nProgress Bar: [");
        int bars = (totalCards * 25) / MAX_CARDS;
        for(int i = 0; i < 25; i++) {
            if(i < bars) {
                printf(GREEN "#" RESET);
            } else {
                printf("-");
            }
        }
        printf("]\n");
        
        if(totalCards >= 20) {
            printf(GREEN "\nWow! You're becoming a flash card master! :D\n" RESET);
        } else if(totalCards >= 10) {
            printf(YELLOW "\nGreat progress! Double digits! :)\n" RESET);
        } else if(totalCards >= 5) {
            printf(CYAN "\nNice start! Keep building your deck! :)\n" RESET);
        }
    }
    
    printf("\nPress any key to return to menu...");
    getch();
}

// Menu option handlers for main menu
int showTypeWrite() {
    clearScreen();
    const char* word_bank[] = {
        "the", "quick", "brown", "fox", "jumps", "over", "lazy", "dog",
        "programming", "is", "fun", "keyboard", "typing", "speed", "accuracy",
        "test", "developer", "console", "interface", "language", "code",
        "compiler", "library", "function", "variable", "string", "loop",
        "condition", "statement", "simple", "example", "application",
        "system", "input", "output", "character", "time", "test"
    };
    int word_bank_size = sizeof(word_bank) / sizeof(word_bank[0]);
    srand(time(NULL));
    int num_test_words = 50;
    char* test_words[num_test_words];
    for (int i = 0; i < num_test_words; ++i) {
        test_words[i] = (char*)word_bank[rand() % word_bank_size];
    }
    int current_word_index = 0;
    char typed_word[256] = "";
    int typed_word_len = 0;
    int correct_chars = 0;
    int total_keystrokes = 0;
    clock_t start_time;
    int test_duration = 60;
    draw_text(test_words, num_test_words, 0, typed_word, 0.0, 0.0, test_duration);
    int started = 0;
    while (1) {
        if (_kbhit()) {
            if (!started) {
                start_time = clock();
                started = 1;
            }
            char ch = _getch();
            if (ch == 27) {
                break;
            }
            if (ch == ' ' && typed_word_len > 0) {
                if (current_word_index < num_test_words) {
                    int word_len = strlen(test_words[current_word_index]);
                    int len_to_compare = typed_word_len < word_len ? typed_word_len : word_len;
                    for (int i = 0; i < len_to_compare; ++i) {
                        if (typed_word[i] == test_words[current_word_index][i]) {
                            correct_chars++;
                        }
                    }
                    current_word_index++;
                    typed_word_len = 0;
                    typed_word[0] = '\0';
                }
            } else if (ch == 8) {
                if (typed_word_len > 0) {
                    typed_word_len--;
                    typed_word[typed_word_len] = '\0';
                }
            } else if (ch >= 32 && ch <= 126) {
                total_keystrokes++;
                if (typed_word_len < sizeof(typed_word) - 1) {
                    typed_word[typed_word_len] = ch;
                    typed_word_len++;
                    typed_word[typed_word_len] = '\0';
                }
            }
            if (current_word_index >= num_test_words) {
                break;
            }
            double elapsed_time_seconds = (double)(clock() - start_time) / CLOCKS_PER_SEC;
            if (elapsed_time_seconds > test_duration) {
                break;
            }
            double wpm = 0.0;
            double accuracy = 0.0;
            if (elapsed_time_seconds > 0) {
                wpm = ((double)correct_chars / 5.0) * (60.0 / elapsed_time_seconds);
            }
            if (total_keystrokes > 0) {
                accuracy = ((double)correct_chars / (double)total_keystrokes) * 100.0;
            }
            draw_text(test_words, num_test_words, current_word_index, typed_word, wpm, accuracy, test_duration - (int)elapsed_time_seconds);
        }
        if (started && (double)(clock() - start_time) / CLOCKS_PER_SEC > test_duration) {
            break;
        }
    }
    double final_elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    double final_wpm = 0.0;
    if (final_elapsed_time > 0) {
        final_wpm = ((double)correct_chars / 5.0) * (60.0 / final_elapsed_time);
    }
    double final_accuracy = 0.0;
    if (total_keystrokes > 0) {
        final_accuracy = ((double)correct_chars / (double)total_keystrokes) * 100.0;
    }
    clearScreen();
    printf("%s%s===============================================================================\n", ANSI_BOLD, ANSI_GREEN);
    printf("                                    Test Complete!                             \n");
    printf("===============================================================================\n%s", ANSI_RESET);
    printf("%sFinal WPM: %.2f\n", ANSI_BOLD, final_wpm);
    printf("Accuracy:  %.2f%%\n", final_accuracy);
    printf("Total Keystrokes: %d\n", total_keystrokes);
    printf("Correct Keystrokes: %d\n", correct_chars);
    printf("===============================================================================\n%s", ANSI_RESET);
    printf("\nPress any key to return to menu...");
    getch();
    return 0;
}

void showFlash() {
    clearScreen();
    
    srand(time(NULL));
    int selected = 0;
    int numOptions = 5;
    char key;
    
    hideCursor();
    
    while(1) {
        clearScreen();
        displayHeader1();
        displayMenu(selected);
        
        printf(YELLOW "\n  Use arrows or WASD to navigate, ENTER to select, ESC to exit\n" RESET);
        
        key = getch();
        
        switch(key) {
            case 72: // Up arrow (Windows)
            case 'w':
            case 'W':
                selected = (selected - 1 + numOptions) % numOptions;
                break;
                
            case 80: // Down arrow (Windows) 
            case 's':
            case 'S':
                selected = (selected + 1) % numOptions;
                break;
                
            case 13: // Enter
                switch(selected) {
                    case 0: showAddCard(); break;
                    case 1: showViewCards(); break;
                    case 2: showQuiz(); break;
                    case 3: showStats(); break;
                    case 4:
                        showCursor();    
                        clearScreen();
                        printf(RED "Thanks for learning with us!\n" RESET);
                        printf(GREEN "Keep being curious and keep learning! :)\n" RESET);
                        return;
                }
                break;
                
            case 27: // ESC
                showCursor();
                clearScreen();
                printf(RED "Goodbye!\n" RESET);
                return;
        }
    }
}

void showSimulations() {
    clearScreen();
    printf(BOLD CYAN "=== SIMULATIONS FEATURE ===\n" RESET);
    printf("This feature is coming soon!\n\n");
    printf("Press any key to return to menu...");
    getch();
}

void showChatbot() {
    clearScreen();
    printf(BOLD CYAN "=== CHATBOT QUESTIONS FEATURE ===\n" RESET);
    printf("This feature is coming soon!\n\n");
    printf("Press any key to return to menu...");
    getch();
}

// Main function
int main() {
    int selected = 0;
    int numOptions = 5;
    char key;
    
    hideCursor();
    
    while(1) {
        clearScreen();
        displayHeader();
        displayMenu1(selected);
        
        printf(YELLOW "\n  Use arrows or WASD to navigate, ENTER to select, ESC to exit\n" RESET);
        
        key = getch();
        
        switch(key) {
            case 72: // Up arrow (Windows)
            case 'w':
            case 'W':
                selected = (selected - 1 + numOptions) % numOptions;
                break;
                
            case 80: // Down arrow (Windows) 
            case 's':
            case 'S':
                selected = (selected + 1) % numOptions;
                break;
                
            case 13: // Enter
                switch(selected) {
                    case 0: showTypeWrite(); break;
                    case 1: showFlash(); break;
                    case 2: showSimulations(); break;
                    case 3: showChatbot(); break;
                    case 4:
                        showCursor();
                        clearScreen();
                        printf(RED "Thanks for using our menu system!\n" RESET);
                        return 0;
                }
                break;
                
            case 27: // ESC
                showCursor();
                clearScreen();
                printf(RED "Goodbye!\n" RESET);
                return 0;
        }
    }
    
    return 0;
}