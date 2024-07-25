#ifndef CSV_PARSER_H
#define CSV_PARSER_H

using namespace std;

#include <vector>
#include <fat.h>
#include <unistd.h>
#include <sys/dir.h>

#include "../thcommon.h"
#include "../tcommon/filehandle.h"

class CsvRecord {
    private:

    public:
        vector<char*> fields;

        CsvRecord(char* line);
        virtual ~CsvRecord();

        u16 GetNumberOfFields();
        char* AsString(u16 index);
        s32   AsFixed(u16 index);
        int   AsInt(u16 index);
        bool  AsBool(u16 index);
};

class CsvFile {
    private:
        vector<CsvRecord*> records;

        void Clear();
        void ProcessLine(FileHandle* fh, char* script, int& readIndex, int& readSize,
                         char* lineBuffer);

    public:
        CsvFile();
        virtual ~CsvFile();

        bool Load(const char* file);
        bool Load(FileHandle* fh);

        bool Save(const char* file);

        u16 GetNumberOfRecords();
        CsvRecord* GetRecord(u16 index);
};

#endif
