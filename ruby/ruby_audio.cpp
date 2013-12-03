const char *Audio::Handle = "Handle";

bool AudioInterface::cap(const string& name) {
  return p ? p->cap(name) : false;
}

any AudioInterface::get(const string& name) {
  return p ? p->get(name) : false;
}

bool AudioInterface::set(const string& name, const any& value) {
  return p ? p->set(name, value) : false;
}

void AudioInterface::clear() {
}

AudioInterface::AudioInterface() {
}

AudioInterface::~AudioInterface() {
}
