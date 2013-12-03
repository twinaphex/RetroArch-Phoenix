//audio.pulseaudio (2010-01-05)
//author: RedDwarf

#include <pulse/pulseaudio.h>

namespace ruby {

class pAudioPulseAudio {
public:
  struct {
    pa_mainloop *mainloop;
    pa_context *context;
    pa_stream *stream;
    pa_sample_spec spec;
    pa_buffer_attr buffer_attr;
    bool first;
  } device;

  struct {
    uint32_t *data;
    size_t size;
    unsigned offset;
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
  }

  any get(const string& name) {
    if(name == Audio::Synchronize) return settings.synchronize;
    if(name == Audio::Frequency) return settings.frequency;
    if(name == Audio::Latency) return settings.latency;
  }

  bool set(const string& name, const any& value) {
    if(name == Audio::Synchronize) {
      settings.synchronize = any_cast<bool>(value);
      return true;
    }

    if(name == Audio::Frequency) {
      settings.frequency = any_cast<unsigned>(value);
      if(device.stream) {
        pa_operation_unref(pa_stream_update_sample_rate(device.stream, settings.frequency, NULL, NULL));
      }
      return true;
    }

    if(name == Audio::Latency) {
      settings.latency = any_cast<unsigned>(value);
      if(device.stream) {
        device.buffer_attr.tlength = pa_usec_to_bytes(settings.latency * PA_USEC_PER_MSEC, &device.spec);
        pa_stream_set_buffer_attr(device.stream, &device.buffer_attr, NULL, NULL);
      }
      return true;
    }
  }

  void clear() {
  }

  pAudioPulseAudio() {
    device.mainloop = 0;
    device.context = 0;
    device.stream = 0;
    buffer.data = 0;
    settings.synchronize = false;
    settings.frequency = 22050;
    settings.latency = 60;
  }

  ~pAudioPulseAudio() {
  }
};

DeclareAudio(PulseAudio)

}
