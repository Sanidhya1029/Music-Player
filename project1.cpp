#include <iostream>
#include <SDL.h>
#include <SDL_audio.h>

#define BUFFER_SIZE 4096

struct AudioData {
    Uint8* position;
    Uint32 length;
};

void audioCallback(void* userData, Uint8* buffer, int length) {
    AudioData* audioData = (AudioData*)userData;

    if (audioData->length == 0) {
        return;
    }

    length = (length > audioData->length ? audioData->length : length);

    SDL_memcpy(buffer, audioData->position, length);

    audioData->position += length;
    audioData->length -= length;
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 2;
    desiredSpec.samples = BUFFER_SIZE;
    desiredSpec.callback = audioCallback;

    SDL_AudioSpec obtainedSpec;

    AudioData audioData;
    Uint32 audioLength;

    if (SDL_LoadWAV("audiofile.wav", &obtainedSpec, &audioData.position, &audioLength) == NULL) {
        std::cerr << "Failed to load audio file: " << SDL_GetError() << std::endl;
        return 1;
    }

    audioData.length = audioLength;

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);

    if (audioDevice == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_PauseAudioDevice(audioDevice, 0);

    while (audioData.length > 0) {
        SDL_Delay(100);
    }

    SDL_CloseAudioDevice(audioDevice);
    SDL_FreeWAV(audioData.position);

    SDL_Quit();

    return 0;
}
