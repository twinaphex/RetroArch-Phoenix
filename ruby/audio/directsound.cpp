/*
  audio.directsound (2007-12-26)
  author: byuu
*/

#include <dsound.h>

namespace ruby {

class pAudioDS {
public:
  LPDIRECTSOUND ds;
  LPDIRECTSOUNDBUFFER dsb_p, dsb_b;
  DSBUFFERDESC dsbd;
  WAVEFORMATEX wfx;

  struct {
    unsigned rings;
    unsigned latency;

    uint32_t *buffer;
    unsigned bufferoffset;

    unsigned readring;
    unsigned writering;
    int distance;
  } device;

  struct {
    HWND handle;
    bool synchronize;
    unsigned frequency;
    unsigned latency;
  } settings;

  bool cap(const string& name) {
    if(name == Audio::Handle) return true;
    if(name == Audio::Synchronize) return true;
    if(name == Audio::Frequency) return true;
    if(name == Audio::Latency) return true;
    return false;
  }

  any get(const string& name) {
    if(name == Audio::Handle) return (uintptr_t)settings.handle;
    if(name == Audio::Synchronize) return settings.synchronize;
    if(name == Audio::Frequency) return settings.frequency;
    if(name == Audio::Latency) return settings.latency;
    return false;
  }

  bool set(const string& name, const any& value) {
    if(name == Audio::Handle) {
      settings.handle = (HWND)any_cast<uintptr_t>(value);
      return true;
    }

    if(name == Audio::Synchronize) {
      settings.synchronize = any_cast<bool>(value);
      if(ds) clear();
      return true;
    }

    if(name == Audio::Frequency) {
      settings.frequency = any_cast<unsigned>(value);
      if(ds) init();
      return true;
    }

    if(name == Audio::Latency) {
      settings.latency = any_cast<unsigned>(value);
      if(ds) init();
      return true;
    }

    return false;
  }

  void clear() {
    device.readring  = 0;
    device.writering = device.rings - 1;
    device.distance  = device.rings - 1;

    device.bufferoffset = 0;
    if(device.buffer) memset(device.buffer, 0, device.latency * device.rings * 4);

    if(!dsb_b) return;
    dsb_b->Stop();
    dsb_b->SetCurrentPosition(0);

    DWORD size;
    void *output;
    dsb_b->Lock(0, device.latency * device.rings * 4, &output, &size, 0, 0, 0);
    memset(output, 0, size);
    dsb_b->Unlock(output, size, 0, 0);

    dsb_b->Play(0, 0, DSBPLAY_LOOPING);
  }

  pAudioDS() {
    ds = 0;
    dsb_p = 0;
    dsb_b = 0;

    device.buffer = 0;
    device.bufferoffset = 0;
    device.readring = 0;
    device.writering = 0;
    device.distance = 0;

    settings.handle = GetDesktopWindow();
    settings.synchronize = false;
    settings.frequency = 22050;
    settings.latency = 120;
  }
};

DeclareAudio(DS)

};
