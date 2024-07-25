#ifndef REMOTESETTINGS_H
#define REMOTESETTINGS_H

#include "remotecommon.h"

#define NICKNAME_LENGTH 6

class RemoteSettings {
	private:

	public:
		u32 ip;
		u16 port;
		bool receiveVoice;
		bool sendVoice;
		char nickname[NICKNAME_LENGTH+1];

		RemoteSettings();
		virtual ~RemoteSettings();

		void Reset();
		bool Load(const char* path);
		void Save(const char* path);
};

#endif
