#ifndef SOUND_H
#define SOUND_H

#include <nds.h>
#include <fat.h>
#include <unistd.h>
#include <sys/dir.h>

#define SFX_CACHE_LEN 128
#define DEFAULT_SOUND_COOLDOWN 5
#define DEFAULT_MICBUF_LENGTH (8<<10)

struct SoundRecord {
	char name[128];
	u32 bufferOffset;
	u32 cooldown;
};

class SoundManager {
    private:
        char   soundFolder[MAXPATHLEN];
        char   bgmFilename[MAXPATHLEN];
        u16    bgmCooldown;

        u8*    memoryBuffer;
        u32    memoryBufferOffset;
        u32    systemMemoryBufferOffset;
        u32    memoryBufferL;

        SoundRecord memoryCache[SFX_CACHE_LEN];
        u16    memoryCacheL;
        u16    systemMemoryCacheL;

        bool   AddFile(const char* path, const char* id);
        bool   GetEntry(const char* filename, SoundRecord** r, u8** data, int* dataL, bool forwardSearch);

    public:
        SoundManager();
        virtual ~SoundManager();

        void ClearCache();
        void RemoveUserSounds();
        void Init(const char* folder, u32 memoryCacheSize=393216); //384KiB
        void Update();
        int  PlaySound(const char* filename, u32 cooldown=DEFAULT_SOUND_COOLDOWN,
        		bool loop=false, u8 priority=0);
        int  PlayUserSound(const char* folder, const char* filename,
        		u32 cooldown=DEFAULT_SOUND_COOLDOWN);
        void StopSound(int channel);
        void SetBGM(const char* filename);
        void StopSFX();
        void StopBGM();
        void StopAll();

};

void startMic(void (*callback) (void*, int), u32 bufferSize=DEFAULT_MICBUF_LENGTH);
void stopMic();

#endif

