#ifndef LOG_H
#define LOG_H

#define LOG_ERROR(cstr_const)    console_log(VIEW_FROM_CONST_STR(cstr_const), LOGL_ERROR)
#define LOG_WARNING(cstr_const)  console_log(VIEW_FROM_CONST_STR(cstr_const), LOGL_WARNING)
#define LOG_INFO(cstr_const)     console_log(VIEW_FROM_CONST_STR(cstr_const), LOGL_INFO)
#define LOG_VERBOSE(cstr_const)  console_log(VIEW_FROM_CONST_STR(cstr_const), LOGL_VERBOSE)

#endif // LOG_H
