#include "parser.h"

#include "runtime.h"
#include "../sprite.h"
#include "../thlog.h"
#include "../tcommon/filehandle.h"

int loadScript(const char* path) {
    FileHandle* fh = fhOpen(path);
    if (!fh) {
        compilerLog(EL_error, __FILENAME__, "File not found: %s", path);
        return COMPILE_FILE_NOT_FOUND;
    }

    int textL = fh->length;
    char* text = new char[textL];
    fh->ReadFully(text);
    fhClose(fh);

    int result = scriptFromMem(text, textL);
    if (result != 0) {
        compilerLog(EL_error, __FILENAME__, "Error in: %s", path);
    }

    int parseErrorsL = strlen(parseErrors);
    if (parseErrorsL > 0) {
    	compilerLog(EL_error, __FILENAME__, "%s", parseErrors);
        if (result == 0) {
        	result = COMPILE_SYNTAX_ERROR;
        }
    }

    delete[] text;
    return result;
}
