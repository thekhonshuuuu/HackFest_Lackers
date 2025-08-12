#include <stdio.h>
#include <stdlib.h>
#include <conio.h>  // For getch() - Windows specific
#include <windows.h> // For Windows console manipulation

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




//fucntion for the main system
// Function to clear screen (cross-platform)
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Function to set cursor position
void setCursorPosition(int x, int y) {
    #ifdef _WIN32
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    #else
        printf("\033[%d;%dH", y + 1, x + 1);
    #endif
}

// Function to hide cursor
void hideCursor() {
    #ifdef _WIN32
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    #else
        printf("\033[?25l");
    #endif
}

// Function to show cursor
void showCursor() {
    #ifdef _WIN32
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    #else
        printf("\033[?25h");
    #endif
}

// Function to draw a box
void drawBox(int x, int y, int width, int height) {
    setCursorPosition(x, y);
    printf("+");
    for(int i = 0; i < width - 2; i++) printf("-");
    printf("+");
    
    for(int i = 1; i < height - 1; i++) {
        setCursorPosition(x, y + i);
        printf("¦");
        setCursorPosition(x + width - 1, y + i);
        printf("¦");
    }
    
    setCursorPosition(x, y + height - 1);
    printf("+");
    for(int i = 0; i < width - 2; i++) printf("-");
    printf("+");
}

// Function to display menu header
void displayHeader() {
    printf(RESET);
    printf(BOLD MAGENTA 	"              Education Reimagined              ""\n" RESET);
    printf("                     Menu              ""\n" RESET);
    printf("\n");
}

// Function to display menu options
void displayMenu(int selected) {
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


// Menu option handlers
void showTypeWrite() {
    clearScreen();
    printf("Press any key to return to menu...");
    getch();
}

void showFlash() {
	clearScreen();
}

void showSimulations() {
	clearScreen();
    printf("Press any key to return to menu...");
    getch();
}

void showChatbot() {
    clearScreen();
    printf("Press any key to return to menu...");
    getch();
}
//end of fucntions for the main menu 

//fucntions for flash cards 


// Function to display menu header

int main() {
    int selected = 0;
    int numOptions = 5;
    char key;
    
    hideCursor();
    
    while(1) {
        clearScreen();
        displayHeader();
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
                    case 0: showTypeWrite(); break;
                    case 1: showFlash(); break;
                    case 2: showSimulations(); break;
                    case 3: showChatbot(); break;
                    case 4:
                        showCursor();
                        clearScreen();
                        printf(RED " Thanks for using our menu system!\n" RESET);
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