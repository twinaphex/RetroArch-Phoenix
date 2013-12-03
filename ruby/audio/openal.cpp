/*
  audio.openal (2007-12-26)
  author: Nach
  contributors: byuu, wertigon, _willow_
*/

#if defined(PLATFORM_OSX)
  #include <OpenAL/al.h>
  #include <OpenAL/alc.h>
#else
  #include <AL/al.h>
  #include <AL/alc.h>
#endif

namespace ruby {

class pAudioOpenAL {
public:
  struct {
    ALCdevice *handle;
    ALCcontext *context;
    ALuint source;
    ALenum format;
    unsigned latency;
    unsigned queue_length;
  } device;

  struct {
    uint32_t *data;
    unsigned length;
    unsigned size;
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
      settings.synchronize = any_cast<bool>(value);
      return true;
    }

    if(name == Audio::Frequency) {
      settings.frequency = any_cast<unsigned>(value);
      return true;
    }

    if(name == Audio::Latency) {
      if(settings.latency != any_cast<unsigned>(value)) {
        settings.latency = any_cast<unsigned>(value);
        update_latency();
      }
      return true;
    }

    return false;
  }

  void clear() {
  }

  void update_latency() {
    if(buffer.data) delete[] buffer.data;
    buffer.size = settings.frequency * settings.latency / 1000.0 + 0.5;
    buffer.data = new uint32_t[buffer.size];
  }

  pAudioOpenAL() {
    device.source = 0;
    device.handle = 0;
    device.context = 0;
    device.format = AL_FORMAT_STEREO16;
    device.queue_length = 0;

    buffer.data = 0;
    buffer.length = 0;
    buffer.size = 0;

    settings.synchronize = true;
    settings.frequency = 22050;
    settings.latency = 40;
  }
};

DeclareAudio(OpenAL)

};
