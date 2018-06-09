#ifndef LOG_H
#define LOG_H

/**
 * @auther Matthew Strasiotto mstr3336
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#define DEBUG
#define VERBIOSITY (4)

#define THREAD_ID (0)

#ifdef PTHREAD_PROCESS_SHARED
#undef THREAD_ID
#define THREAD_ID  (pthread_self())
#endif

#define LOG_B(_format, ...)  fprintf(stderr, "T:%lu|File:%15s|Ln:%3d|Fn:%20s|" _format "\n", THREAD_ID, __FILE__, __LINE__,__func__, __VA_ARGS__)

#define LOG_V(_format, ...)
#define LOG_I(_format, ...)
#define LOG_D(_format, ...)
#define LOG_W(_format, ...)
#define LOG_E(_format, ...)


#ifdef DEBUG
	#undef LOG_E
	#define LOG_E(_format, ...) LOG_B("E|" _format, __VA_ARGS__)

	#if VERBIOSITY > 0
		#undef LOG_W
		#define LOG_W(_format, ...) LOG_B("W|" _format, __VA_ARGS__)
	#endif

	#if VERBIOSITY > 1
		#undef LOG_D
		#define LOG_D(_format, ...) LOG_B("D|" _format, __VA_ARGS__)
	#endif

	#if VERBIOSITY > 2
		#undef LOG_I
		#define LOG_I(_format, ...) LOG_B("I|" _format, __VA_ARGS__)
	#endif

	#if VERBIOSITY > 3
		#undef LOG_V
		#define LOG_V(_format, ...) LOG_B("V|" _format, __VA_ARGS__)
	#endif

#endif

#endif
