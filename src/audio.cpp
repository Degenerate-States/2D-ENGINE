#include "audio.h"

// Global audio object
Audio* audio = new Audio();

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
        //cout << song->path << endl;
        for (int i = 0; i < MAX_PLAYHEADS; ++i){
            if(song->heads[i].on){
                //cout << "on" << endl;

                len = ( len > song->heads[i].len ? song->heads[i].len  : len );

                // mix from one buffer into another
                SDL_MixAudio(stream, song->heads[i].pos, len, VOLUME);
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
    //cout << "frame done ---" << endl;
    //exit(0);
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
    
    this->fx.push_back(load_wav(PEW1));
    this->fx.push_back(load_wav(PEW2));

    if (SDL_Init(SDL_INIT_AUDIO) < 0){
        std::cerr << "SDL couldn't initialize audio: " << SDL_GetError() << std::endl;
        exit(-1);
    }


   
}

Wav* Audio::load_wav(char* path){
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
        wav->heads[i].on = false;
    }
    return wav;
}

void Audio::playSound(char* path){
    for (auto song : this->fx){
        //cout << path << " " << song->path<< endl;
        if (strcmp(song->path, path) == 0){
            for (int i = 0; i < MAX_PLAYHEADS; ++i){
                if (!song->heads[i].on) {
                    song->heads[i].on = true;
                    break;
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

void pauseAudio() {
    SDL_PauseAudio(1);
}

void unpauseAudio() {
    SDL_PauseAudio(0);
}


