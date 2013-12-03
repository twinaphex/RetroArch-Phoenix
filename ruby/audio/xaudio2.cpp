/*
  audio.xaudio2 (2010-08-14)
  author: OV2
*/

#include "xaudio2.hpp"
#include <Windows.h>

namespace ruby {

class pAudioXAudio2: public IXAudio2VoiceCallback {
public:
  IXAudio2 *pXAudio2;
  IXAudio2MasteringVoice* pMasterVoice;
  IXAudio2SourceVoice *pSourceVoice;
  
  // inherited from IXAudio2VoiceCallback
  STDMETHODIMP_(void) OnBufferStart(void *pBufferContext){}
  STDMETHODIMP_(void) OnLoopEnd(void *pBufferContext){}
  STDMETHODIMP_(void) OnStreamEnd() {}
  STDMETHODIMP_(void) OnVoiceError(void *pBufferContext, HRESULT Error) {}
  STDMETHODIMP_(void) OnVoiceProcessingPassEnd() {}
  STDMETHODIMP_(void) OnVoiceProcessingPassStart(UINT32 BytesRequired) {}

  struct {
    unsigned buffers;
    unsigned latency;

    uint32_t *buffer;
    unsigned bufferoffset;

    volatile long submitbuffers;
    unsigned writebuffer;
  } device;

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
      if(pXAudio2) clear();
      return true;
    }

    if(name == Audio::Frequency) {
      settings.frequency = any_cast<unsigned>(value);
      return true;
    }

    if(name == Audio::Latency) {
      settings.latency = any_cast<unsigned>(value);
      return true;
    }

    return false;
  }
  
  void pushbuffer(unsigned bytes,uint32_t *pAudioData) {
    XAUDIO2_BUFFER xa2buffer={0};
    xa2buffer.AudioBytes=bytes;
    xa2buffer.pAudioData=reinterpret_cast<BYTE *>(pAudioData);
    xa2buffer.pContext=0;
    InterlockedIncrement(&device.submitbuffers);
    pSourceVoice->SubmitSourceBuffer(&xa2buffer);
  }

  void clear() {
    if(!pSourceVoice) return;
    pSourceVoice->Stop(0);
    pSourceVoice->FlushSourceBuffers();     //calls OnBufferEnd for all currently submitted buffers
    
    device.writebuffer = 0;

    device.bufferoffset = 0;
    if(device.buffer) memset(device.buffer, 0, device.latency * device.buffers * 4);

     pSourceVoice->Start(0);
  }

  STDMETHODIMP_(void) OnBufferEnd(void *pBufferContext) {
    InterlockedDecrement(&device.submitbuffers);
  }

  pAudioXAudio2() {
    pXAudio2 = 0;
    pMasterVoice = 0;
    pSourceVoice = 0;

    device.buffer = 0;
    device.bufferoffset = 0;
    device.submitbuffers = 0;
    device.writebuffer = 0;

    settings.synchronize = false;
    settings.frequency = 22050;
    settings.latency = 120;
  }
};

DeclareAudio(XAudio2)

};
