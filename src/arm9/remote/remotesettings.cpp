#include "remotesettings.h"
#include "../tcommon/wifi.h"
#include "../tcommon/parser/ini_parser.h"

RemoteSettings::RemoteSettings() {
	Reset();
}
RemoteSettings::~RemoteSettings() {

}

void RemoteSettings::Reset() {
	ip = 0xC0A801BD;
	port = 28290;
	receiveVoice = true;
	sendVoice = true;

	strcpy(nickname, "anon");
	nickname[NICKNAME_LENGTH] = '\0';
}

bool RemoteSettings::Load(const char* path) {
	IniFile iniFile;
	if (iniFile.Load(path)) {
        IniRecord* r;

        Reset();

        r = iniFile.GetRecord("ip");
        if (r) ip = stringToIP(r->AsString());
        r = iniFile.GetRecord("port");
        if (r) port = r->AsInt();
        r = iniFile.GetRecord("receiveVoice");
        if (r) receiveVoice = r->AsBool();
        r = iniFile.GetRecord("sendVoice");
        if (r) sendVoice = r->AsBool();
        r = iniFile.GetRecord("nickname");
        if (r) {
        	strncpy(nickname, r->AsString(), NICKNAME_LENGTH);
        	nickname[NICKNAME_LENGTH] = '\0';
        }
	} else {
		return false;
	}

	return true;
}
void RemoteSettings::Save(const char* path) {
	FILE* file = fopen(path, "w");
	if (!file) return;

	char temp[32];

	fprintf(file, "ip=%s\n", ipToString(temp, ip));
	fprintf(file, "port=%d\n", port);
	fprintf(file, "receiveVoice=%s\n", receiveVoice ? "true" : "false");
	fprintf(file, "sendVoice=%s\n", receiveVoice ? "true" : "false");
	fprintf(file, "nickname=%s\n", nickname);

	fclose(file);
}
