class Audio {
public:
  static const char *Handle;
  static const char *Synchronize;
  static const char *Frequency;
  static const char *Latency;

  virtual bool cap(const nall::string& name) { return false; }
  virtual nall::any get(const nall::string& name) { return false; }
  virtual bool set(const nall::string& name, const nall::any& value) { return false; }

  Audio() {}
  virtual ~Audio() {}
};
