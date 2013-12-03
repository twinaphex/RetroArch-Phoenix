/*
  audio.oss (2007-12-26)
  author: Nach
*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

//OSS4 soundcard.h includes below SNDCTL defines, but OSS3 does not
//However, OSS4 soundcard.h does not reside in <sys/>
//Therefore, attempt to manually define SNDCTL values if using OSS3 header
//Note that if the defines below fail to work on any specific platform, one can point soundcard.h
//above to the correct location for OSS4 (usually /usr/lib/oss/include/sys/soundcard.h)
//Failing that, one can disable OSS4 ioctl calls inside init() and remove the below defines

#ifndef SNDCTL_DSP_COOKEDMODE
  #define SNDCTL_DSP_COOKEDMODE _IOW('P', 30, int)
#endif

#ifndef SNDCTL_DSP_POLICY
  #define SNDCTL_DSP_POLICY _IOW('P', 45, int)
#endif

namespace ruby {

class pAudioOSS {
public:
  struct {
    int fd;
    int format;
    int channels;
    const char *name;
  } device;

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

  pAudioOSS() {
    device.fd = -1;
    device.format = AFMT_S16_LE;
    device.channels = 2;
    device.name = "/dev/dsp";

    settings.frequency = 22050;
  }

  ~pAudioOSS() {
  }
};

DeclareAudio(OSS)

};
