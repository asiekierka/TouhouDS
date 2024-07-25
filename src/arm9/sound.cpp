#include "sound.h"

#include "tcommon/filehandle.h"
#include "as_lib9.h"
#include "thcommon.h"

u32* micbuf = NULL;
u32  micbufL = 0;

void startMic(void (*callback) (void*, int), u32 bufferSize) {
	bufferSize >>= 2; //size in words instead of bytes

	if (!micbuf) {
		micbuf = new u32[bufferSize];
	} else if (micbufL < bufferSize) {
		delete[] micbuf;
		micbuf = new u32[bufferSize];
	}

	micbufL = bufferSize;
	soundMicRecord(micbuf, micbufL<<2, MicFormat_8Bit, 12000, callback);
}
void stopMic() {
	soundMicOff();

	if (micbuf) {
		delete[] micbuf;
		micbuf = NULL;
	}
	micbufL = 0;
}

//-----------------------------------------------------------------------------

SoundManager::SoundManager() {
    soundFolder[0] = '\0';
    bgmFilename[0] = '\0';

    micbuf = NULL;
    memoryBuffer = NULL;
    memoryBufferOffset = 0;
    memoryBufferL = 0;

    memoryCacheL = 0;
}

SoundManager::~SoundManager() {
    ClearCache();

    if (micbuf) delete[] micbuf;
    if (memoryBuffer) delete[] memoryBuffer;
}

void SoundManager::ClearCache() {
	StopAll();

	memoryCacheL = 0;
    memoryBufferOffset = 0;
}

void SoundManager::RemoveUserSounds() {
	memoryBufferOffset = systemMemoryBufferOffset;
	memoryCacheL = systemMemoryCacheL;
}

void SoundManager::Init(const char* folder, u32 cachesize) {
    ClearCache();
    if (memoryBuffer) delete[] memoryBuffer;

    strcpy(soundFolder, folder);

    memoryBufferL = cachesize;
    memoryBuffer = new u8[memoryBufferL];
    memoryBufferOffset = 0;
    bgmCooldown = 0;

    struct stat st;
    char path[MAXPATHLEN];
    char filename[MAXPATHLEN];

    DIR_ITER* dir = diropen(soundFolder);
    if (dir) {
        while (dirnext(dir, filename, &st) == 0) {
            if ((st.st_mode & S_IFDIR) == 0) {
                sprintf(path, "%s/%s", folder, filename);

                char* f = strrchr(path, '/');
            	AddFile(path, f ? f+1 : path);
            }
        }
        dirclose(dir);
    }

    systemMemoryBufferOffset = memoryBufferOffset;
    systemMemoryCacheL = memoryCacheL;
}

bool SoundManager::AddFile(const char* path, const char* filename) {

	//Filename to lower case
    //Copy file into cache buffer
    FileHandle* fh = fhOpen(path);
    if (fh) {
        if (memoryCacheL < SFX_CACHE_LEN && memoryBufferOffset + fh->length <= memoryBufferL) {
            //Read data into buffer
            fh->ReadFully(memoryBuffer + memoryBufferOffset);

            //Create entry in cache map:
            SoundRecord& r = memoryCache[memoryCacheL];

            //Key
            int filenameL = MIN(strlen(filename), 127);
            for (int n = 0; n < filenameL; n++) {
            	r.name[n] = tolower(filename[n]);
            }
            r.name[filenameL] = '\0';

            //Value
            r.bufferOffset = memoryBufferOffset;

            //Cooldown
            r.cooldown = 0;

            //Move buffer write offset
            memoryBufferOffset += fh->length;
            memoryCacheL++;

            fhClose(fh);
            return true;
        }
        fhClose(fh);
    }
    return false;
}

//The list of entries doesn't account for duplicates, so user sounds can have the same name
//as built-in sounds without any problem. You just have to search from back to front.
bool SoundManager::GetEntry(const char* id, SoundRecord** rout, u8** data, int* dataL, bool forwardSearch) {
	int start = 0;
	int end = memoryCacheL;
	int inc = 1;
	if (!forwardSearch) {
		start = end;
		end = -1;
		inc = -inc;
	}

	SoundRecord* r = memoryCache + start;
    for (int n = start; n != end; n += inc) {
        if (strcmp(r->name, id) == 0) {
        	*rout = r;

        	u32 dataEnd = (n < memoryCacheL-1 ? (r+1)->bufferOffset : memoryBufferOffset);
            *data = memoryBuffer + r->bufferOffset;
            *dataL = dataEnd - r->bufferOffset;
            return true;
        }
        r += inc;
    }
    return false;
}

int SoundManager::PlayUserSound(const char* folder, const char* filename, u32 cooldown) {
	char path[MAXPATHLEN];
	sprintf(path, "%s/%s", folder, filename);

	SoundRecord* r = NULL;
	u8* data = NULL;
    int dataL = 0;

    if (!GetEntry(filename, &r, &data, &dataL, false)) {
    	AddFile(path, filename);
    	if (!GetEntry(filename, &r, &data, &dataL, false)) {
    		return -1;
    	}
    }

    if (r->cooldown > 0) return -1;
    r->cooldown = cooldown;
    return AS_SoundDefaultPlay(data, dataL, 127, 64, FALSE, 0);
}
int SoundManager::PlaySound(const char* id, u32 cooldown, bool loop, u8 priority) {
	SoundRecord* r = NULL;
    u8* data = NULL;
    int dataL = 0;

    if (!GetEntry(id, &r, &data, &dataL, true)) {
        return -1;
    }

    if (r->cooldown > 0) return -1;
    r->cooldown = cooldown;
    return AS_SoundDefaultPlay(data, dataL, 127, 64, loop ? TRUE : FALSE, priority);
}
void SoundManager::StopSound(int channel) {
    AS_SoundStop(channel);
}

//The sound lib doesn't seem to allow us to stop the current music and immediately
//start a different song. So you should stop the music a while before starting a new song.
void SoundManager::SetBGM(const char* f) {
	if (!f) {
		bgmFilename[0] = '\0';
		StopBGM();
		return;
	}

	StopBGM();
    strcpy(bgmFilename, f);
    bgmCooldown = 20;
}

void SoundManager::StopSFX() {
    //Stop all non-MP3 sound channels
    for (int n = 2; n < 16; n++) {
        AS_SoundStop(n);
    }
}
void SoundManager::StopBGM() {
	bgmCooldown = 0;
    AS_MP3Stop();
}
void SoundManager::StopAll() {
    StopSFX();
    StopBGM();
}

void SoundManager::Update() {
	for (int n = 0; n < memoryCacheL; n++) {
		if (memoryCache[n].cooldown > 0) {
			memoryCache[n].cooldown--;
		}
	}
	if (bgmCooldown > 0) {
		bgmCooldown--;
		if (bgmCooldown == 0) {
		    AS_SetMP3Loop(TRUE);
		    AS_MP3StreamPlay(fhOpen(NULL, bgmFilename));
		}
	}

	AS_SoundVBL();
}
