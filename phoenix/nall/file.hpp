#ifndef NALL_FILE_HPP
#define NALL_FILE_HPP

#include <nall/platform.h>
#include <stdint.h>
#include <nall/string.hpp>
#include <nall/utility.hpp>
#include <nall/windows/utf8.hpp>

namespace nall {
  class file {
  public:
    enum class mode : unsigned { read, write, readwrite, writeread };
    enum class index : unsigned { absolute, relative };
    enum class time : unsigned { create, modify, access };

    static bool read(const string &filename, uint8_t *&data, unsigned &size) {
      file fp;
      if(fp.open(filename, mode::read) == false) return false;
      size = fp.size();
      data = new uint8_t[size];
      fp.read(data, size);
      fp.close();
      return true;
    }

    static bool write(const string &filename, const uint8_t *data, unsigned size) {
      file fp;
      if(fp.open(filename, mode::write) == false) return false;
      fp.write(data, size);
      fp.close();
      return true;
    }

    uint8_t read() {
      if(!fp) return 0xff;                       //file not open
      if(file_mode == mode::write) return 0xff;  //reads not permitted
      if(file_offset >= file_size) return 0xff;  //cannot read past end of file
      buffer_sync();
      return buffer[(file_offset++) & buffer_mask];
    }

    void read(uint8_t *buffer, unsigned length) {
      while(length--) *buffer++ = read();
    }

    void write(uint8_t data) {
      if(!fp) return;                      //file not open
      if(file_mode == mode::read) return;  //writes not permitted
      buffer_sync();
      buffer[(file_offset++) & buffer_mask] = data;
      buffer_dirty = true;
      if(file_offset > file_size) file_size = file_offset;
    }

    void write(const uint8_t *buffer, unsigned length) {
      while(length--) write(*buffer++);
    }

    template<typename... Args> void print(Args... args) {
      string data(args...);
      const char *p = data;
      while(*p) write(*p++);
    }

    int size() {
      if(!fp) return -1;  //file not open
      return file_size;
    }

    static uintmax_t size(const string &filename) {
      #if !defined(_WIN32)
      struct stat data;
      stat(filename, &data);
      #else
      struct __stat64 data;
      _wstat64(utf16_t(filename), &data);
      #endif
      return S_ISREG(data.st_mode) ? data.st_size : 0u;
    }

    bool open() {
      return fp;
    }

    bool open(const string &filename, mode mode_) {
      if(fp) return false;

      switch(file_mode = mode_) {
        #if !defined(_WIN32)
        case mode::read:      fp = fopen(filename, "rb" ); break;
        case mode::write:     fp = fopen(filename, "wb+"); break;  //need read permission for buffering
        case mode::readwrite: fp = fopen(filename, "rb+"); break;
        case mode::writeread: fp = fopen(filename, "wb+"); break;
        #else
        case mode::read:      fp = _wfopen(utf16_t(filename), L"rb" ); break;
        case mode::write:     fp = _wfopen(utf16_t(filename), L"wb+"); break;
        case mode::readwrite: fp = _wfopen(utf16_t(filename), L"rb+"); break;
        case mode::writeread: fp = _wfopen(utf16_t(filename), L"wb+"); break;
        #endif
      }
      if(!fp) return false;
      buffer_offset = -1;  //invalidate buffer
      file_offset = 0;
      fseek(fp, 0, SEEK_END);
      file_size = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      return true;
    }

    void close() {
      if(!fp) return;
      buffer_flush();
      fclose(fp);
      fp = 0;
    }

    file() {
      memset(buffer, 0, sizeof buffer);
      buffer_offset = -1;
      buffer_dirty = false;
      fp = 0;
      file_offset = 0;
      file_size = 0;
      file_mode = mode::read;
    }

    ~file() {
      close();
    }

    file& operator=(const file&) = delete;
    file(const file&) = delete;

  private:
    enum { buffer_size = 1 << 12, buffer_mask = buffer_size - 1 };
    char buffer[buffer_size];
    int buffer_offset;
    bool buffer_dirty;
    FILE *fp;
    unsigned file_offset;
    unsigned file_size;
    mode file_mode;

    void buffer_sync() {
      if(!fp) return;  //file not open
      if(buffer_offset != (file_offset & ~buffer_mask)) {
        buffer_flush();
        buffer_offset = file_offset & ~buffer_mask;
        fseek(fp, buffer_offset, SEEK_SET);
        unsigned length = (buffer_offset + buffer_size) <= file_size ? buffer_size : (file_size & buffer_mask);
        if(length) unsigned unused = fread(buffer, 1, length, fp);
      }
    }

    void buffer_flush() {
      if(!fp) return;                      //file not open
      if(file_mode == mode::read) return;  //buffer cannot be written to
      if(buffer_offset < 0) return;        //buffer unused
      if(buffer_dirty == false) return;    //buffer unmodified since read
      fseek(fp, buffer_offset, SEEK_SET);
      unsigned length = (buffer_offset + buffer_size) <= file_size ? buffer_size : (file_size & buffer_mask);
      if(length) unsigned unused = fwrite(buffer, 1, length, fp);
      buffer_offset = -1;                  //invalidate buffer
      buffer_dirty = false;
    }
  };
}

#endif
