class Video {
public:
  static const char *Handle;
  static const char *Synchronize;
  static const char *Filter;
  static const char *Shader;

  enum Filter {
    FilterPoint,
    FilterLinear,
  };

  virtual bool cap(const nall::string& name) { return false; }
  virtual nall::any get(const nall::string& name) { return false; }
  virtual bool set(const nall::string& name, const nall::any& value) { return false; }
  Video() {}
  virtual ~Video() {}
};
