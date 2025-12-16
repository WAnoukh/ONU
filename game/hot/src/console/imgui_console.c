#include "imgui_console.h"
#include "console/console.h"
#include <string.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_OPENGL3
#define CIMGUI_USE_GLFW
#include "cimgui.h"

#define CONSOLE_MAX_INPUT 256
#define LOG_LEVEL_STRING_MAX_LENGTH 10
#define LOG_TIME_LENGTH 9

#define LOG_STRING_LENGTH CONSOLE_LOG_LENGTH + LOG_LEVEL_STRING_MAX_LENGTH + LOG_TIME_LENGTH + 1

char console_input[CONSOLE_MAX_INPUT] = "";

int console_input_callback(ImGuiInputTextCallbackData *data)
{
    return 0;
}

char log_level_string[LOG_COUNT][LOG_LEVEL_STRING_MAX_LENGTH] =
{
    "[ERROR]",
    "[WARNING]",
    "[INFO]",
    "[VERBOSE]",
};

void log_level_get_string(char *buffer, size_t size, enum LogLevel level)
{
    snprintf(buffer, size, "%s", log_level_string[level]);
}

void get_time_string(char *buffer, size_t size, uint8_t hour, uint8_t min, uint8_t sec)
{
    snprintf(buffer, size, "%02d:%02d:%02d ", hour, min, sec);
}

void log_get_string(char *buffer, size_t size, struct Log log)
{
    get_time_string(buffer, size, log.hour, log.min, log.sec);
    size_t log_length = strlen(buffer);
    log_level_get_string(buffer + log_length, size - log_length, log.level);
    log_length = strlen(buffer);
    snprintf(buffer + log_length, size - log_length, " %s", log.text);
}

void igConsoleContent()
{
    struct Console *console = console_get_current();
    struct Log *log_ring_buffer = console_get_log_ring_buffer(console); 

    int log_count = console_get_log_count(console);
    int log_index = console_get_log_head(console);

    int last_log_index = log_index - log_count;
    if(last_log_index < 0) last_log_index += CONSOLE_MAX_LOG;

    char log_text[LOG_STRING_LENGTH];
    log_index = last_log_index;
    while(log_index != console->log_head)
    {
        log_get_string(log_text, LOG_STRING_LENGTH, log_ring_buffer[log_index]);
        igTextUnformatted(log_text, NULL);

        ++log_index;
        if(log_index >= CONSOLE_MAX_LOG) log_index = 0;
    }
    //igTextUnformatted(console->out, NULL);
}

void igConsole()
{
    igBegin("Console", NULL, 0);

    igBeginChild_Str("ScrollingRegion", (ImVec2){0, -igGetFrameHeightWithSpacing()-5}, false, ImGuiWindowFlags_HorizontalScrollbar);
    igConsoleContent();
    igSetScrollHereY(1.0f);

    igEndChild();


    bool reclaim_focus = 0;
    if (igInputText("Input", console_input, CONSOLE_MAX_INPUT,
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_CallbackHistory,
        console_input_callback, NULL))
    {
        console_log(console_input, LOG_INFO);
        console_input[0] = '\0';
        reclaim_focus = 1;
    }

    igSetItemDefaultFocus();
    if(reclaim_focus)
    {
        igSetKeyboardFocusHere(-1);
    }

    igEnd();
}
