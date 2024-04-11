#ifndef COMPILER_EXTENTIONS_H
#define COMPILER_EXTENTIONS_H

/** @brief Macro extending C99 to force warning when function result is unused. */
#ifndef CHECK_RESULT
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define CHECK_RESULT __attribute__ ((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define CHECK_RESULT _Check_return_
#else
#define CHECK_RESULT
#endif
#endif

#endif