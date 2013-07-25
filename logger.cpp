#define BUFFER_SIZE BUFSIZ
#define LOCAL_BUFFER_SIZE BUFSIZ


#include "logger.h"

#if __x86_64__
  #define ENV64BIT
#else
  #define ENV32BIT
#endif

/* ########################################################################################################################################################*/
#ifdef __linux__
  typedef pthread_mutex_t MutexType;
#else
  typedef CRITICAL_SECTION MutexType;
#endif


MutexType mutex;

#ifdef WIN32

// Реализация через Windows API

  int mutex_init (MutexType* mutex)	              { InitializeCriticalSection(mutex); return 1; }
  int mutex_destroy (MutexType* mutex)            { DeleteCriticalSection(mutex); return 1; }
  void mutex_lock (MutexType* mutex)              { EnterCriticalSection(mutex); }
  void mutex_unlock (MutexType* mutex)            { LeaveCriticalSection(mutex); }

#else // WIN32

// UNIX версия через pthread

  int mutex_init (MutexType* mutex)               { return pthread_mutex_init(mutex, NULL);  }
  int mutex_destroy (MutexType* mutex)            { return pthread_mutex_destroy(mutex); }
  void mutex_lock (MutexType* mutex)              { pthread_mutex_lock(mutex); }
  void mutex_unlock (MutexType* mutex)            { pthread_mutex_unlock(mutex); }

#endif 

// initializing the mutex
int rrr = mutex_init(&mutex);

/* ########################################################################################################################################################*/

using namespace std;



static bool should_create_log_file = true;
static ofstream logfile ("log.txt", ios::app);
static char local_buffer[LOCAL_BUFFER_SIZE]; //this buffer is used to read various number of arguments in log_write() function 
static string buffer; // this buffer is used to optimize output process in file 
static va_list args;
static unsigned int free_space = BUFFER_SIZE;

struct theTime
{
  int sec;
  float  milli;
} curtime;

struct theDate
{
  int sec;
  int min;
  int hour;
  int day;
  int month;
  int year;
} curdate;

#if defined(__linux__)
  static struct timespec current_time;
  clockid_t clk_id = CLOCK_MONOTONIC;
  static struct sigaction signal_handler_structure;

  time_t t = time(0);
  struct tm* now = localtime(&t);

  static void getTime() {
    clock_gettime(clk_id, &current_time);
    curtime.sec = current_time.tv_sec;
    curtime.milli = current_time.tv_nsec/1000000;
  }

  static void getDate() {
    curdate.sec = now->tm_sec;
    curdate.min = now->tm_min;
    curdate.hour = now->tm_hour;
    curdate.day = now->tm_mday;
    curdate.month = now->tm_mon + 1;
    curdate.year = now->tm_year + 1900;
  }

#else
  SYSTEMTIME st;

  static void getTime() {
    GetSystemTime(&st);
    curtime.sec = st.wSecond;
    curtime.milli = st.wMilliseconds;
  }

  static void getDate() {
  	GetSystemTime(&st);
    curdate.sec = st.wSecond;
    curdate.min = st.wMinute;
    curdate.hour = st.wHour;
    curdate.day = st.wDay;
    curdate.month = st.wMonth;
    curdate.year = st.wYear;
  }
#endif




void close_log_file(){
  logfile << buffer;
  logfile << "\n\n#######################################################################################################\n\n";
  logfile.close();
}



#if defined(__linux__)
// signal handler
static void signal_handler(const int sig) {
  string s;
  switch (sig) {
    case SIGSEGV : 
      s = "SEGMENTATION FAULT";
      break;
    case SIGILL : 
      s = "Illegal Instruction";
      break;
    case SIGPIPE : 
      s = "Broken pipe: write to pipe with no readers";
      break;
    case SIGBUS : 
      s = "Bus error (bad memory access)";
      break;
    case SIGSYS : 
      s = "Bad argument to routine (SVr4)";
      break;
    case SIGXFSZ : 
      s = "File size limit exceeded (4.2BSD)";
      break;
    case SIGFPE : 
      s = "Floating point exception";
      break;
    case SIGABRT : 
      s = "Abort signal from abort(3)";
      break;
  };
  logfile << s << endl;
  mutex_lock(&mutex);
  close_log_file();
  mutex_unlock(&mutex);
  mutex_destroy(&mutex);
  exit(1);
}

#elif defined (WIN32)
//windows exceptions filter function

LONG exHandler(LPEXCEPTION_POINTERS p)
{
  DWORD exception_type = p->ExceptionRecord->ExceptionCode;
  string s;
  switch (exception_type) {
    case EXCEPTION_ACCESS_VIOLATION : 
      s = "EXCEPTION_ACCESS_VIOLATION";
      break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED : 
      s = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
      break;
    case EXCEPTION_BREAKPOINT : 
      s = "EXCEPTION_BREAKPOINT";
      break;
    case EXCEPTION_DATATYPE_MISALIGNMENT : 
      s = "EXCEPTION_DATATYPE_MISALIGNMENT";
      break;
    case EXCEPTION_FLT_DENORMAL_OPERAND : 
      s = "EXCEPTION_FLT_DENORMAL_OPERAND";
      break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO : 
      s = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
      break;
    case EXCEPTION_FLT_INEXACT_RESULT : 
      s = "EXCEPTION_FLT_INEXACT_RESULT";
      break;
    case EXCEPTION_FLT_INVALID_OPERATION : 
      s = "EXCEPTION_FLT_INVALID_OPERATION";
      break;
    case EXCEPTION_FLT_OVERFLOW : 
      s = "EXCEPTION_FLT_OVERFLOW";
      break;
    case EXCEPTION_FLT_STACK_CHECK : 
      s = "EXCEPTION_FLT_STACK_CHECK";
      break;
    case EXCEPTION_FLT_UNDERFLOW : 
      s = "EXCEPTION_FLT_UNDERFLOW";
      break;
    case EXCEPTION_ILLEGAL_INSTRUCTION : 
      s = "EXCEPTION_ILLEGAL_INSTRUCTION";
      break;
    case EXCEPTION_IN_PAGE_ERROR : 
      s = "EXCEPTION_IN_PAGE_ERROR";
      break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO : 
      s = "EXCEPTION_INT_DIVIDE_BY_ZERO";
      break;
    case EXCEPTION_INT_OVERFLOW : 
      s = "EXCEPTION_INT_OVERFLOW";
      break;
    case EXCEPTION_INVALID_DISPOSITION : 
      s = "EXCEPTION_INVALID_DISPOSITION";
      break;  
    case EXCEPTION_NONCONTINUABLE_EXCEPTION : 
      s = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
      break;
    case EXCEPTION_PRIV_INSTRUCTION : 
      s = "EXCEPTION_PRIV_INSTRUCTION";
      break;
    case EXCEPTION_SINGLE_STEP : 
      s = "EXCEPTION_SINGLE_STEP";
      break;
    case EXCEPTION_STACK_OVERFLOW : 
      s = "EXCEPTION_STACK_OVERFLOW";
      break; 
  };
  logfile << s << endl;
  logfile << buffer;
  buffer.clear();
  return EXCEPTION_EXECUTE_HANDLER;
}
#endif


// this function returns description of host operating system
static string getOSname() {
  #if  defined (WIN32)
    return "Windows32";
  #elif defined(__linux__)
    return "Linux";
  #elif defined(__APPLE__)
    return "Apple";
  #endif
}
// this function returns if the host system is 32bit or 64bit 
static string getOStype() {
  #ifdef ENV64BIT
    return "64 bit";
  #else
    return "32 bit";
  #endif 
}


// this function converts int to string
static string to_s(const int& port) {
  stringstream ss;
  ss << port;
  return ss.str();
}

// this function pushes log message to buffer
static int add_to_buffer(const char* type, const char* file, const int line , const int sec, const long milli, const char* lcl) {
  buffer += "[";
  buffer += type;
  buffer += "] ";
  buffer +=  file;
  buffer += ", line " + to_s(line) + "  " + to_s(sec) + "." + to_s(milli) + " " + lcl + "\n";
  return buffer.size();
}




// this function register signal handlers , creates the log file and write there init information 
static void create_logfile()
{

//register signals  handlers

#if defined(__linux__)

  signal_handler_structure.sa_handler = signal_handler;
  sigemptyset(&signal_handler_structure.sa_mask);
  signal_handler_structure.sa_flags = 0;

  if ( sigaction(SIGSEGV, &signal_handler_structure, NULL) < 0 ) fprintf(stderr, "%s\n", "Can't register SIGSEGV" );
  if ( sigaction(SIGILL, &signal_handler_structure, NULL) < 0 ) fprintf(stderr, "%s\n", "Can't register SIGILL" );
  if ( sigaction(SIGPIPE, &signal_handler_structure, NULL) < 0 ) fprintf(stderr, "%s\n", "Can't register SIGPIPE" );
  if ( sigaction(SIGBUS, &signal_handler_structure, NULL) < 0 ) fprintf(stderr, "%s\n", "Can't register SIGBUS" );
  if ( sigaction(SIGSYS, &signal_handler_structure, NULL) < 0 ) fprintf(stderr, "%s\n", "Can't register SIGSYS" );
  if ( sigaction(SIGXFSZ, &signal_handler_structure, NULL) < 0 ) fprintf(stderr, "%s\n", "Can't register SIGXFSZ" );
  if ( sigaction(SIGFPE, &signal_handler_structure, NULL) < 0 ) fprintf(stderr, "%s\n", "Can't register SIGFPE" );
  if ( sigaction(SIGABRT, &signal_handler_structure, NULL) < 0 ) fprintf(stderr, "%s\n", "Can't register SIGABRT" );
#elif defined(WIN32)
  SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER) &exHandler);
#endif


//print system info to logfile

  logfile << "System version: " << " " << getOSname() <<" " << getOStype() << endl;

  getDate();

//print time info to logfile
  logfile << "Start time & date: ";
  logfile << curdate.day << "-" << curdate.month << "-" << curdate.year << " ";
  logfile << curdate.hour << ":" << curdate.min << ":" << curdate.sec << '\n';

}

// this is the main function.  
void log_print(const char* type, const char* file, int line ,const char *format,...) {

  string bufferAddOn;

// if log file doesn't exist, It should be created by calling create_log_file() function. 	
  mutex_lock(&mutex);
  if (should_create_log_file) {
    should_create_log_file = false;
    create_logfile();
  }
  mutex_unlock(&mutex);

// here we retrieve incoming useful parameters with format and write formatted parameters in local buffer 
  va_start (args, format);
  vsnprintf (local_buffer, LOCAL_BUFFER_SIZE, format, args);
  bufferAddOn = local_buffer;
// if there isn't enough space in our file buffer, we should flush it into the log file
  if  ( free_space < bufferAddOn.size() )  {
    mutex_lock(&mutex);
    logfile << buffer;
    buffer.clear();
    free_space = BUFFER_SIZE;
    mutex_unlock(&mutex);
  }


  getTime();
  mutex_lock(&mutex);

// here we add type of message, file and line where in was generated, time when it was generated and the message itself.
// the add_to_buffer() function returns the current data size in buffer 
  int len = add_to_buffer(type, file, line, curtime.sec, curtime.milli, local_buffer);
  free_space -= len;
  mutex_unlock(&mutex);
}
/*
TODO

-replace time functions by Windows analog
*/
