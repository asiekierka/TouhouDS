#include "repository.h"
#include "csv_parser.h"
#include "../tcommon/parser/http.h"
#include "../tcommon/parser/ini_parser.h"

#define REPOSITORY_NUMBER_OF_FIELDS 7
//id;name;version;date;author;url;desc

//------------------------------------------------------------------------------
RepositoryRecord::RepositoryRecord(const char* id, const char* name, u32 version, u32 date,
		const char* author, const char* url, const char* desc)
{
	this->id = strdup(id);
	this->name = strdup(name);
	this->version = version;
	this->date = date;
	this->author = strdup(author);
	this->url = strdup(url);
	this->desc = strdup(desc);

	this->tag = RVT_unknown;
}
RepositoryRecord::~RepositoryRecord() {
	free(id);
	free(name);
	free(author);
	free(url);
	free(desc);
}

char* RepositoryRecord::GetId() {
	return id;
}
char* RepositoryRecord::GetName() {
	return name;
}
u32 RepositoryRecord::GetVersion() {
	return version;
}
void RepositoryRecord::GetVersionString(char* out) {
	versionIntToString(out, version);
}
u32 RepositoryRecord::GetDate() {
	return date;
}
char* RepositoryRecord::GetAuthor() {
	return author;
}
char* RepositoryRecord::GetUrl() {
	return url;
}
char* RepositoryRecord::GetDesc() {
	return desc;
}

RepoVersionTag RepositoryRecord::GetTag() {
	return tag;
}
void RepositoryRecord::SetTag(RepoVersionTag tag) {
	this->tag = tag;
}

//------------------------------------------------------------------------------

Repository::Repository() {
}
Repository::~Repository() {
	Clear();
}

void Repository::Clear() {
    u32 recL = records.size();
    RepositoryRecord* rec[recL];
    for (u32 n = 0; n < recL; n++) {
        rec[n] = records[n];
    }
    records.clear();

    for (u32 n = 0; n < recL; n++) {
        delete rec[n];
    }
}

bool Repository::AddAll(const char* repoUrl, const char* filename) {
	CsvFile csvFile;
	if (csvFile.Load(filename)) {
		return AddAll(repoUrl, &csvFile);
	}
	return false;
}
bool Repository::AddAll(const char* repoUrl, CsvFile* file) {
	u16 records = file->GetNumberOfRecords();
	for (u16 n = 0; n < records; n++) {
		CsvRecord* r = file->GetRecord(n);
		if (r->GetNumberOfFields() != REPOSITORY_NUMBER_OF_FIELDS) {
			printf("Number of fields: %d. records: %d\n", r->GetNumberOfFields(), records);
			return false;
		}
	}

	for (u16 n = 0; n < records; n++) {
	    CsvRecord* r = file->GetRecord(n);

	    char url[256];
		for (u16 x = 0; x < r->GetNumberOfFields(); x++) {
			toAbsoluteUrl(url, repoUrl, r->AsString(5));

			Add(new RepositoryRecord(r->AsString(0), r->AsString(1), versionStringToInt(r->AsString(2)),
					r->AsInt(3), r->AsString(4), url, r->AsString(6)));
		}
	}
	return true;
}

void Repository::Add(RepositoryRecord* r) {
	for (u32 n = 0; n < records.size(); n++) {
		if (strcmp(records[n]->GetId(), r->GetId()) == 0) {
			if (r->GetVersion() > records[n]->GetVersion()) {
				RepositoryRecord* old = records[n];
				records[n] = r;
				delete old;
			} else {
				delete r;
			}
			return;
		}
	}

	records.push_back(r);
}

void Repository::CompareWithInstall(const char* installFolder) {
	FileList fileList(installFolder, ".ini");
	if (fileList.GetFilesL() <= 0) {
		printf("Can't find installFolder\n");
		return;
	}

	IniFile iniFile;
	char id[PATH_MAX];
	for (s16 n = 0; n < fileList.GetFilesL(); n++) {
		char* filename = fileList.NextFile();
        char* extpos = strrchr(filename, '.');
		sprintf(id, "%s/%s", installFolder, filename);
		if (extpos && iniFile.Load(id)) {
	        strncpy(id, filename, extpos-filename);
	        id[extpos-filename] = '\0';

	        IniRecord* r = iniFile.GetRecord("version");
	        if (r) {
	        	u32 version = versionStringToInt(r->GetValue());
	        	RepositoryRecord* rr = GetRecord(id);

	        	if (rr) {
					if (version > rr->GetVersion()) rr->SetTag(RVT_newerInstalled);
					else if (version == rr->GetVersion()) rr->SetTag(RVT_installed);
					else if (version < rr->GetVersion()) rr->SetTag(RVT_olderInstalled);
	        	}
	        }
		} else {
			printf("Can't open %s, id=%s\n", filename, id);
		}
	}
}

u16 Repository::GetNumberOfRecords() {
	return records.size();
}

RepositoryRecord* Repository::GetRecord(const char* id) {
	for (u32 n = 0; n < records.size(); n++) {
		if (strcmp(records[n]->GetId(), id) == 0) {
			return records[n];
		}
	}
	return NULL;
}
RepositoryRecord* Repository::GetRecord(u16 index) {
	return records[index];
}

//------------------------------------------------------------------------------
