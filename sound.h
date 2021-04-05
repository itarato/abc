#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL.h>

#include "util.h"

class SoundEffect {
 private:
  SDL_AudioSpec wav_spec;
  uint32_t wav_length;
  uint8_t *wav_buffer;
  SDL_AudioDeviceID device_id;

 public:
  SoundEffect(const char *effect_file_name);
  ~SoundEffect();
  void play();
  void stop();
};

#endif
