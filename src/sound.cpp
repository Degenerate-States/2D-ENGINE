#include "sound.h"


// TODO: move audio into a class
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play
static Uint32 wav_length; // length of our sample
static Uint8 *wav_buffer; // buffer containing our audio file
static SDL_AudioSpec wav_spec; // the specs of our piece of music



static int playMusic(void *ptr);

void init_audio();

void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	
	if (audio_len ==0)
		return;
	
	len = ( len > audio_len ? audio_len : len );

	SDL_memset(stream, 0, len);
    // mix from one buffer into another
	SDL_MixAudio(stream, audio_pos, len, VOLUME);
	
	audio_pos += len;
	audio_len -= len;
}

void init_audio() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0){
        std::cerr << "SDL couldn't initialize audio: " << SDL_GetError() << std::endl;
	    exit(-1);
    }

	if( SDL_LoadWAV(MUS_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL ){
        std::cerr << "Couldn't load wave: " << SDL_GetError() << std::endl;
        exit(-1);
	}

	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
    
	audio_pos = wav_buffer;
	audio_len = wav_length;
	
	if ( SDL_OpenAudio(&wav_spec, NULL) < 0 ){
	  std::cerr << "Couldn't open audio: " << SDL_GetError() << std::endl;
	  exit(-1);
	}
	
}

// for audio thread
static int playMusic(void *ptr) {
	SDL_PauseAudio(0);

	while ( audio_len > 0 ) {
		SDL_Delay(10); 
	}

    return 0;
}

void start_audio_thread() {
    init_audio();
    SDL_Thread *audioThread;
    audioThread = SDL_CreateThread(playMusic, "AudioThread", (void *)NULL);
}

void audio_clean() {
    // TEMP audio cleanup
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);
}