#include "audio.h"


void printSpec(SDL_AudioSpec s){
    cout << "---Audio Spec:" << endl;
    cout << s.freq << endl;
    cout << s.format << endl;
    cout << s.channels << endl;
    cout << s.samples << endl;
    cout << "---" << endl;
}


void fx_callback(void *userdata, Uint8 *stream, int len) {
    Audio* audio = (Audio*)userdata;
    // clear buffer
    SDL_memset(stream, 0, len);
    for (auto song : audio->fx){
        for (int i = 0; i < MAX_PLAYHEADS; ++i){
            if(song->heads[i].on){
                len = ( len > (int)song->heads[i].len ? song->heads[i].len  : len );

                // mix from one buffer into another
                SDL_MixAudio(stream, song->heads[i].pos, len, song->heads[i].volume);
                song->heads[i].pos += len;
                song->heads[i].len -= len;
                if (song->heads[i].len <= 0){
                    song->heads[i].len = song->len;
                    song->heads[i].pos = song->buffer;
                    song->heads[i].on = false;
                }
            }
        }
    }
}

Audio* Audio::instance() {
    static Audio* a = new Audio();
    return a;
}

void Audio::init() {
    device.freq = AUDIO_FREQUENCY;
    device.format = AUDIO_FORMAT;
    device.channels = AUDIO_CHANNELS;
    device.samples = AUDIO_SAMPLES;
    device.callback = fx_callback;
    device.userdata = this;

    if ( SDL_OpenAudio(&device, NULL) < 0 ){
      std::cerr << "Couldn't open audio: " << SDL_GetError() << std::endl;
      exit(-1);
    }
    
    this->fx.push_back(load_wav(PEW1, VOLUME));
    this->fx.push_back(load_wav(PEW2, VOLUME));

    if (SDL_Init(SDL_INIT_AUDIO) < 0){
        std::cerr << "SDL couldn't initialize audio: " << SDL_GetError() << std::endl;
        exit(-1);
    }
    paused = true;
}

Wav* Audio::load_wav(char const *path, uint8_t vol){
    Wav* wav = new Wav();
    
    SDL_AudioSpec s;

    if( SDL_LoadWAV(path, &s, &wav->buffer, &wav->len) == NULL ){
        std::cerr << "Couldn't load wave: " << SDL_GetError() << std::endl;
        exit(-1);
    }

    wav->path = path;
    for (int i = 0; i<MAX_PLAYHEADS; ++i){
        wav->heads[i].pos = wav->buffer;
        wav->heads[i].len = wav->len;
        wav->heads[i].volume = vol;
        wav->heads[i].on = false;
    }
    return wav;
}

void Audio::playSound(char const *path, uint8_t vol){
    if (!paused) {
        for (auto fx : this->fx){
            if (strcmp(fx->path, path) == 0){
                for (int i = 0; i < MAX_PLAYHEADS; ++i){
                    if (!fx->heads[i].on) {
                        fx->heads[i].volume = vol;
                        fx->heads[i].on = true;
                        break;
                    }
                }
            }
        }
    }
}

void Audio::clean() {
    SDL_CloseAudio();
    for (auto song : this->fx)
        SDL_FreeWAV(song->buffer);
}

void Audio::pauseAudio() {
    paused = true; 
    SDL_PauseAudio(1);
}

void Audio::unpauseAudio() {
    paused = false; 
    SDL_PauseAudio(0);
}


