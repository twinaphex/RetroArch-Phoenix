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
  return p ? p->cap(name) : false;
}

any AudioInterface::get(const string& name) {
  return p ? p->get(name) : false;
}

bool AudioInterface::set(const string& name, const any& value) {
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
