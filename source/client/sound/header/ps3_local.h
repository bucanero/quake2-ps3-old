// Addition to link ps3_sound_backend to ps3_sound
#ifndef _PS3_SOUND_LOCAL_H_
#define _PS3_SOUND_LOCAL_H_

// Those functions are implemented from 0
qboolean SB_PS3_BackendInit(void);
void SB_PS3_BackendShutdown(void);

// Those functions or slightly changed copies 
// of SDL_* origins or not changed at all
void SB_ClearBuffer(void);
qboolean SB_Cache(sfx_t *sfx, wavinfo_t *info, byte *data, short volume,
				 int begin_length, int  end_length,
				 int attack_length, int fade_length);
void SB_Spatialize(channel_t *ch);
int  SB_DriftBeginofs(float);
void SB_RawSamples(int samples, int rate, int width,
		int channels, byte *data, float volume);
void SB_Update(void);
void SB_SoundInfo(void);


#endif
