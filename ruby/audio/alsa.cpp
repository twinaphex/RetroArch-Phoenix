//audio.alsa (2009-11-30)
//authors: BearOso, byuu, Nach, RedDwarf

#include <alsa/asoundlib.h>

namespace ruby {

class pAudioALSA {
public:
  struct {
    snd_pcm_t *handle;
    snd_pcm_format_t format;
    snd_pcm_uframes_t buffer_size;
    snd_pcm_uframes_t period_size;
    int channels;
    const char *name;
  } device;

  struct {
    uint32_t *data;
    unsigned length;
  } buffer;

  struct {
    bool synchronize;
    unsigned frequency;
    unsigned latency;
  } settings;

  bool cap(const string& name) {
    if(name == Audio::Synchronize) return true;
    if(name == Audio::Frequency) return true;
    if(name == Audio::Latency) return true;
    return false;
  }

  any get(const string& name) {
    if(name == Audio::Synchronize) return settings.synchronize;
    if(name == Audio::Frequency) return settings.frequency;
    if(name == Audio::Latency) return settings.latency;
    return false;
  }

  bool set(const string& name, const any& value) {
    if(name == Audio::Synchronize) {
      if(settings.synchronize != any_cast<bool>(value)) {
        settings.synchronize = any_cast<bool>(value);
        if(device.handle) init();
      }
      return true;
    }

    if(name == Audio::Frequency) {
      if(settings.frequency != any_cast<unsigned>(value)) {
        settings.frequency = any_cast<unsigned>(value);
        if(device.handle) init();
      }
      return true;
    }

    if(name == Audio::Latency) {
      if(settings.latency != any_cast<unsigned>(value)) {
        settings.latency = any_cast<unsigned>(value);
        if(device.handle) init();
      }
      return true;
    }

    return false;
  }

  void clear() {
  }

  pAudioALSA() {
    device.handle = 0;
    device.format = SND_PCM_FORMAT_S16_LE;
    device.channels = 2;
    device.name = "default";

    buffer.data = 0;
    buffer.length = 0;

    settings.synchronize = false;
    settings.frequency = 22050;
    settings.latency = 60;
  }

  ~pAudioALSA() {
  }
};

DeclareAudio(ALSA)

};
