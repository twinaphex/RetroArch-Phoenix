/*
  ruby
  version: 0.06 (2009-05-22)
  license: public domain
*/

#ifndef RUBY_H
#define RUBY_H

#include <nall/algorithm.hpp>
#include <nall/any.hpp>
#include <nall/array.hpp>
#include <nall/bit.hpp>
#include <nall/detect.hpp>
#include <nall/input.hpp>
#include <stdint.h>
#include <nall/string.hpp>
#include <nall/vector.hpp>

namespace ruby {

#include <ruby/input.hpp>

class VideoInterface {
public:
  void driver(const char *driver = "");
  const char* default_driver();
  const char* driver_list();
  VideoInterface();
  ~VideoInterface();
};

class AudioInterface {
public:
  void driver(const char *driver = "");
  const char* default_driver();
  const char* driver_list();
  AudioInterface();
  ~AudioInterface();
};

class InputInterface {
public:
  void driver(const char *driver = "");
  const char* default_driver();
  const char* driver_list();
  bool init();
  void term();

  bool cap(const nall::string& name);
  nall::any get(const nall::string& name);
  bool set(const nall::string& name, const nall::any& value);

  bool acquire();
  bool unacquire();
  bool acquired();

  bool poll(int16_t *table);
  InputInterface();
  ~InputInterface();

private:
  Input *p;
};

extern VideoInterface video;
extern AudioInterface audio;
extern InputInterface input;

};

#endif
