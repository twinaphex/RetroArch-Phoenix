//audio.pulseaudiosimple (2010-01-05)
//author: byuu

#include <pulse/simple.h>
#include <pulse/error.h>

namespace ruby {

class pAudioPulseAudioSimple {
public:
  struct {
    pa_simple *handle;
    pa_sample_spec spec;
  } device;

  struct {
    uint32_t *data;
    unsigned offset;
  } buffer;

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
      return true;
    }

    return false;
  }

  void clear() {
  }

  pAudioPulseAudioSimple() {
    device.handle = 0;
    buffer.data = 0;
    settings.frequency = 22050;
  }

  ~pAudioPulseAudioSimple() {
  }
};

DeclareAudio(PulseAudioSimple)

};
