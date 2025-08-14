#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

/* ===== ANSI / UI ===== */
#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"
#define BOLD    "\x1b[1m"

#define ANSI_RESET  "\x1b[0m"
#define ANSI_GREEN  "\x1b[32m"
#define ANSI_RED    "\x1b[31m"
#define ANSI_GRAY   "\x1b[90m"
#define ANSI_BOLD   "\x1b[1m"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif

#define SIZE 9
#define MAX_MISTAKES 10

#define MENU_W 24
#define MAX_CARDS 100
#define MAX_LEN  200


typedef struct {
    char target[30];
    int  gamesPlayed;
} GameData;

/* ===== Data ===== */
typedef struct { char question[MAX_LEN]; char answer[MAX_LEN]; } FlashCard;
static FlashCard deck[MAX_CARDS];
static int totalCards = 0;

/* ===== Layout helpers ===== */
typedef struct { int x, y, w, h; } Rect;

static void fastClear(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi; COORD origin = {0,0};
    GetConsoleScreenBufferInfo(h, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y, written;
    FillConsoleOutputCharacter(h, ' ', cells, origin, &written);
    FillConsoleOutputAttribute(h, csbi.wAttributes, cells, origin, &written);
    SetConsoleCursorPosition(h, origin);
}
static void setCursor(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
static void hideCursor(void) {
    CONSOLE_CURSOR_INFO ci; ci.dwSize = 25; ci.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}
static void showCursor(void) {
    CONSOLE_CURSOR_INFO ci; ci.dwSize = 25; ci.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}
static void getConsoleSize(int* cols, int* rows) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
static void drawBox(Rect r) {
    for (int i=0;i<r.w;i++) { setCursor(r.x+i, r.y);       putchar(i==0?'+':(i==r.w-1?'+':'-')); }
    for (int i=0;i<r.w;i++) { setCursor(r.x+i, r.y+r.h-1); putchar(i==0?'+':(i==r.w-1?'+':'-')); }
    for (int j=1;j<r.h-1;j++) {
        setCursor(r.x,           r.y+j); putchar('|');
        setCursor(r.x+r.w-1,     r.y+j); putchar('|');
    }
}
static void clearContent(Rect r) {
    for (int row=1; row<r.h-1; ++row) {
        setCursor(r.x+1, r.y+row);
        for (int col=0; col<r.w-2; ++col) putchar(' ');
    }
}
static void printIn(Rect r, int px, int py, const char* s) {
    int x = r.x + 1 + (px<0?0:px);
    int y = r.y + 1 + (py<0?0:py);
    if (x > r.x + r.w - 2) x = r.x + r.w - 2;
    if (y > r.y + r.h - 2) y = r.y + r.h - 2;
    setCursor(x, y);
    fputs(s, stdout);
}

/* ===== Chrome ===== */
static void drawAppChrome(Rect* outSidebar, Rect* outContent) {
    int C,R; getConsoleSize(&C,&R);
    fastClear();

    // Header bar
    printf(BOLD MAGENTA);
    setCursor(0,0);
    for (int i=0;i<C;i++) putchar(' ');
    setCursor((C-24)/2, 0); printf("Edutainment");
    printf(RESET);

    Rect sidebar = (Rect){0,1,MENU_W,R-1};
    Rect content = (Rect){MENU_W,1,C-MENU_W,R-1};
    drawBox(sidebar);
    drawBox(content);
    printIn(sidebar,1,0,BOLD " Menu" RESET);

    if (outSidebar) *outSidebar = sidebar;
    if (outContent) *outContent = content;
}
static void drawSidebar(Rect sidebar, int selected) {
    // Added two options: Algorithm Mode, Redirect System (both "Coming Soon")
    const char* opts[] = {
        "Type Writer",
        "Flash Cards",
        "Word Guessing",
        "Sudoku",
        "Algorithm Mode ",
        "Simulation",
        "Exit"
    };
    int n = (int)(sizeof(opts)/sizeof(opts[0]));
    for (int i=0;i<n;i++) {
        setCursor(sidebar.x+2, sidebar.y+2+i);
        if (i==selected) printf(GREEN "-> " WHITE "%-16s" RESET, opts[i]);
        else             printf(CYAN  "   %-16s" RESET, opts[i]);
    }
    setCursor(sidebar.x+2, sidebar.y+sidebar.h-2);
    printf(YELLOW "W/S or \x18\x19, Enter=open, ESC=quit" RESET);
}

/* ===== Typewriter pane ===== */
static void print_colored_char(char c, const char* color) { printf("%s%c%s", color, c, ANSI_RESET); }

static void type_draw(Rect c, char** words, int N, int idx, const char* typed, double wpm, double acc, int tleft) {
    clearContent(c);
    setCursor(c.x+2, c.y+1);
    printf(BOLD "Type Writer — " RESET "WPM: %.1f | Acc: %.1f%% | Time: %ds", wpm, acc, tleft);

    int x=0, y=3, maxw = c.w - 4;
    for (int i=0;i<N;i++) {
        const char* w = words[i]; int wl = (int)strlen(w) + 1;
        if (x + wl > maxw) { x = 0; y += 1; }
        if (y >= c.h-3) break;
        setCursor(c.x+2+x, c.y+y);
        if (i < idx) {
            printf(ANSI_GREEN "%s " ANSI_RESET, w);
        } else if (i == idx) {
            int tl=(int)strlen(typed), wl2=(int)strlen(w);
            for (int j=0;j<tl;j++) {
                if (j<wl2 && typed[j]==w[j]) print_colored_char(w[j], ANSI_GREEN);
                else                          print_colored_char(typed[j], ANSI_RED);
            }
            if (tl<wl2) printf(ANSI_GRAY "%s" ANSI_RESET, w+tl);
            putchar(' ');
        } else {
            printf(ANSI_GRAY "%s " ANSI_RESET, w);
        }
        x += wl;
    }
    setCursor(c.x+2, c.y+c.h-3); printf(ANSI_BOLD "Start typing…  ESC to return" ANSI_RESET);
}
static void showTypeWriterPane(Rect c) {
    srand((unsigned)time(NULL));
    const char* bank[] = {
        "the","quick","brown","fox","jumps","over","lazy","dog","programming","is","fun",
        "keyboard","typing","speed","accuracy","test","developer","console","interface",
        "language","code","compiler","library","function","variable","string","loop",
        "condition","statement","simple","example","application","system","input","output",
        "character","time","test"
    };
    int bank_sz = (int)(sizeof(bank)/sizeof(bank[0]));
    int N = 50; char* words[50];
    for (int i=0;i<N;i++) words[i] = (char*)bank[rand()%bank_sz];

    int idx=0, typed_len=0, correct=0, keys=0, duration=60, started=0; clock_t start=0;
    char typed[256] = "";
    type_draw(c, words, N, idx, typed, 0, 0, duration);

    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch==27) break;
            if (!started) { started=1; start = clock(); }

            if (ch==' ' && typed_len>0) {
                int wl=(int)strlen(words[idx]);
                int cmplen = (typed_len<wl?typed_len:wl);
                for (int i=0;i<cmplen;i++) if (typed[i]==words[idx][i]) correct++;
                idx++; typed_len=0; typed[0]='\0'; if (idx>=N) break;
            } else if (ch==8) {
                if (typed_len>0) typed[--typed_len]='\0';
            } else if (ch>=32 && ch<=126) {
                keys++;
                if (typed_len < (int)sizeof(typed)-1) { typed[typed_len++]=ch; typed[typed_len]='\0'; }
            }
            double elapsed = started? (double)(clock()-start)/CLOCKS_PER_SEC : 0.0;
            if (elapsed>duration) break;
            double wpm = (elapsed>0)? ((double)correct/5.0)*(60.0/elapsed) : 0.0;
            double acc = (keys>0)? ((double)correct/(double)keys)*100.0 : 0.0;
            type_draw(c, words, N, idx, typed, wpm, acc, duration-(int)elapsed);
        }
    }
    clearContent(c);
    setCursor(c.x+2, c.y+2); printf(BOLD GREEN "Test Complete!" RESET);
    double elapsed = started? (double)(clock()-start)/CLOCKS_PER_SEC : 1.0;
    double final_wpm = ((double)correct/5.0)*(60.0/elapsed);
    double final_acc = (keys>0)? ((double)correct/(double)keys)*100.0 : 0.0;
    setCursor(c.x+2, c.y+4); printf("Final WPM: %.2f", final_wpm);
    setCursor(c.x+2, c.y+5); printf("Accuracy : %.2f%%", final_acc);
    setCursor(c.x+2, c.y+7); printf("Press any key to return...");
    getch();
}

/* ===== Flash Cards (pane) ===== */
static void flashAddPane(Rect c) {
    showCursor();
    clearContent(c);
    printIn(c,0,0,BOLD CYAN "Add New Flash Card" RESET);
    if (totalCards >= MAX_CARDS) {
        printIn(c,0,2,RED "Deck full. Press any key..." RESET);
        getch(); hideCursor(); return;
    }
    setCursor(c.x+2, c.y+3);  printf("Question: ");
    setCursor(c.x+12,c.y+3);  fflush(stdout);
    fgets(deck[totalCards].question, MAX_LEN, stdin);
    deck[totalCards].question[strcspn(deck[totalCards].question,"\n")] = 0;
    if (!deck[totalCards].question[0]) { setCursor(c.x+2,c.y+5); printf(RED "Question empty." RESET); getch(); hideCursor(); return; }

    setCursor(c.x+2, c.y+5);  printf("Answer  : ");
    setCursor(c.x+12,c.y+5);  fflush(stdout);
    fgets(deck[totalCards].answer, MAX_LEN, stdin);
    deck[totalCards].answer[strcspn(deck[totalCards].answer,"\n")] = 0;
    if (!deck[totalCards].answer[0]) { setCursor(c.x+2,c.y+7); printf(RED "Answer empty." RESET); getch(); hideCursor(); return; }

    totalCards++;
    setCursor(c.x+2, c.y+7); printf(GREEN "Card added! (%d/%d)" RESET, totalCards, MAX_CARDS);
    setCursor(c.x+2, c.y+9); printf("Press any key to return...");
    getch(); hideCursor();
}
static void flashViewPane(Rect c) {
    clearContent(c);
    printIn(c,0,0,BOLD BLUE "Your Flash Cards" RESET);
    if (totalCards==0) {
        printIn(c,0,2,YELLOW "No cards yet. Add some!" RESET);
        setCursor(c.x+2,c.y+4); printf("Press any key...");
        getch(); return;
    }
    int row = 2;
    for (int i=0;i<totalCards;i++) {
        if (c.y+1+row+3 >= c.y+c.h-2) { setCursor(c.x+2,c.y+c.h-3); printf("More... press any key"); getch(); clearContent(c); row = 2; }
        setCursor(c.x+2,c.y+1+row++); printf(MAGENTA "Card #%d" RESET, i+1);
        setCursor(c.x+2,c.y+1+row++); printf(WHITE   "Q: %s" RESET, deck[i].question);
        setCursor(c.x+2,c.y+1+row++); printf(GREEN   "A: %s" RESET, deck[i].answer);
        row++;
    }
    setCursor(c.x+2,c.y+c.h-3); printf("Press any key to return...");
    getch();
}
static void flashQuizPane(Rect c) {
    showCursor();
    clearContent(c);
    printIn(c,0,0,BOLD MAGENTA "Interactive Quiz" RESET);
    if (totalCards==0) { printIn(c,0,2,YELLOW "No cards to quiz." RESET); setCursor(c.x+2,c.y+4); printf("Press any key..."); getch(); hideCursor(); return; }

    int score=0; char ans[MAX_LEN];
    for (int i=0;i<totalCards;i++) {
        clearContent(c);
        setCursor(c.x+2,c.y+2); printf(YELLOW "Question %d/%d" RESET, i+1,totalCards);
        setCursor(c.x+2,c.y+4); printf("%s", deck[i].question);
        setCursor(c.x+2,c.y+6); printf("Your answer: ");
        setCursor(c.x+16,c.y+6); fflush(stdout);
        fgets(ans, MAX_LEN, stdin); ans[strcspn(ans,"\n")] = 0;

        if (strcmp(ans, deck[i].answer)==0) { setCursor(c.x+2,c.y+8); printf(GREEN "Correct!" RESET); score++; }
        else { setCursor(c.x+2,c.y+8); printf(RED "Wrong." RESET); setCursor(c.x+2,c.y+9); printf(GREEN "Ans: %s" RESET, deck[i].answer); }

        if (i < totalCards-1) { setCursor(c.x+2,c.y+c.h-3); printf("Press any key for next..."); getch(); }
    }
    double pct = (double)score / totalCards * 100.0;
    clearContent(c);
    setCursor(c.x+2,c.y+2); printf(BOLD CYAN "Quiz Results" RESET);
    setCursor(c.x+2,c.y+4); printf("Score: " WHITE BOLD "%d/%d" RESET " (%.1f%%)", score, totalCards, pct);
    setCursor(c.x+2,c.y+6);
    if (pct>=90) printf(GREEN "Excellent! :D" RESET);
    else if (pct>=70) printf(YELLOW "Great job! :)" RESET);
    else if (pct>=50) printf(YELLOW "Good effort!" RESET);
    else printf(CYAN "Keep practicing!" RESET);
    setCursor(c.x+2,c.y+8); printf("Press any key to return...");
    getch(); hideCursor();
}
static void flashStatsPane(Rect c) {
    clearContent(c);
    printIn(c,0,0,BOLD GREEN "Learning Statistics" RESET);
    if (totalCards==0) { printIn(c,0,2,YELLOW "No statistics yet." RESET); setCursor(c.x+2,c.y+4); printf("Press any key..."); getch(); return; }
    int totQ=0, totA=0; for (int i=0;i<totalCards;i++){ totQ+=(int)strlen(deck[i].question); totA+=(int)strlen(deck[i].answer);}
    double used = (double)totalCards / MAX_CARDS * 100.0;
    setCursor(c.x+2,c.y+2); printf("Total Cards: " WHITE BOLD "%d" RESET " / %d", totalCards, MAX_CARDS);
    setCursor(c.x+2,c.y+3); printf("Capacity   : %.1f%%", used);
    setCursor(c.x+2,c.y+4); printf("Avg Q len  : %d", totQ/totalCards);
    setCursor(c.x+2,c.y+5); printf("Avg A len  : %d", totA/totalCards);

    setCursor(c.x+2,c.y+7); printf("Progress: [");
    int bars = (totalCards * 25) / MAX_CARDS;
    for (int i=0;i<25;i++) putchar(i<bars ? '#' : '-');
    printf("]");
    if (totalCards>=20) { setCursor(c.x+2,c.y+9); printf(GREEN "Master in the making!" RESET); }
    else if (totalCards>=10){ setCursor(c.x+2,c.y+9); printf(YELLOW "Great progress!" RESET); }
    else if (totalCards>=5) { setCursor(c.x+2,c.y+9); printf(CYAN "Nice start!" RESET); }
    setCursor(c.x+2,c.y+c.h-3); printf("Press any key to return...");
    getch();
}
static void showFlashPane(Rect c) {
    const char* ops[] = {"Add Card","View Cards","Take Quiz","View Stats","Back"};
    int sel=0, n=5;
    while (1) {
        clearContent(c);
        printIn(c,0,0,BOLD MAGENTA "Flash Card Education System" RESET);
        setCursor(c.x+2, c.y+2); printf(YELLOW "Use W/S or \x18/\x19, Enter=select, ESC=back" RESET);
        for (int i=0;i<n;i++) {
            setCursor(c.x+2, c.y+4+i);
            if (i==sel) printf(GREEN "-> " WHITE "%s" RESET, ops[i]);
            else         printf(CYAN  "   %s" RESET, ops[i]);
        }
        int ch = getch();
        if (ch==27) return;
        else if (ch==72 || ch=='w' || ch=='W') sel = (sel-1+n)%n;
        else if (ch==80 || ch=='s' || ch=='S') sel = (sel+1)%n;
        else if (ch==13) {
            switch (sel) { case 0: flashAddPane(c); break; case 1: flashViewPane(c); break;
                           case 2: flashQuizPane(c); break; case 3: flashStatsPane(c); break;
                           case 4: return; }
        }
    }
}

/* ===== Word Guessing (pane) ===== */
static void gg_save(GameData data) {
    FILE *fp = fopen("word_game_data.bin", "wb");
    if (!fp) return;
    fwrite(&data, sizeof(GameData), 1, fp);
    fclose(fp);
}
static GameData gg_load(void) {
    GameData d;
    FILE *fp = fopen("word_game_data.bin", "rb");
    if (!fp) {
        srand((unsigned)time(NULL));
        const char *list[] = {"computer","programming","hangman","developer","keyboard","puzzle"};
        strcpy(d.target, list[rand()%6]);
        d.gamesPlayed = 0;
        gg_save(d);
    } else {
        fread(&d, sizeof(GameData), 1, fp);
        fclose(fp);
    }
    return d;
}

static void gg_draw(Rect c, const char* title, const char* display, int chances, const int guessed[26]) {
    clearContent(c);
    printIn(c,0,0,BOLD CYAN "=== WORD GUESSING GAME ===" ANSI_RESET);
    if (title && *title) { setCursor(c.x+2,c.y+2); printf("%s", title); }

    setCursor(c.x+2,c.y+4); printf("Word: ");
    if (display) {
        for (int i=0; display[i]; ++i) { putchar(display[i]); putchar(' '); }
    }
    setCursor(c.x+2,c.y+6); printf("Chances left: %d", chances);
    setCursor(c.x+2,c.y+8); printf("Guessed letters: ");
    int any=0;
    if (guessed) {
        for (int i=0;i<26;i++) if (guessed[i]) { putchar('a'+i); putchar(' '); any=1; }
    }
    if (!any) printf("(none)");
}

static int gg_prompt_int(Rect c, int y, const char* prompt, int defVal) {
    char line[64];
    setCursor(c.x+2, y); printf("%s", prompt);
    setCursor(c.x+2, y+1); printf("> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) return defVal;
    int v=defVal; sscanf(line, "%d", &v);
    return v;
}

/* read a single “letter or exit” from a line at bottom of pane */
static char gg_get_letter(Rect c) {
    char buf[64];
    setCursor(c.x+2, c.y + c.h - 3);
    for (int i=0;i<c.w-4;i++) putchar(' ');
    setCursor(c.x+2, c.y + c.h - 3);
    printf("Enter a letter (or type 'exit'): ");
    setCursor(c.x+2, c.y + c.h - 2);
    printf("> ");
    fflush(stdout);
    if (!fgets(buf, sizeof(buf), stdin)) return '0';
    buf[strcspn(buf, "\n")] = 0;

    if (_stricmp(buf, "exit")==0) return '0';
    if (strlen(buf)==1) {
        char ch = buf[0];
        if (ch>='A' && ch<='Z') ch = (char)(ch + 32);
        if (ch>='a' && ch<='z') return ch;
    }
    return '\n'; // invalid; loop will re-prompt
}

static void showGuessingGamePane(Rect c) {
    GameData game = gg_load();
    int L = (int)strlen(game.target);

    // difficulty prompt
    gg_draw(c, "Select difficulty:", NULL, 0, NULL);
    setCursor(c.x+2, c.y+4);  printf("1. EASY   (10 wrong guesses)");
    setCursor(c.x+2, c.y+5);  printf("2. MEDIUM (7 wrong guesses)");
    setCursor(c.x+2, c.y+6);  printf("3. HARD   (5 wrong guesses)");
    int choice = gg_prompt_int(c, c.y+8, "Choice (1/2/3):", 1);

    int chances = (choice==1)?10 : (choice==2)?7 : (choice==3)?5 : 10;

    char display[32]; for (int i=0;i<L;i++) display[i]='_'; display[L]='\0';
    int guessed[26]={0};

    while (chances > 0) {
        gg_draw(c, "", display, chances, guessed);

        char guess;
        do { guess = gg_get_letter(c); } while (guess=='\n'); // re-prompt on invalid

        // exit to menu
        if (guess=='0') {
            setCursor(c.x+2, c.y + c.h - 5); printf("\nExiting to menu...");
            setCursor(c.x+2, c.y + c.h - 3); printf("Press any key to continue...");
            getch();
            return;
        }

        if (guessed[guess-'a']) {
            setCursor(c.x+2, c.y + c.h - 5); printf("You already guessed '%c'!", guess);
            continue;
        }
        guessed[guess-'a'] = 1;

        int found=0;
        for (int i=0;i<L;i++) if (game.target[i]==guess) { display[i]=guess; found=1; }
        if (!found) chances--;

        if (strcmp(display, game.target)==0) {
            setCursor(c.x+2, c.y + c.h - 5);
            printf(ANSI_GREEN "Congratulations! You guessed '%s'!" ANSI_RESET, game.target);

            // prepare next target + persist
            srand((unsigned)time(NULL));
            const char *list[] = {"computer","programming","hangman","developer","keyboard","puzzle"};
            strcpy(game.target, list[rand()%6]);
            game.gamesPlayed++;
            gg_save(game);

            setCursor(c.x+2, c.y + c.h - 3); printf("Press any key to return...");
            getch();
            return;
        }
    }

    // out of chances
    gg_draw(c, "", display, chances, guessed);
    setCursor(c.x+2, c.y + c.h - 5);
    printf(ANSI_RED "Out of chances! The word was '%s'." ANSI_RESET, game.target);
    game.gamesPlayed++; gg_save(game);
    setCursor(c.x+2, c.y + c.h - 3); printf("Press any key to return...");
    getch();
}

/* ===== Sudoku (pane) ===== */
int puzzle[SIZE][SIZE] = {
    {5,3,0,0,7,0,0,0,0},
    {6,0,0,1,9,5,0,0,0},
    {0,9,8,0,0,0,0,6,0},
    {8,0,0,0,6,0,0,0,3},
    {4,0,0,8,0,3,0,0,1},
    {7,0,0,0,2,0,0,0,6},
    {0,6,0,0,0,0,2,8,0},
    {0,0,0,4,1,9,0,0,5},
    {0,0,0,0,8,0,0,7,9}
};

int solution[SIZE][SIZE] = {
    {5,3,4,6,7,8,9,1,2},
    {6,7,2,1,9,5,3,4,8},
    {1,9,8,3,4,2,5,6,7},
    {8,5,9,7,6,1,4,2,3},
    {4,2,6,8,5,3,7,9,1},
    {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4},
    {2,8,7,4,1,9,6,3,5},
    {3,4,5,2,8,6,1,7,9}
};

int given[SIZE][SIZE];

static void printSudokuInPane(Rect c, int mistakes) {
    clearContent(c);
    
    // Title and stats
    printIn(c, 0, 0, BOLD CYAN "=== SUDOKU CHALLENGE ===" RESET);
    setCursor(c.x + 2, c.y + 1);
    printf("Mistakes: %d/%d", mistakes, MAX_MISTAKES);
    
    // Draw the sudoku grid
    int startY = c.y + 3;
    int startX = c.x + 2;
    
    // Top border
    setCursor(startX, startY);
    printf("+-------+-------+-------+");
    
    for (int i = 0; i < SIZE; i++) {
        setCursor(startX, startY + 1 + i + i/3);
        printf("|");
        
        for (int j = 0; j < SIZE; j++) {
            if (puzzle[i][j] == 0) {
                printf(" .");
            } else {
                // Color coding: green for given numbers, white for user input
                if (given[i][j]) {
                    printf(GREEN " %d" RESET, puzzle[i][j]);
                } else {
                    printf(CYAN " %d" RESET, puzzle[i][j]);
                }
            }
            if ((j + 1) % 3 == 0) printf(" |");
        }
        
        // Horizontal separator every 3 rows
        if ((i + 1) % 3 == 0) {
            setCursor(startX, startY + 2 + i + i/3);
            printf("+-------+-------+-------+");
        }
    }
    
    // Instructions
    setCursor(c.x + 2, c.y + c.h - 4);
    printf(YELLOW "Enter row col val (e.g., '3 5 7')" RESET);
    setCursor(c.x + 2, c.y + c.h - 3);
    printf(YELLOW "Type 'quit' to return to menu" RESET);
}

static int isComplete() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (puzzle[i][j] == 0) return 0;
    return 1;
}

static void showSudokuPane(Rect c) {
    showCursor();
    int mistakes = 0;
    
    // Mark given cells
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            given[i][j] = (puzzle[i][j] != 0);
    
    while (1) {
        printSudokuInPane(c, mistakes);
        
        // Check win condition
        if (isComplete()) {
            setCursor(c.x + 2, c.y + c.h - 2);
            printf(GREEN BOLD "?? Congratulations! You solved it!" RESET);
            setCursor(c.x + 2, c.y + c.h - 1);
            printf("Press any key to return...");
            getch();
            hideCursor();
            return;
        }
        
        // Check loss condition
        if (mistakes >= MAX_MISTAKES) {
            setCursor(c.x + 2, c.y + c.h - 2);
            printf(RED BOLD "? Game Over! Too many mistakes." RESET);
            setCursor(c.x + 2, c.y + c.h - 1);
            printf("Press any key to return...");
            getch();
            hideCursor();
            return;
        }
        
        // Get input
        char input[64];
        setCursor(c.x + 2, c.y + c.h - 2);
        printf("Input: ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            hideCursor();
            return;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        // Check for quit
        if (strcmp(input, "quit") == 0) {
            hideCursor();
            return;
        }
        
        // Parse input
        int row, col, val;
        if (sscanf(input, "%d %d %d", &row, &col, &val) != 3) {
            setCursor(c.x + 2, c.y + c.h - 1);
            printf(RED "Invalid format! Try: row col value" RESET);
            getch();
            continue;
        }
        
        // Validate ranges
        if (row < 1 || row > 9 || col < 1 || col > 9 || val < 0 || val > 9) {
            setCursor(c.x + 2, c.y + c.h - 1);
            printf(RED "Invalid range! Row/col: 1-9, value: 0-9" RESET);
            getch();
            continue;
        }
        
        // Convert to 0-based indexing
        row--; col--;
        
        // Check if cell is given/fixed
        if (given[row][col]) {
            setCursor(c.x + 2, c.y + c.h - 1);
            printf(RED "Cannot modify given number!" RESET);
            getch();
            continue;
        }
        
        // Process move
        if (val == 0) {
            // Clear cell
            puzzle[row][col] = 0;
        } else if (solution[row][col] == val) {
            // Correct move
            puzzle[row][col] = val;
        } else {
            // Wrong move
            mistakes++;
            setCursor(c.x + 2, c.y + c.h - 1);
            printf(RED "? Wrong number! Mistakes: %d/%d" RESET, mistakes, MAX_MISTAKES);
            getch();
        }
    }
}


/* ===== Generic Coming Soon pane ===== */
static void showComingSoon(Rect c, const char* title, const char* body) {
    clearContent(c);
    printIn(c, 0, 0, BOLD CYAN);
    printIn(c, 0, 0, title);
    printIn(c, 0, 0, RESET);

    printIn(c, 0, 2, YELLOW "Coming Soon!" RESET);
    if (body && *body) printIn(c, 0, 4, body);

    setCursor(c.x+2, c.y + c.h - 3);
    printf("Press any key to return...");
    getch();
}

/* ===== Main ===== */
int main(void) {
    // enable ANSI VT on Windows
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0; if (GetConsoleMode(hout, &mode)) {
        SetConsoleMode(hout, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8); // set input code page

    hideCursor();

    Rect sidebar, content;
    drawAppChrome(&sidebar, &content);

    // Updated: 7 options now (added Algorithm Mode, Redirect System)
    int selected = 0, numOptions = 7;
    drawSidebar(sidebar, selected);

    while (1) {
        int ch = getch();
        if (ch == 27) { // ESC
            showCursor(); fastClear();
            printf(RED "Goodbye!\n" RESET);
            return 0;
        } else if (ch == 72 || ch=='w' || ch=='W') {
            selected = (selected - 1 + numOptions) % numOptions;
            drawSidebar(sidebar, selected);
        } else if (ch == 80 || ch=='s' || ch=='S') {
            selected = (selected + 1) % numOptions;
            drawSidebar(sidebar, selected);
        } else if (ch == 13) {
            switch (selected) {
                case 0: showTypeWriterPane(content);  break;
                case 1: showFlashPane(content);       break;
                case 2: showGuessingGamePane(content);break;
                case 3: showSudokuPane(content); break;
                case 4: // Algorithm Mode (Coming Soon)
                    showComingSoon(content, "Algorithm Mode", "A future mode for auto-solving and generating puzzles.");
                    break;
                case 5: 
                    system("start \"\" \".\\allphysics.html\"");
                     
                    break;
                case 6:
                    showCursor(); fastClear();
                    printf(RED "Thanks for using our menu system!\n" RESET);
                    return 0;
            }
            // redraw after returning from a pane
            drawAppChrome(&sidebar, &content);
            drawSidebar(sidebar, selected);
        }
    }
}

