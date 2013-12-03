/* Global Headers */

#if defined(PLATFORM_X)
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <X11/Xatom.h>
#elif defined(PLATFORM_OSX)
  #define __INTEL_COMPILER
  #include <Carbon/Carbon.h>
#elif defined(PLATFORM_WIN)
  #define _WIN32_WINNT 0x0501
  #include <windows.h>
#endif

/* Input */

#define DeclareInput(Name) \
  class Input##Name : public Input { \
  public: \
    bool cap(const string& name) { return p.cap(name); } \
    any get(const string& name) { return p.get(name); } \
    bool set(const string& name, const any& value) { return p.set(name, value); } \
    \
    bool acquire() { return p.acquire(); } \
    bool unacquire() { return p.unacquire(); } \
    bool acquired() { return p.acquired(); } \
    \
    bool poll(int16_t *table) { return p.poll(table); } \
    bool init() { return p.init(); } \
    void term() { p.term(); } \
    \
    Input##Name() : p(*new pInput##Name) {} \
    ~Input##Name() { delete &p; } \
  \
  private: \
    pInput##Name &p; \
  };

#ifdef INPUT_DIRECTINPUT
  #include <ruby/input/directinput.cpp>
#endif

#ifdef INPUT_DISDL
  #include <ruby/input/disdl.cpp>
#endif

#ifdef INPUT_RAWINPUT
  #include <ruby/input/rawinput.cpp>
#endif

#ifdef INPUT_SDL
  #include <ruby/input/sdl.cpp>
#endif

#ifdef __linux
  #include <ruby/input/linuxraw.cpp>
#endif

#ifdef INPUT_X
  #include <ruby/input/x.cpp>
#endif

#ifdef INPUT_CARBON
  #include <ruby/input/carbon.cpp>
#endif
