#include "console_input_parser.h"
#include "console/log.h"
#include <assert.h>
#include <stdio.h>

enum CPState
{
    CP_PRE_COMMAND,
    CP_COMMAND,
    CP_PRE_ARGS,
    CP_ARGS,
};

struct ConsoleParsedCommand console_parse_input(StringView input)
{
    struct ConsoleParsedCommand parsed_cmd;
    const char *input_data = view_raw(input);
    parsed_cmd.command_name = (StringView){.length = 0, .data = input_data};
    parsed_cmd.command_args = (StringView){.length = 0, .data = input_data};
    size_t input_len = view_length(input);

    enum CPState state = CP_PRE_COMMAND;
    for(int parser_head = 0; parser_head < input_len; ++parser_head)
    {
        char cur_char = input_data[parser_head];
        switch (state)
        {
            case CP_PRE_COMMAND:
                if(cur_char != ' ')
                {
                    state = CP_COMMAND;
                    parsed_cmd.command_name.data = input_data+parser_head;
                    parsed_cmd.command_name.length = 1;
                }
                break;
            case CP_COMMAND:
                if(cur_char == ' ')
                {
                    state = CP_PRE_ARGS;
                }
                else ++parsed_cmd.command_name.length;
                break;
            case CP_PRE_ARGS:
                if(cur_char != ' ')
                {
                    state = CP_ARGS;
                    parsed_cmd.command_args.data = input_data+parser_head;
                    parsed_cmd.command_args.length = 1;
                }
                break;
            case CP_ARGS:
                ++parsed_cmd.command_args.length;
                break;
            default:
                assert(0);
            break;
        }
    }
    return parsed_cmd;
}

