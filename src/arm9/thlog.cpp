#include "thlog.h"
#include "tcommon/png.h"

using namespace std;

LogBuffer* logBuffer = NULL;
LogBuffer* compilerLogBuffer = NULL;

//------------------------------------------------------------------------------
LogEntry::LogEntry(ErrorLevel el, const char* n, const char* m)
: elevel(el), name(n), message(m)
{

}
LogEntry::~LogEntry() {

}

//------------------------------------------------------------------------------

LogBuffer::LogBuffer(int size) {
	bufferL = size;
	buffer = new char[bufferL];
	writeOffset = 0;
}
LogBuffer::~LogBuffer() {
	if (buffer) delete[] buffer;
}

bool LogBuffer::Append(ErrorLevel elevel, const char* component, const char* format, va_list args) {
	char* message = buffer+writeOffset;
	int written = 0;
	written += snprintf(message, bufferL-1-writeOffset-written, "[%s] ", component);
	written += vsnprintf(message+written, bufferL-1-writeOffset-written, format, args);
    if (written >= bufferL-writeOffset) {
    	return false;
    }

    writeOffset += written;
    buffer[writeOffset] = '\n';
    writeOffset++;

    entries.push_back(LogEntry(elevel, component, message));

    u16 c = RGB15(31, 8, 8);
	if (elevel == EL_warning) {
		c = RGB15(31, 16, 8);
	} else if (elevel == EL_verbose) {
		c = RGB15(8, 16, 31);
	}
	//if (preferences->GetDebugLevel() < elevel) {
	//	return true;
	//}
	printf("%s\n", message);

    return true;
}

void LogBuffer::FlushToDisk() {
	FILE* file = fopen("thds.log", "a");
	if (file) {
		fwrite(buffer, sizeof(char), writeOffset, file);
		fclose(file);
	}

	Reset();
}

void LogBuffer::Reset() {
	writeOffset = 0;
	entries.clear();
}

const vector<LogEntry> LogBuffer::GetEntries() {
	return entries;
}

//------------------------------------------------------------------------------

void pngErr(png_structp png_ptr, png_const_charp error_msg) {
	log(EL_error, "libpng", "libpng_err: %s", error_msg);
	longjmp(png_ptr->jmpbuf, 1);
}
void pngWarn(png_structp png_ptr, png_const_charp warning_msg) {
	log(EL_warning, "libpng", "libpng_warn: %s", warning_msg);
}

void initLog(int logSize) {
	destroyLog();
	logBuffer = new LogBuffer(logSize > 0 ? logSize : 65535);

	pngErrorCallback = &pngErr;
	pngWarningCallback = &pngWarn;
}
void destroyLog() {
	if (logBuffer) {
		delete logBuffer; logBuffer = NULL;
	}
}

void log(ErrorLevel elevel, const char* componentName, const char* format, ...) {
    va_list args;
    va_start(args, format);
	if (!logBuffer->Append(elevel, componentName, format, args)) {
		logBuffer->FlushToDisk();
		logBuffer->Append(elevel, componentName, format, args);
	}
    va_end(args);
}

void initCompilerLog(int logSize) {
	destroyCompilerLog();
	compilerLogBuffer = new LogBuffer(logSize > 0 ? logSize : 65535);
}
void destroyCompilerLog() {
	if (compilerLogBuffer) {
		delete compilerLogBuffer; compilerLogBuffer = NULL;
	}
}

void resetCompilerLog() {
	compilerLogBuffer->Reset();
}

void compilerLog(ErrorLevel elevel, const char* componentName, const char* format, ...) {
    va_list args;
    va_start(args, format);
	if (!compilerLogBuffer->Append(elevel, componentName, format, args)) {
		//Ignore overflows in case of the compiler log
		//compilerLogBuffer->FlushToDisk();
		//compilerLogBuffer->Append(elevel, componentName, format, args);
	}
    va_end(args);
}
