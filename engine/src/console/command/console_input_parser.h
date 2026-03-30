#ifndef CONSOLE_INPUT_PARSER_H
#define CONSOLE_INPUT_PARSER_H

#include "string/StringView.h"

struct ConsoleParsedCommand
{
    StringView command_name;
    StringView command_args;
};

struct ConsoleParsedCommand console_parse_input(StringView input);

#endif // CONSOLE_INPUT_PARSER_H
