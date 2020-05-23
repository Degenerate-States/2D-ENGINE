#pragma once
#include <SDL.h>
#include <iostream>// temp for errors

#define MUS_PATH "assets/music/metal.wav"
#define VOLUME SDL_MIX_MAXVOLUME/3

// sound setup and start thread
void start_audio_thread();
// sdl audio callbac
void my_audio_callback(void *userdata, Uint8 *stream, int len);
void audio_clean();