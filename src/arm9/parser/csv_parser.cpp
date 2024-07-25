#include "csv_parser.h"
#include <stdio.h>
#include <ctype.h>

#define SCRIPT_SIZE 8192
#define BUF_L       4096

//==============================================================================

CsvFile::CsvFile() {
}
CsvFile::~CsvFile() {
    Clear();
}

void CsvFile::Clear() {
    u32 recL = records.size();
    CsvRecord* rec[recL];
    for (u32 n = 0; n < recL; n++) {
        rec[n] = records[n];
    }
    records.clear();

    for (u32 n = 0; n < recL; n++) {
        delete rec[n];
    }
}

bool CsvFile::Load(const char* file) {
    FileHandle* fh = fhOpen(file);
    if (!fh) {
        return false;
    }

    bool result = Load(fh);

    fhClose(fh);
    return result;
}

bool CsvFile::Load(FileHandle* fh) {
    Clear();

    char* script = new char[SCRIPT_SIZE];
    char* buf = new char[BUF_L];
    int readSize = 0;
    int readIndex = 0;

    do {
        ProcessLine(fh, script, readIndex, readSize, buf);
    } while (readSize > 0);

    delete[] buf;
    delete[] script;

    return true;
}

void CsvFile::ProcessLine(FileHandle* fh, char* script, int& readIndex, int& readSize,
    char* lineBuffer)
{
    bool nonWhitespaceRead = false;
    int t = 0;
    while (t < BUF_L - 1) {
        if (readIndex >= readSize) {
            readSize = fh->Read(script, SCRIPT_SIZE);
            readIndex = 0;
            if (readSize <= 0) {
                break;
            }
        }
        if (script[readIndex] == '\r' || script[readIndex] == '\n') {
            readIndex++;
            break;
        }

        if (script[readIndex] != ' ' && script[readIndex] != '\t') {
            nonWhitespaceRead = true;
        }
        if (nonWhitespaceRead) {
            lineBuffer[t] = script[readIndex];
            t++;
        }

        readIndex++;
    }
    lineBuffer[t++] = '\0';

    if (strchr(lineBuffer, ';')) {
        records.push_back(new CsvRecord(lineBuffer));
    }
}

CsvRecord* CsvFile::GetRecord(u16 index) {
    return records[index];
}

u16 CsvFile::GetNumberOfRecords() {
    return records.size();
}

//==============================================================================

CsvRecord::CsvRecord(char* line) {
	char* s = line;
	while (*s != '\0') {
		if (*s == ';') {
			*s = '\0';
	        fields.push_back(strdup(line));

	        line = s + 1;
		}
		s++;
	}
    fields.push_back(strdup(line));
}
CsvRecord::~CsvRecord() {
    u32 fldL = fields.size();
    char* fld[fldL];
    for (u32 n = 0; n < fldL; n++) {
        fld[n] = fields[n];
    }
    fields.clear();

    for (u32 n = 0; n < fldL; n++) {
        free(fld[n]);
    }
}

u16 CsvRecord::GetNumberOfFields() {
    return fields.size();
}
char* CsvRecord::AsString(u16 index) {
    return fields[index];
}
s32 CsvRecord::AsFixed(u16 index) {
    return floattof32(atof(fields[index]));
}
int CsvRecord::AsInt(u16 index) {
    return atoi(fields[index]);
}
bool CsvRecord::AsBool(u16 index) {
    return (strcmp(fields[index], "true") == 0);
}
