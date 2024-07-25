#ifndef TH_LOG_H
#define TH_LOG_H

#define __FILENAME__ (strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__)

enum ErrorLevel {
	EL_verbose = 8,
	EL_warning = 4,
	EL_error   = 2,
	EL_message = 0
};

#ifdef __cplusplus

#include "thcommon.h"
#include <cstdarg>
#include <vector>

class LogEntry {
	private:

	public:
		ErrorLevel elevel;
		const char* name;
		const char* message;

		LogEntry(ErrorLevel elevel, const char* name, const char* message);
		virtual ~LogEntry();

};

class LogBuffer {
	private:
		char* buffer;
		int bufferL;
		int writeOffset;

		std::vector<LogEntry> entries;

	public:
		LogBuffer(int size);
		virtual ~LogBuffer();

		bool Append(ErrorLevel elevel, const char* componentName, const char* format, va_list args);
		void FlushToDisk();
		void Reset();

		const std::vector<LogEntry> GetEntries();

};

extern LogBuffer* compilerLogBuffer;

extern "C" {

#endif

void initLog(int size);
void destroyLog();
void log(enum ErrorLevel elevel, const char* componentName, const char* format, ...);

void initCompilerLog(int size);
void destroyCompilerLog();
void resetCompilerLog();
void compilerLog(enum ErrorLevel elevel, const char* componentName, const char* format, ...);

#ifdef __cplusplus

} //end extern C

#endif

#endif
