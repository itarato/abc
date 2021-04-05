#include "sound.h"

SoundEffect::SoundEffect(const char *effect_file_name) {
  if (!SDL_LoadWAV(effect_file_name, &wav_spec, &wav_buffer, &wav_length))
    PANIC("Cannot load wav");

  device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
  if (!device_id) PANIC("Cannot open audio device");
}

SoundEffect::~SoundEffect() {
  SDL_CloseAudioDevice(device_id);
  SDL_FreeWAV(wav_buffer);
}

void SoundEffect::play() {
  stop();
  if (SDL_QueueAudio(device_id, wav_buffer, wav_length) == -1)
    PANIC("Cannot queue audio");

  SDL_PauseAudioDevice(device_id, 0);
}

void SoundEffect::stop() { SDL_ClearQueuedAudio(device_id); }
