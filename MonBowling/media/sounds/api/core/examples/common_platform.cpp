/*==============================================================================
FMOD Example Framework
Copyright (c), Firelight Technologies Pty, Ltd 2014-2025.
==============================================================================*/
#include "common.h"
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <string>

static unsigned int gPressedButtons = 0;
static unsigned int gDownButtons = 0;
static std::string gConsoleText;
static std::vector<char *> gPathList;
static termios originalTerm = {0};

static void RevertTerminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerm);
    
    printf("%c[?25h", 0x1B); // Show the cursor
}

void Common_Init(void **extraDriverData)
{
    int err = tcgetattr(STDIN_FILENO, &originalTerm);
    assert(err == 0);

    err = atexit(RevertTerminal);    // Register for atexit in case we bail and don't call Common_Close
    assert(err == 0);
    
    termios term = {0};
    err = tcgetattr(STDIN_FILENO, &term);
    assert(err == 0);
    
    term.c_lflag &= ~(ICANON); // Disable special characters, i.e. EOF, EOL, etc
    term.c_lflag &= ~(ECHO);   // Prevent echo of characters
    term.c_cc[VMIN] = 1;       // Specify min number of bytes before a read() can return
    
    err = tcsetattr(STDIN_FILENO, TCSANOW, &term); 
    assert(err == 0);
    
    printf("%c[?25l", 0x1B); // Hide the cursor
}

void Common_Close()
{
    for (std::vector<char *>::iterator item = gPathList.begin(); item != gPathList.end(); ++item)
    {
        free(*item);
    }
}

static bool IsKeyPressed()
{
    fd_set fileDescMask;  
    FD_ZERO(&fileDescMask);
    FD_SET(STDIN_FILENO, &fileDescMask);

    timeval timeSpan = {0, 1000}; // 0s, 1000us
    
    // Check file descriptor provided for read, returns number of ready for read file descriptors
    int err = select(1, &fileDescMask, NULL, NULL, &timeSpan);
    assert(err >= 0);
    
    return (err > 0);
}

void Common_Update()
{
    /*
        Capture key input
    */
    unsigned int newButtons = 0;
    while (IsKeyPressed())
    {   
        unsigned int key = getchar();

        if      (key == '1')    newButtons |= (1 << BTN_ACTION1);
        else if (key == '2')    newButtons |= (1 << BTN_ACTION2);
        else if (key == '3')    newButtons |= (1 << BTN_ACTION3);
        else if (key == '4')    newButtons |= (1 << BTN_ACTION4);
        else if (key == 'w')    newButtons |= (1 << BTN_UP);
        else if (key == 'a')    newButtons |= (1 << BTN_LEFT);
        else if (key == 's')    newButtons |= (1 << BTN_DOWN);
        else if (key == 'd')    newButtons |= (1 << BTN_RIGHT);
        else if (key == 32)     newButtons |= (1 << BTN_MORE);
        else if (key == 'q')    newButtons |= (1 << BTN_QUIT);
    }

    gPressedButtons = (gDownButtons ^ newButtons) & newButtons;
    gDownButtons = newButtons;

    /*
        Update the screen
    */
    printf("%c[H", 0x1B);               // Move cursor to home position
    printf("%s", gConsoleText.c_str()); // Terminal console is already double buffered, so just print
    printf("%c[J", 0x1B);               // Clear the rest of the screen
    
    gConsoleText.clear();
}

void Common_Exit(int returnCode)
{
    exit(returnCode);
}

void Common_DrawText(const char *text)
{
    char s[256];
    snprintf(s, sizeof(s), "%s%c[K\n", text, 0x1B); // Print the text and clear the rest of the line

    gConsoleText.append(s);
}

bool Common_BtnPress(Common_Button btn)
{
    return ((gPressedButtons & (1 << btn)) != 0);
}

bool Common_BtnDown(Common_Button btn)
{
    return ((gDownButtons & (1 << btn)) != 0);
}

const char *Common_BtnStr(Common_Button btn)
{
    switch (btn)
    {
        case BTN_ACTION1:   return "1";
        case BTN_ACTION2:   return "2";
        case BTN_ACTION3:   return "3";
        case BTN_ACTION4:   return "4";
        case BTN_UP:        return "W";
        case BTN_LEFT:      return "A";
        case BTN_DOWN:      return "S";
        case BTN_RIGHT:     return "D";
        case BTN_MORE:      return "SPACE";
        case BTN_QUIT:      return "Q";
        default:            return "Unknown";
    }
}

const char *Common_MediaPath(const char *fileName)
{
    static bool pathInitialized = false;
    static char pathPrefix[256] = { };
    if (!pathInitialized)
    {
        pathInitialized = true;
        FILE *file = fopen(fileName, "r");
        if (file)
        {
            fclose(file);
            pathPrefix[0] = 0;
        }
        else
        {
            ssize_t len = readlink("/proc/self/exe", pathPrefix, 256);
            assert(len != -1);

            char *filePathEnd = strrchr(pathPrefix, '/');
            assert (filePathEnd != NULL);

            filePathEnd++; // Move past the last slash
            filePathEnd[0] = '\0';

            strcat(pathPrefix, "../media/");
        }
    }
    if (pathPrefix[0] == 0)
    {
        return fileName;
    }

    char *filePath = (char *)calloc(256, sizeof(char));
    strcpy(filePath, pathPrefix);
    strcat(filePath, fileName);
    gPathList.push_back(filePath);

    return filePath;
}

const char *Common_WritePath(const char *fileName)
{
    return Common_MediaPath(fileName);
}
