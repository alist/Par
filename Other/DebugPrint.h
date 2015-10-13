
#ifndef DebugPrint_h
#define DebugPrint_h

#if ! defined(__GNUC__)
#warning DEBUG/NDEBUG macros may not be #defined correctly
#endif
#if ! defined(DEBUG) && ! defined(NDEBUG)
#if defined(__OPTIMIZE__)
#define NDEBUG 1
#else
#define DEBUG 1
#endif
#endif

#if ! defined(DEBUG) && ! defined(NDEBUG)
#warning Neither of DEBUG/NDEBUG macros are #defined
#endif

#ifdef DEBUG
#define DebugPrint(...) fprintf(stderr, __VA_ARGS__)
#define Debug(...) __VA_ARGS__
#else
#define DebugPrint(...)
#define Debug(...)
#endif

#endif /* DebugPrint_h */
