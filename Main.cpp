
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
#define BOLD   "\033[1m"

#define MAX_CARDS 100
#define MAX_LEN 200

typedef struct {
    char question[MAX_LEN];
    char answer[MAX_LEN];
} FlashCard;

FlashCard deck[MAX_CARDS];
int totalCards = 0;

// Function prototypes (added missing declarations)
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
void showTypeWrite();
void showFlash();
void showSimulations();
void showChatbot();

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
    
    printf(YELLOW "Let's create card %d\n" RESET, totalCards + 1); // Fixed printf
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
        printf(GREEN "Excellent! Outstanding work ! :D\n" RESET);
    } else if(percentage >= 70) {
        printf(YELLOW "Great job! Keep it up,! :)\n" RESET );
    } else if(percentage >= 50) {
        printf(YELLOW "Good effort! Practice makes perfect!\n" RESET);
    } else {
        printf(CYAN "Keep studying, %s! You'll improve! :)\n" RESET);
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
        printf(YELLOW "No statistics available yet !\n" RESET);
        printf("Create some flash cards to see your progress.\n\n");
    } else {
        int totalQuestionLen = 0, totalAnswerLen = 0;
        for(int i = 0; i < totalCards; i++) {
            totalQuestionLen += strlen(deck[i].question);
            totalAnswerLen += strlen(deck[i].answer);
        }
        
        printf(CYAN "%s's Learning Progress:\n" RESET);
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
void showTypeWrite() {
    clearScreen();
    printf(BOLD CYAN "=== TYPE WRITER FEATURE ===\n" RESET);
    printf("This feature is coming soon!\n\n");
    printf("Press any key to return to menu...");
    getch();
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
        displayHeader1();  // Fixed function call
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
                        printf(RED "Thanks for learning with us !\n" RESET);
                        printf(GREEN "Keep being curious and keep learning! :)\n" RESET);
                        return; // Fixed return statement
                }
                break;
                
            case 27: // ESC
                showCursor();
                clearScreen();
                printf(RED "Goodbye !\n" RESET);
                return; // Fixed return statement
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
        displayMenu1(selected);  // Fixed function call
        
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
    
    
    
    a
    return 0;
}