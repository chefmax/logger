#include <string>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <cstdarg>
#include <errno.h>
#include <cstdio>
#include <sstream>

#if defined(__linux__)
#include <signal.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <sys/time.h>
#else
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <ctype.h>
#endif



/*
This library provides simple logger, which has printf-like interface.
The library provides several printf-like functions for user:
	
	void log_fatal(...) 
	void log_error(...) 
	void log_warning(...)
	void log_info(...) 
	void log_debug(...)

The result file is named "log.txt" 	
On Linux the logger catches and handles some signals from the kernel, while on Windows it handles exceptions.

*/




#define log_write(type,...) ( log_print(type,__FILE__,__LINE__, __VA_ARGS__) )

#define log_fatal(...) ( log_write("fatal",__VA_ARGS__) )
#define log_error(...) ( log_write("error", __VA_ARGS__) )
#define log_warning(...) ( log_write("warning", __VA_ARGS__) )
#define log_info(...) ( log_write("info", __VA_ARGS__) )
#define log_debug(...) ( log_write("debug", __VA_ARGS__) )



void log_print(const char* type, const char* file, int line ,const char *format, ...) __attribute__ (( format( printf,4, 5 ) ));
void close_log_file();










