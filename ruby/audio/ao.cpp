/*
  audio.ao (2008-06-01)
  authors: Nach, RedDwarf
*/

#include <ao/ao.h>

namespace ruby {

class pAudioAO {
public:
  int driver_id;
  ao_sample_format driver_format;
  ao_device *audio_device;

  struct {
    unsigned frequency;
  } settings;

  bool cap(const string& name) {
    if(name == Audio::Frequency) return true;
    return false;
  }

  any get(const string& name) {
    if(name == Audio::Frequency) return settings.frequency;
    return false;
  }

  bool set(const string& name, const any& value) {
    if(name == Audio::Frequency) {
      settings.frequency = any_cast<unsigned>(value);
      if(audio_device) init();
      return true;
    }

    return false;
  }

  void sample(uint16_t l_sample, uint16_t r_sample) {
    uint32_t samp = (l_sample << 0) + (r_sample << 16);
    ao_play(audio_device, (char*)&samp, 4); //This may need to be byte swapped for Big Endian
  }

  void clear() {
  }

  pAudioAO() {
    audio_device = 0;
    ao_initialize();

    settings.frequency = 22050;
  }

  ~pAudioAO() {
  }
};

DeclareAudio(AO)

};
