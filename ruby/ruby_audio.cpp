const char *Audio::Volume = "Volume";
const char *Audio::Resample = "Resample";
const char *Audio::ResampleRatio = "ResampleRatio";

const char *Audio::Handle = "Handle";
const char *Audio::Synchronize = "Synchronize";
const char *Audio::Frequency = "Frequency";
const char *Audio::Latency = "Latency";

bool AudioInterface::init() {
  if(!p) driver();
  return p->init();
}

void AudioInterface::term() {
  if(p) {
    delete p;
    p = 0;
  }
}

bool AudioInterface::cap(const string& name) {
  if(name == Audio::Volume) return true;
  if(name == Audio::Resample) return true;
  if(name == Audio::ResampleRatio) return true;

  return p ? p->cap(name) : false;
}

any AudioInterface::get(const string& name) {
  if(name == Audio::Volume) return volume;
  if(name == Audio::Resample) return resample_enabled;
  if(name == Audio::ResampleRatio);

  return p ? p->get(name) : false;
}

bool AudioInterface::set(const string& name, const any& value) {
  if(name == Audio::Volume) {
    volume = any_cast<unsigned>(value);
    return true;
  }

  if(name == Audio::Resample) {
    resample_enabled = any_cast<bool>(value);
    return true;
  }

  if(name == Audio::ResampleRatio) {
    r_step = any_cast<double>(value);
    r_frac = 0;
    return true;
  }

  return p ? p->set(name, value) : false;
}

void AudioInterface::sample(uint16_t left, uint16_t right) {
}

void AudioInterface::clear() {
}

AudioInterface::AudioInterface() {
}

AudioInterface::~AudioInterface() {
  term();
}
