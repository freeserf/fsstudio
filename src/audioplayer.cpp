/*
 * audioplayer.cpp - FSSAudioPlayer implementation
 *
 * Copyright (C) 2026  Wicked_Digger <wicked_digger@mail.ru>
 *
 * This file is part of FSStudio.
 *
 * FSStudio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FSStudio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FSStudio.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "src/audioplayer.h"

#include <SDL3/SDL.h>
#ifdef FSSTUDIO_HAVE_MIXER
#include <SDL3_mixer/SDL_mixer.h>
#endif

#include <algorithm>

#include "src/buffer.h"

#ifdef FSSTUDIO_HAVE_MIXER
namespace {

// Music is decoded a bit every frame so the UI stays responsive.
const int decode_chunk_size = 256 * 1024;
// Some modules loop forever, the decoding stops after this much.
const double max_music_duration = 20. * 60.;

}  // namespace
#endif

FSSAudioPlayer::FSSAudioPlayer()
  : stream(nullptr)
#ifdef FSSTUDIO_HAVE_MIXER
  , decoder(nullptr)
#endif
  , spec()
  , playing(false)
  , start_position(0.)
  , start_ticks(0) {
}

FSSAudioPlayer::~FSSAudioPlayer() {
  clear();
}

bool
FSSAudioPlayer::open_stream() {
  stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
                                     nullptr, nullptr);
  if (stream == nullptr) {
    SDL_Log("Failed to open audio device: %s", SDL_GetError());
    samples.clear();
    return false;
  }
  return true;
}

bool
FSSAudioPlayer::load_wav(PBuffer data) {
  clear();
  if (!data) {
    return false;
  }

  SDL_IOStream *io = SDL_IOFromConstMem(data->get_data(), data->get_size());
  Uint8 *buffer = nullptr;
  Uint32 length = 0;
  if (!SDL_LoadWAV_IO(io, true, &spec, &buffer, &length)) {
    SDL_Log("Failed to decode sound: %s", SDL_GetError());
    return false;
  }
  samples.assign(buffer, buffer + length);
  SDL_free(buffer);

  return open_stream();
}

bool
FSSAudioPlayer::can_play_music() {
#ifdef FSSTUDIO_HAVE_MIXER
  return true;
#else
  return false;
#endif
}

bool
FSSAudioPlayer::load_music(PBuffer data, const std::string &soundfont) {
  clear();
#ifdef FSSTUDIO_HAVE_MIXER
  if (!data || !MIX_Init()) {
    return false;
  }

  SDL_PropertiesID props = SDL_CreateProperties();
  if (!soundfont.empty()) {
    SDL_SetStringProperty(props, "SDL_mixer.decoder.fluidsynth.soundfont_path",
                          soundfont.c_str());
  }
  // the decoder reads the data while it is decoding, keep it alive
  music_data = data;
  SDL_IOStream *io = SDL_IOFromConstMem(data->get_data(), data->get_size());
  decoder = MIX_CreateAudioDecoder_IO(io, true, props);
  SDL_DestroyProperties(props);
  if (decoder == nullptr) {
    SDL_Log("Failed to decode music: %s", SDL_GetError());
    music_data = nullptr;
    MIX_Quit();
    return false;
  }

  spec.format = SDL_AUDIO_S16;
  spec.channels = 2;
  spec.freq = 44100;
  if (!open_stream()) {
    clear();
    return false;
  }
  decode_chunk();
  return true;
#else
  (void)data;
  (void)soundfont;
  return false;
#endif
}

bool
FSSAudioPlayer::is_decoding() const {
#ifdef FSSTUDIO_HAVE_MIXER
  return decoder != nullptr;
#else
  return false;
#endif
}

void
FSSAudioPlayer::decode_chunk() {
#ifdef FSSTUDIO_HAVE_MIXER
  if (decoder == nullptr) {
    return;
  }

  size_t old_size = samples.size();
  samples.resize(old_size + decode_chunk_size);
  int decoded = MIX_DecodeAudio(decoder, samples.data() + old_size,
                                decode_chunk_size, &spec);
  samples.resize(old_size + std::max(decoded, 0));

  if (decoded > 0 && playing) {
    SDL_PutAudioStreamData(stream, samples.data() + old_size, decoded);
  }
  if (decoded <= 0 || get_duration() >= max_music_duration) {
    MIX_DestroyAudioDecoder(decoder);
    decoder = nullptr;
    music_data = nullptr;
    MIX_Quit();
    if (playing) {
      SDL_FlushAudioStream(stream);
    }
  }
#endif
}

void
FSSAudioPlayer::clear() {
#ifdef FSSTUDIO_HAVE_MIXER
  if (decoder != nullptr) {
    MIX_DestroyAudioDecoder(decoder);
    decoder = nullptr;
    MIX_Quit();
  }
  music_data = nullptr;
#endif
  if (stream != nullptr) {
    SDL_DestroyAudioStream(stream);
    stream = nullptr;
  }
  samples.clear();
  playing = false;
  start_position = 0.;
}

double
FSSAudioPlayer::get_duration() const {
  int frame_size = SDL_AUDIO_FRAMESIZE(spec);
  if (frame_size == 0 || spec.freq == 0) {
    return 0.;
  }
  return static_cast<double>(samples.size() / frame_size) / spec.freq;
}

double
FSSAudioPlayer::get_position() const {
  if (!playing) {
    return start_position;
  }
  double elapsed = static_cast<double>(SDL_GetTicksNS() - start_ticks) / 1e9;
  return std::min(start_position + elapsed, get_duration());
}

void
FSSAudioPlayer::play(double from) {
  if (stream == nullptr) {
    return;
  }

  int frame_size = SDL_AUDIO_FRAMESIZE(spec);
  size_t frame = static_cast<size_t>(std::max(from, 0.) * spec.freq);
  size_t offset = std::min(frame * frame_size, samples.size());

  SDL_PauseAudioStreamDevice(stream);
  SDL_ClearAudioStream(stream);
  SDL_PutAudioStreamData(stream, samples.data() + offset,
                         static_cast<int>(samples.size() - offset));
  if (!is_decoding()) {
    SDL_FlushAudioStream(stream);
  }
  SDL_ResumeAudioStreamDevice(stream);

  playing = true;
  start_position = static_cast<double>(offset / frame_size) / spec.freq;
  start_ticks = SDL_GetTicksNS();
}

void
FSSAudioPlayer::stop() {
  if (stream != nullptr) {
    SDL_PauseAudioStreamDevice(stream);
    SDL_ClearAudioStream(stream);
  }
  playing = false;
  start_position = 0.;
}

void
FSSAudioPlayer::seek(double position) {
  if (playing) {
    play(position);
  } else {
    start_position = std::clamp(position, 0., get_duration());
  }
}

void
FSSAudioPlayer::update() {
  decode_chunk();
  if (playing && !is_decoding() && SDL_GetAudioStreamQueued(stream) == 0 &&
      get_position() >= get_duration()) {
    stop();
  }
}
