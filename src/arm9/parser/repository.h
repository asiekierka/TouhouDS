#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../thcommon.h"
#include <vector>

using namespace std;

class CsvFile;

enum RepoVersionTag {
	RVT_unknown,
	RVT_olderInstalled,
	RVT_installed,
	RVT_newerInstalled
};

class RepositoryRecord {
	private:
		char* id;
		char* name;
		u32 version;
		u32 date;
		char* author;
		char* url;
		char* desc;

		RepoVersionTag tag;

	public:
		RepositoryRecord(const char* id, const char* name, u32 version, u32 date, const char* author,
				const char* url, const char* desc);
		~RepositoryRecord();

		char* GetId();
		char* GetName();
		u32 GetVersion();
		void GetVersionString(char* out);
		u32 GetDate();
		char* GetAuthor();
		char* GetUrl();
		char* GetDesc();

		RepoVersionTag GetTag();
		void SetTag(RepoVersionTag tag);
};

class Repository {
	private:
		vector<RepositoryRecord*> records;

		void Add(RepositoryRecord* record);

	public:
		Repository();
		~Repository();

		void Clear();
		bool AddAll(const char* repoUrl, const char* file);
		bool AddAll(const char* repoUrl, CsvFile* csvFile);
		void CompareWithInstall(const char* installFolder);

		u16 GetNumberOfRecords();
		RepositoryRecord* GetRecord(const char* id);
		RepositoryRecord* GetRecord(u16 index);
};

#endif
