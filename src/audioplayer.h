/*
 * audioplayer.h - FSSAudioPlayer declaration
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

#ifndef SRC_AUDIOPLAYER_H_
#define SRC_AUDIOPLAYER_H_

#include <SDL3/SDL_audio.h>

#include <cstdint>
#include <string>
#include <vector>

#include "src/data-source.h"

#ifdef FSSTUDIO_HAVE_MIXER
typedef struct MIX_AudioDecoder MIX_AudioDecoder;
#endif

// Plays a WAV buffer, or music (MOD and MIDI) decoded with SDL3_mixer,
// through SDL3 audio.
class FSSAudioPlayer {
 protected:
  SDL_AudioStream *stream;
#ifdef FSSTUDIO_HAVE_MIXER
  MIX_AudioDecoder *decoder;  // music still being decoded
  PBuffer music_data;
#endif
  SDL_AudioSpec spec;
  std::vector<uint8_t> samples;
  bool playing;
  double start_position;  // seconds
  uint64_t start_ticks;   // ns

 public:
  FSSAudioPlayer();
  virtual ~FSSAudioPlayer();

  bool load_wav(PBuffer data);
  // MIDI needs a SoundFont, MOD does not.
  bool load_music(PBuffer data, const std::string &soundfont);
  void clear();

  static bool can_play_music();

  bool is_loaded() const { return stream != nullptr; }
  bool is_decoding() const;
  bool is_playing() const { return playing; }
  double get_duration() const;
  double get_position() const;

  void play(double from);
  void play() { play(start_position); }
  void stop();
  void seek(double position);

  // Call every frame to decode music and to detect the end of playback.
  void update();

 protected:
  bool open_stream();
  void decode_chunk();
};

#endif  // SRC_AUDIOPLAYER_H_
