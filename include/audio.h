#pragma once
#include <sdl.h>
#include <iostream>// temp for errors
#include <cstring>// strcmp
// containers
#include <vector>

using namespace std;

// TEMP
#define PEW1 "assets/sound/fx/door1.wav"
#define PEW2 "assets/sound/fx/door2.wav"
#define VOLUME SDL_MIX_MAXVOLUME/4

// maximum number of concurent playheads per wav file
#define MAX_PLAYHEADS 50

/* Native WAVE format */
/* SDL_AudioFormat of files, such as s16 little endian */
#define AUDIO_FORMAT AUDIO_S16LSB

/* Frequency of the file */
#define AUDIO_FREQUENCY 48000

/* 1 mono, 2 stereo, 4 quad, 6 (5.1) */
#define AUDIO_CHANNELS 2

/* Specifies a unit of audio data to be used at a time. Must be a power of 2 */
#define AUDIO_SAMPLES 4096


/* Flags OR'd together, which specify how SDL should behave when a device cannot offer a specific feature
 * If flag is set, SDL will change the format in the actual audio file structure (as opposed to gDevice->want)
 *
 * Note: If you're having issues with Emscripten / EMCC play around with these flags
 *
 * 0                                    Allow no changes
 * SDL_AUDIO_ALLOW_FREQUENCY_CHANGE     Allow frequency changes (e.g. AUDIO_FREQUENCY is 48k, but allow files to play at 44.1k
 * SDL_AUDIO_ALLOW_FORMAT_CHANGE        Allow Format change (e.g. AUDIO_FORMAT may be S32LSB, but allow wave files of S16LSB to play)
 * SDL_AUDIO_ALLOW_CHANNELS_CHANGE      Allow any number of channels (e.g. AUDIO_CHANNELS being 2, allow actual 1)
 * SDL_AUDIO_ALLOW_ANY_CHANGE           Allow all changes above
 */
#define SDL_AUDIO_ALLOW_CHANGES SDL_AUDIO_ALLOW_ANY_CHANGE


void fx_callback(void *userdata, Uint8 *stream, int len);

// Pinters into a wave buffer
struct PlayHead {
	uint8_t *pos; // pointer to position in audio buffer
	uint32_t len; // remaining length of the sample
    bool on = false; // enable flag
	uint8_t volume = SDL_MIX_MAXVOLUME; //0-127
	// pan?
	// loop?
	// fade?
};

// Wave buffer structure
// TODO: create unique structures for songs and fx waves
struct Wav {
	uint8_t *buffer; // pointer to allocated buffer
	uint32_t len; // length of buffer
	char const *path; // fx path
	PlayHead heads[MAX_PLAYHEADS];
};

// Global audio object
class Audio {
public:
	~Audio();

	static Audio* instance();

	//disable copying
	Audio(const Audio&);
    Audio& operator= (const Audio&);

	//TODO: replace with map
	vector<Wav*> fx;

    SDL_AudioSpec device;
	bool paused = false;

    void init();
	void clean();
    Wav* load_wav(char const *path, uint8_t vol);
	void playSound(char const *path, uint8_t vol);
	void unpauseAudio();
	void pauseAudio();
private:
	Audio() { }
};

// SDL audio spec
//typedef struct SDL_AudioSpec
//{
//    int freq;                   /**< DSP frequency -- samples per second */
//    SDL_AudioFormat format;     /**< Audio data format */
//    Uint8 channels;             /**< Number of channels: 1 mono, 2 stereo */