#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printText(char* ptr, WORD newColor) {

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo = { 0 };
    WORD saved_attributes = 0;
    
    if (!hConsole) {
        printf("Error -: %lu\n", GetLastError());
    }
    else {
        if (!GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
            printf("Error -: %lu\n", GetLastError());
        }
    }
    
    saved_attributes = consoleInfo.wAttributes;   
    
    if (hConsole!=0) {
        if (!SetConsoleTextAttribute(hConsole, newColor)) {
            printf("Error -: %lu\n", GetLastError());
        }
    }
    printf("%s", ptr);

    if (hConsole != 0) {
        if (!SetConsoleTextAttribute(hConsole, saved_attributes)) {
            printf("Error -: %lu\n", GetLastError());
        }
    }
}

// Function to create a global mutex
void create_global_mutex(const char *mutexName) {
    char globalMutexName[256];
    snprintf(globalMutexName, sizeof(globalMutexName), "Global\\%s", mutexName);

    HANDLE hMutex = CreateMutexA(NULL, FALSE, globalMutexName);
    if (hMutex == NULL) {
        printf("CreateMutex failed for %s with error %lu\n", globalMutexName, GetLastError());
    } else {
        printText("   `-=>Created Decoy mutex: ", FOREGROUND_GREEN);
        printf("%s\n", globalMutexName);
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            printf("Mutex %s already exists.\n", globalMutexName);
        }
    }
}

// Simple function to trim whitespace from a string
char *trim_whitespace(char *str) {
    char *end;
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') str++;  // Leading space
    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) *end-- = '\0';

    return str;
}

// Function to parse JSON and create mutexes
void parse_and_create_mutexes(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    char line[1024];
    int inMutexArray = 0;

    while (fgets(line, sizeof(line), file)) {
        char *trimmedLine = trim_whitespace(line);

        if (strstr(trimmedLine, "\"ransomware\"")) {
            // Extract ransomware name
            char *start = strchr(trimmedLine, ':');
            if (start) {
                start += 2;  // Move to start of the ransomware name
                char *end = strchr(start, '"');
                if (end) *end = '\0';
                printText("\n[+] ", FOREGROUND_INTENSITY);
                printText("Ransomware: ", FOREGROUND_RED);
                printf("%s\n", start);
            }
        } else if (strstr(trimmedLine, "\"mutex\"")) {
            // Begin of mutex array
            inMutexArray = 1;
        } else if (inMutexArray && strstr(trimmedLine, "]")) {
            // End of mutex array
            inMutexArray = 0;
        } else if (inMutexArray) {
            // Extract mutex name
            char *start = strchr(trimmedLine, '"');
            if (start) {
                start++;
                char *end = strchr(start, '"');
                if (end) *end = '\0';
                create_global_mutex(start);  // Create global mutex
            }
        }
    }
    printText("\n[+] Successfully loaded all Decoy Mutex objects. Test Ransomware simulation now...", FOREGROUND_INTENSITY);
    fclose(file);
}


void PrintBanner(){
    printText(" ___                           __ __        _           \n", FOREGROUND_INTENSITY);
    printText("| . \\ ___  ___  ___  _ _  ___ |  \\  \\ _ _ _| |_ ___ __  \n", FOREGROUND_INTENSITY);
    printText("| | |/ ._>/ | '/ . \\| | ||___||     || | | | | / ._>\\ \\/\n", FOREGROUND_INTENSITY);
    printText("|___/\\___.\\_|_.\\___/`_. |     |_|_|_|`___| |_| \\___./\\_\\\n", FOREGROUND_INTENSITY);
    printText("                    <___'                               \n", FOREGROUND_INTENSITY);
    printText("                        by Madhukar Raina (@ScarredMonk)\n", FOREGROUND_GREEN);

    printText("\n[!] ", FOREGROUND_INTENSITY);
    printText("Warning: This program is created for educational purposes\n", FOREGROUND_RED);
    printText("              and is intended to be used in lab environments.\n\n", FOREGROUND_RED);

}

// Display help menu
void display_help() {
    PrintBanner();
    printf("Usage: decoy-mutex.exe [options]\n");
    printf("Options:\n");
    printf("  -h, --help           Show this help message\n");
    printf("  -f <filename>        Specify JSON file containing ransomware and mutex names\n");
    printf("\nExample JSON structure:\n");
    printf("[\n");
    printf("    {\n");
    printf("        \"ransomware\": \"WannaCry\",\n");
    printf("        \"mutex\": [\n");
    printf("            \"MsWinZonesCacheCounterMutexA\",\n");
    printf("            \"MsWinZonesCacheCounterMutexA0\"\n");
    printf("        ]\n");
    printf("    },\n");
    printf("    {\n");
    printf("        \"ransomware\": \"Conti\",\n");
    printf("        \"mutex\": [\n");
    printf("            \"kjsidugidf99439\",\n");
    printf("            \"hsfjuukjzloqu28oajh7271\"\n");
    printf("        ]\n");
    printf("    }\n");
    printf("]\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        display_help();
        return 1;
    }

    const char *filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            display_help();
            return 0;
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            filename = argv[++i];
        }
    }

    if (filename == NULL) {
        printf("Error: JSON file not specified.\n");
        display_help();
        return 1;
    }

    PrintBanner();
    
    parse_and_create_mutexes(filename);

    // Keep the program running to keep mutexes alive
    while (1) Sleep(1000);
    return 0;
}
