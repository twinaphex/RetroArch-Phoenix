#ifndef __RetroArch_SETTINGS_HPP
#define __RetroArch_SETTINGS_HPP

#include "config_file.hpp"
#include <phoenix.hpp>
#include <utility>
#include "util.hpp"
#include "ruby/ruby.hpp"

#ifdef _WIN32
#define DYNAMIC_EXTENSION "*.dll"
#elif defined(__APPLE__)
#define DYNAMIC_EXTENSION "*.dylib"
#else
#define DYNAMIC_EXTENSION "*.so"
#endif

#define EDIT_WIDTH 220

using namespace phoenix;
using namespace nall;

struct ToggleWindow : public Window
{
   ToggleWindow(const string& title) 
   { 
      setTitle(title); 
      setIcon("/usr/share/icons/retroarch-phoenix.png");
      onClose = [this]() { if (cb) cb(); }; 
   }
   void show() { setVisible(); }
   void hide() { setVisible(false); }

   function<void ()> cb;
   void setCloseCallback(const function<void ()>& _cb) { cb = _cb; }
};

class SettingLayout : public util::SharedAbstract<SettingLayout>
{
   public:
      SettingLayout(ConfigFile &_conf, const string& _key, const string& _label, bool use_label = true) : conf(_conf), key(_key) 
      {
         label.setText(_label);
         if (use_label)
            hlayout.append(label, 225, 0);
      }
      HorizontalLayout& layout() { return hlayout; }

      virtual void update() = 0;

   protected:
      HorizontalLayout hlayout;
      ConfigFile &conf;
      string key;
      Label label;
};

class StringSetting : public SettingLayout, public util::Shared<StringSetting>
{
   public:
      StringSetting(ConfigFile &_conf, const string& _key, const string& label, const string& _default) : SettingLayout(_conf, _key, label), m_default(_default)
      {
         edit.onChange = [this]() { conf.set(key, edit.text()); };
         hlayout.append(edit, EDIT_WIDTH, 0); 
      }
      
      void update()
      {
         string tmp = m_default;
         conf.get(key, tmp);
         edit.setText(tmp);
      }

   private:
      LineEdit edit;
      string m_default;
};

class PathSetting : public SettingLayout, public util::Shared<PathSetting>
{
   public:
      PathSetting(ConfigFile &_conf, const string& _key, const string& label, const string& _default, const string& _filter) : SettingLayout(_conf, _key, label), m_default(_default), filter(_filter)
      {
         button.setText("Browse ...");
         clear.setText("Clear");
         edit.onChange = [this]() { conf.set(key, edit.text()); };
         hlayout.append(edit, EDIT_WIDTH, 0); 
         hlayout.append(clear, 0, 0);
         hlayout.append(button, 0, 0);

         //edit.setEditable(false);

         button.onTick = [this]() {
            string start_path;
            const char *path = std::getenv("HOME");
            char base_path[1024];
            nall::strlcpy(base_path, edit.text(), sizeof(base_path));
            char *base_ptr = strrchr(base_path, '/');
            if (!base_ptr)
               base_ptr = strrchr(base_path, '\\');
            if (base_ptr)
               *base_ptr = '\0';

            if (strlen(base_path) > 0)
               start_path = base_path;
            else if (path)
               start_path = path;
            string tmp = OS::fileLoad(Window::None, start_path, filter);
            if (strlen(tmp) > 0)
            {
               edit.setText(tmp);
               conf.set(key, tmp);
            }
         };

         clear.onTick = [this]() {
            conf.set(key, string(""));
            edit.setText(string(""));
         };
      }

      void update()
      {
         string tmp = m_default;
         conf.get(key, tmp);
         edit.setText(tmp);
      }

      LineEdit edit;
   private:
      Button button;
      Button clear;
      string m_default;
      string filter;
};

class DirSetting : public SettingLayout, public util::Shared<DirSetting>
{
   public:
      DirSetting(ConfigFile &_conf, const string& _key, const string& label, const string& _default) : SettingLayout(_conf, _key, label), m_default(_default)
      {
         button.setText("Browse ...");
         clear.setText("Clear");
         edit.onChange = [this]() { conf.set(key, edit.text()); };
         hlayout.append(edit, EDIT_WIDTH, 0); 
         hlayout.append(clear, 0, 0);
         hlayout.append(button, 0, 0);

         //edit.setEditable(false);

         button.onTick = [this]() {
            string start_path;
            const char *path = std::getenv("HOME");
            char base_path[1024];
            nall::strlcpy(base_path, edit.text(), sizeof(base_path));
            char *base_ptr = strrchr(base_path, '/');
            if (!base_ptr)
               base_ptr = strrchr(base_path, '\\');
            if (base_ptr)
               *base_ptr = '\0';

            if (strlen(base_path) > 0)
               start_path = base_path;
            else if (path)
               start_path = path;
            string tmp = OS::folderSelect(Window::None, start_path);
            if (strlen(tmp) > 0)
            {
               edit.setText(tmp);
               conf.set(key, tmp);
            }
         };

         clear.onTick = [this]() {
            conf.set(key, string(""));
            edit.setText(string(""));
         };
      }

      void update()
      {
         string tmp = m_default;
         conf.get(key, tmp);
         edit.setText(tmp);
      }

      LineEdit edit;
   private:
      Button button;
      Button clear;
      string m_default;
      string filter;
};


struct myRadioBox : public RadioBox, public util::Shared<myRadioBox>
{
   myRadioBox(ConfigFile& _conf, const string& _config_key, const string& _key = "") : conf(_conf), config_key(_config_key), key(_key) 
   {
      onTick = [this]() { conf.set(config_key, key); };
   }
   ConfigFile& conf;
   string config_key;
   string key;
};

class ShaderSetting : public SettingLayout, public util::Shared<ShaderSetting>
{
   public:
      ShaderSetting(ConfigFile &_conf, const string& _key, const linear_vector<string>& _values, const linear_vector<string>& _keys, linear_vector<PathSetting::Ptr>& _elems)
         : SettingLayout(_conf, _key, "", false), elems(_elems)
      {
         foreach(i, elems)
            vbox.append(i->layout());

         assert(_keys.size() == _values.size());

         label.setText("Shader:");
         hbox.append(label, 120, 0);
         for (unsigned i = 0; i < _keys.size(); i++)
         {
            auto box = myRadioBox::shared(conf, key, _keys[i]);
            box->setText(_values[i]);
            boxes.append(box);
            hbox.append(*box, 120, 0);
         }

         reference_array<RadioBox&> list;
         foreach(i, boxes)
         {
            list.append(*i);
         }
         RadioBox::group(list);
         boxes[0]->setChecked();

         vbox.append(hbox);
         hlayout.append(vbox);
      }

      void update()
      {
         foreach(i, elems)
            i->update();

         string tmp;
         if (conf.get(key, tmp))
         {
            foreach(i, boxes)
            {
               if (i->key == tmp)
               {
                  i->setChecked();
                  break;
               }
            }
         }
         else
            boxes[0]->setChecked();
      }

   private:
      HorizontalLayout hbox;
      VerticalLayout vbox;
      linear_vector<myRadioBox::Ptr> boxes;
      linear_vector<PathSetting::Ptr>& elems;
      Label label;
};

class BoolSetting : public SettingLayout, public util::Shared<BoolSetting>
{
   public:
      BoolSetting(ConfigFile &_conf, const string& _key, const string& label, bool _default) : SettingLayout(_conf, _key, label), m_default(_default)
      {
         check.onTick = [this]() { conf.set(key, check.checked()); };
         hlayout.append(check, 120, 0);
      }

      void update()
      {
         bool tmp = m_default;
         conf.get(key, tmp);
         check.setChecked(tmp);
      }

      CheckBoxPlain check;
   private:
      bool m_default;
};

class IntSetting : public SettingLayout, public util::Shared<IntSetting>
{
   public:
      IntSetting(ConfigFile &_conf, const string& _key, const string& label, int _default) : SettingLayout(_conf, _key, label), m_default(_default)
      {
         edit.onChange = [this]() { conf.set(key, (int)nall::decimal(edit.text())); };
         hlayout.append(edit, EDIT_WIDTH, 0);
      }

      void update()
      {
         int tmp = m_default;
         conf.get(key, tmp);
         edit.setText((unsigned)tmp);
      }

   private:
      LineEdit edit;
      int m_default;
};

class DoubleSetting : public SettingLayout, public util::Shared<DoubleSetting>
{
   public:
      DoubleSetting(ConfigFile &_conf, const string& _key, const string& label, double _default) 
         : SettingLayout(_conf, _key, label), m_default(_default)
      {
         edit.onChange = [this]() { conf.set(key, edit.text()); };
         hlayout.append(edit, EDIT_WIDTH, 0);
      }

      void update()
      {
         double tmp = m_default;
         conf.get(key, tmp);
         edit.setText(tmp);
      }

   private:
      LineEdit edit;
      double m_default;
};

class SliderSetting : public SettingLayout, public util::Shared<SliderSetting>
{
   public:
      SliderSetting(ConfigFile &_conf, const string& _key, const string& label, double _default, double _min, double _max)
         : SettingLayout(_conf, _key, label), m_default(_default), m_min(_min), m_max(_max)
      {
         slider.setLength(100);
         slider.setPosition(50);
         slider_label.setText(val2str(pos2val(50)));
         slider.onChange = [this]() 
         { 
            double val = this->pos2val(this->slider.position());
            this->conf.set(key, val); 
            auto str = this->val2str(val);
            this->slider_label.setText(str);
         };
         hlayout.append(slider, 150, 0, 3);
         hlayout.append(slider_label, 0, 0);
      }

      void update()
      {
         double tmp = m_default;
         conf.get(key, tmp);
         slider.setPosition(val2pos(tmp));
         slider_label.setText(val2str(tmp));
      }

   private:
      HorizontalSlider slider;
      Label slider_label;
      double m_default;
      double m_min;
      double m_max;

      double pos2val(unsigned pos)
      {
         return m_min + (m_max - m_min) * pos / 100.0;
      }

      unsigned val2pos(double val)
      {
         if (val > m_max)
            return 100;
         if (val < m_min)
            return 0;

         return (unsigned)(((val - m_min) / (m_max - m_min)) * 100.0);
      }

      string val2str(double val)
      {
         char buf[64];
         // Yes, %lf seems to fail on Win32 ... <____<
         snprintf(buf, sizeof(buf), "%.2f", (float)val);
         return {buf};
      }
};

class AspectSetting : public SettingLayout, public util::Shared<AspectSetting>
{
   public:
      AspectSetting(ConfigFile &_conf, const string& _key, const string& label)
         : SettingLayout(_conf, _key, label)
      {
         edit.onChange = [this]() { conf.set(key, edit.text()); };
         hlayout.append(edit, EDIT_WIDTH / 2, 0, 5);

         aspect_auto.setText("Auto");
         aspect_4_3.setText("4:3");
         aspect_8_7.setText("8:7");
         aspect_16_9.setText("16:9");
         aspect_auto.onTick = [this]() { this->clear_aspect(); };
         aspect_4_3.onTick = [this]() { this->set_aspect(1.3333); };
         aspect_8_7.onTick = [this]() { this->set_aspect(1.1429); };
         aspect_16_9.onTick = [this]() { this->set_aspect(1.7778); };

         hlayout.append(aspect_auto, 0, 0);
         hlayout.append(aspect_4_3, 0, 0);
         hlayout.append(aspect_8_7, 0, 0);
         hlayout.append(aspect_16_9, 0, 0);
      }

      void update()
      {
         double tmp;
         if (conf.get(key, tmp))
            edit.setText(tmp);
	 else
	    edit.setText("");
      }

   private:
      LineEdit edit;
      Button aspect_4_3, aspect_8_7, aspect_auto, aspect_16_9;

      void set_aspect(double aspect)
      {
         conf.set(key, aspect);
         edit.setText(aspect);
      }

      void clear_aspect()
      {
         conf.set(key, string());
	 edit.setText("");
      }
};

namespace Internal
{
   struct combo_selection
   {
      string internal_name;
      string external_name;
   };

   struct input_selection
   {
      string config_base;
      string base;
      string display;
      string def;
   };
}

class ComboSetting : public SettingLayout, public util::Shared<ComboSetting>
{
   public:
      ComboSetting(ConfigFile &_conf, const string& _key, const string& label, const linear_vector<Internal::combo_selection>& _list, unsigned _default) 
         : SettingLayout(_conf, _key, label), m_default(_default), list(_list)
      {
         foreach(i, list) box.append(i.external_name);
         box.setSelection(m_default);

         box.onChange = [this]() {
            conf.set(key, list[box.selection()].internal_name);
         };

         hlayout.append(box, EDIT_WIDTH, 0);
      }

      void update()
      {
         string tmp;
         conf.get(key, tmp);
         unsigned index = m_default;
         for (unsigned i = 0; i < list.size(); i++)
         {
            if (list[i].internal_name == tmp)
            {
               index = i;
               break;
            }
         }

         box.setSelection(index);
         conf.set(key, list[box.selection()].internal_name);
      }

   private:
      ComboBox box;
      unsigned m_default;   
      const linear_vector<Internal::combo_selection>& list;
};

namespace Internal
{
   static const char keymap[][64] = {
      "escape", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12",
      "print_screen", "scroll_lock", "pause", "tilde",
      "num1", "num2", "num3", "num4", "num5", "num6", "num7", "num8", "num9", "num0",
      "dash", "equal", "backspace",
      "insert", "del", "home", "end", "pageup", "pagedown",
      "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
      "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
      "leftbracket", "rightbracket", "backslash", "semicolon", "apostrophe", "comma", "period", "slash",
      "keypad1", "keypad2", "keypad3", "keypad4", "keypad5", "keypad6", "keypad7", "keypad8", "keypad9", "keypad0",
      "point", "kp_enter", "add", "subtract", "multiply", "divide",
      "numlock", "capslock",
      "up", "down", "left", "right",
      "tab", "enter", "space", "menu",
      "shift", "ctrl", "alt", "super",
   };
}

class InputSetting : public SettingLayout, public util::Shared<InputSetting>
{
   public:
      InputSetting(ConfigFile &_conf, const linear_vector<linear_vector<Internal::input_selection>>& _list, const function<void (const string&)>& _msg_cb, const function<void ()>& _focus_cb)
         : SettingLayout(_conf, "", "Binds:", false), list(_list), msg_cb(_msg_cb), focus_cb(_focus_cb)
      {
         clear.setText("Clear");
         def.setText("Default");
         def_all.setText("Default all");
         erase.setText("Clear all");
         player.append("Player 1");
         player.append("Player 2");
         player.append("Player 3");
         player.append("Player 4");
         player.append("Player 5");
         player.append("Player 6");
         player.append("Player 7");
         player.append("Player 8");
         player.append("Misc");
         hbox.append(player, 120, 0, 10);
         hbox.append(def, 0, 0);
         hbox.append(def_all, 0, 0);
         hbox.append(clear, 0, 0);
         hbox.append(erase, 0, 0);

         player.onChange = [this]() { this->update_list(); };

         clear.onTick = [this]() { this->set_single("None", "nul"); };
         def.onTick = [this]() { this->set_single("Default", ""); };

         def_all.onTick = [this]()
         {
            auto response = MessageWindow::information(Window::None,
                  "Would you like to default binds on other pages as well?",
                  MessageWindow::Buttons::YesNo);

            if (response == MessageWindow::Response::Yes)
               this->set_all_list("Default", "");
            else
               this->set_all_current_list("Default", "");
         };

         erase.onTick = [this]()
         {
            auto response = MessageWindow::information(Window::None,
                  "Would you like to clear binds on other pages as well?",
                  MessageWindow::Buttons::YesNo);

            if (response == MessageWindow::Response::Yes)
               this->set_all_list("None", "nul");
            else
               this->set_all_current_list("None", "nul");
         };

         list_view.setHeaderText(string("RetroArch Bind"), string("Bind"));
         list_view.setHeaderVisible();
         list_view.onActivate = [this]() { this->update_bind(); };

         vbox.append(hbox);

         index_label.setText("Joypad #:");
         hbox_index.append(index_label, 60, 0);
         index_show.setEditable(false);
         hbox_index.append(index_show, 60, 0, 10);
         analog_enable.setText("Detect analog");
         analog_enable.setChecked();
         hbox_index.append(analog_enable, 150, 0);
         vbox.append(hbox_index);

         vbox.append(list_view, ~0, 300);
         vbox.setMargin(2);

         warn.setText("Note: Even if a key is detected properly here, it might not work in-game.\nIf it doesn't, please file a bug-report and try a different one for now.");
         vbox.append(warn, 0, 2.5 * 0);

         cancel_poll_btn.setText("Cancel input");
         cancel_poll_btn.setEnabled(false);
         cancel_poll_btn.onTick = {&InputSetting::cancel_poll, this};
         vbox.append(cancel_poll_btn, 0, 0);

         hlayout.append(vbox);

         timer.onTimeout = {&InputSetting::update_bind_event, this};
         timer.setInterval(10);
      }

      void cancel_poll()
      {
         timer.setEnabled(false);
         list_view.setEnabled(true);
         cancel_poll_btn.setEnabled(false);
         msg_cb("");
      }

      void update()
      {
         update_from_config();
         update_list();
      }
      
   private:
      function<void (const string&)> msg_cb;
      function<void ()> focus_cb;
      HorizontalLayout hbox;
      VerticalLayout vbox;
      ListView list_view;
      ComboBox player;
      Button clear;
      Button def_all;
      Button def;
      Button erase;
      Label warn;
      Button cancel_poll_btn;

      Timer timer;
      int16_t old_data[Scancode::Limit];
      Internal::input_selection *poll_elem;

      Label index_label;
      LineEdit index_show;
      CheckBox analog_enable;
      HorizontalLayout hbox_index;

      linear_vector<linear_vector<Internal::input_selection>> list;

      void set_single(const string& display, const string& conf_string)
      {
         auto j = list_view.selection();
         unsigned i = player.selection();
         if (list_view.selected())
         {
            if (display == "Default")
               list[i][j].display = {"Default", " <", list[i][j].def, ">"};
            else
               list[i][j].display = display;

            conf.set(list[i][j].config_base, conf_string);
            conf.set({list[i][j].config_base, "_btn"}, conf_string);
            conf.set({list[i][j].config_base, "_axis"}, conf_string);
            this->update_list(i, j);
         }
      }

      void set_all_list(const string& display, const string& conf_string)
      {
         foreach(i, list)
         {
            foreach(j, i)
            {
               if (display == "Default")
                  j.display = {"Default", " <", j.def, ">"};
               else
                  j.display = display;

               conf.set(j.config_base, conf_string);
               conf.set({j.config_base, "_btn"}, conf_string);
               conf.set({j.config_base, "_axis"}, conf_string);
            }
         }
         
         conf.set("input_player1_joypad_index", 0);
         conf.set("input_player2_joypad_index", 1);
         conf.set("input_player3_joypad_index", 2);
         conf.set("input_player4_joypad_index", 3);
         conf.set("input_player5_joypad_index", 4);
         conf.set("input_player6_joypad_index", 4);
         conf.set("input_player7_joypad_index", 4);
         conf.set("input_player8_joypad_index", 4);

         this->update_list();
      }

      void set_all_current_list(const string& display, const string& conf_string)
      {
         foreach (i, list[player.selection()])
         {
            if (display == "Default")
                  i.display = {"Default", " <", i.def, ">"};
               else
                  i.display = display;

               conf.set(i.config_base, conf_string);
               conf.set({i.config_base, "_btn"}, conf_string);
               conf.set({i.config_base, "_axis"}, conf_string);
         }

         this->update_list();
      }

      enum class Type
      {
         Keyboard,
         JoyButton,
         JoyAxis,
         JoyHat,
         None
      };

      void update_input_player()
      {
         if (player.selection() == max_players) // Misc, only player 1
         {
            index_show.setText("0");
         }
         else
         {
            string index_conf_str {"input_player", (unsigned)(player.selection() + 1), "_joypad_index"};
            string tmp;
            if (conf.get(index_conf_str, tmp))
               index_show.setText(tmp);
            else
               index_show.setText((unsigned)player.selection());
         }
      }

      void update_list(unsigned p, unsigned i)
      {
         list_view.modify(i, list[p][i].base, list[p][i].display);
         list_view.autoSizeColumns();
         update_input_player();
      }
      
      void update_list()
      {
         list_view.reset();

         foreach(i, list[player.selection()])
            list_view.append(i.base, i.display);
         list_view.autoSizeColumns();

         update_input_player();
      }

      void update_bind_event()
      {
         string option, ext;
         auto type = poll(option);
         if (type == Type::None)
            return;

         timer.setEnabled(false);
         list_view.setEnabled(true);
         cancel_poll_btn.setEnabled(false);

         switch (type)
         {
            case Type::JoyAxis:
               conf.set({poll_elem->config_base, "_axis"}, option);
               conf.set({poll_elem->config_base, "_btn"}, string("nul"));
               conf.set({poll_elem->config_base, ""}, string("nul"));
               ext = " (axis)";
               break;

            case Type::JoyButton:
               conf.set({poll_elem->config_base, "_axis"}, string("nul"));
               conf.set({poll_elem->config_base, "_btn"}, option);
               conf.set({poll_elem->config_base, ""}, string("nul"));
               ext = " (button)";
               break;

            case Type::JoyHat:
               conf.set({poll_elem->config_base, "_axis"}, string("nul"));
               conf.set({poll_elem->config_base, "_btn"}, option);
               conf.set({poll_elem->config_base, ""}, string("nul"));
               ext = " (hat)";
               break;

            case Type::Keyboard:
               conf.set({poll_elem->config_base, "_axis"}, string("nul"));
               conf.set({poll_elem->config_base, "_btn"}, string("nul"));
               conf.set({poll_elem->config_base, ""}, option);
               break;

            default:
               return;
         }

         poll_elem->display = option;
         if (strlen(ext) > 0)
            poll_elem->display.append(ext);

         msg_cb("");
         update_list(player.selection(), list_view.selection());
      }

      void update_bind()
      {
         const string& opt = list[player.selection()][list_view.selection()].config_base;
         msg_cb({"Activate for bind: ", opt});

         focus_cb();

         poll_elem = &list[player.selection()][list_view.selection()];
         list_view.setSelected(false);
         list_view.setEnabled(false);
         cancel_poll_btn.setEnabled(true);

         timer.setEnabled(true);

         memset(old_data, 0, sizeof(old_data));
         ruby::input.poll(old_data);
      }

      Type poll(string& opt)
      {
         auto type = Type::None;
         int16_t new_data[Scancode::Limit] = {0};
         opt = "";

         ruby::input.poll(new_data);
         for (int i = 0; i < Scancode::Limit; i++)
         {
            if (old_data[i] != new_data[i])
            {
               if (Joypad::isAnyAxis(i))
               {
                  if (!analog_enable.checked())
                     continue;

                  if (std::abs((int)old_data[i] - (int)new_data[i]) < 20000)
                     continue;

                  type = Type::JoyAxis;
               }
               else if (Joypad::isAnyButton(i))
                  type = Type::JoyButton;
               else if (Joypad::isAnyHat(i))
                  type = Type::JoyHat;
               else
                  type = Type::Keyboard;

               if ((Joypad::isAnyAxis(i) || Joypad::isAnyButton(i) || Joypad::isAnyHat(i)) 
                     && player.selection() < max_players)
               {
                  conf.set(string("input_player", 
                        (unsigned)(player.selection() + 1), "_joypad_index"), 
                        Joypad::numberDecode(i));
               }

               int code = 0;
               if ((code = Joypad::buttonDecode(i)) >= 0)
                  opt = string((unsigned)code);
               else if ((code = Joypad::axisDecode(i)) >= 0)
               {
                  if (old_data[i] < new_data[i])
                     opt = string("+", (unsigned)code);
                  else
                     opt = string("-", (unsigned)code);
               }
               else if ((code = Joypad::hatDecode(i)) >= 0)
               {
                  opt = {"h", (unsigned)code};
                  int16_t j = new_data[i];
                  if (j & Joypad::HatUp)
                     opt.append("up");
                  else if (j & Joypad::HatDown)
                     opt.append("down");
                  else if (j & Joypad::HatRight)
                     opt.append("right");
                  else if (j & Joypad::HatLeft)
                     opt.append("left");
               }
               else
                  opt = encode(i);

               break;
            }
         }

         return type;
      }

      void update_from_config()
      {
         foreach(i, list)
         {
            foreach(j, i)
            {
               string tmp[3];
               bool is_nul[3] = {false};
               bool is_valid[3] = {false};
               linear_vector<string> appends = {"", "_btn", "_axis"};

               foreach(i, appends, count)
               {
                  if (conf.get({j.config_base, i}, tmp[count]))
                  {
                     if (tmp[count] == "nul")
                        is_nul[count] = true;
                     else
                        is_valid[count] = true;
                  }
               }

               if (is_nul[0] && is_nul[1] && is_nul[2])
                  j.display = "None";
               else if (is_valid[0])
                  j.display = tmp[0];
               else if (is_valid[1])
                  j.display = {tmp[1], " (button)"};
               else if (is_valid[2])
                  j.display = {tmp[2], " (axis)"};
               else
                  j.display = {"Default", " <", j.def, ">"};
            }
         }
      }

      static string encode(unsigned i)
      {
         auto j = Keyboard::keyDecode(i);
         if (j >= 0)
            return Internal::keymap[j];
         else
            return "(unimplemented, pick something else)";
      }

      static const unsigned max_players = 8;
};

class General : public ToggleWindow
{
   public:
      General(ConfigFile &_pconf, ConfigFile &_conf) : ToggleWindow("RetroArch || General settings")
      {
         widgets.append(DirSetting::shared(_conf, "rgui_browser_directory",
            "RGUI ROM directory:", string("")));
         widgets.append(BoolSetting::shared(_conf, "rewind_enable", "Enable rewind:", false));
         widgets.append(IntSetting::shared(_conf, "rewind_buffer_size", "Rewind buffer size (MB):", 20));
         widgets.append(IntSetting::shared(_conf, "rewind_granularity", "Rewind frames granularity:", 1));
         widgets.append(DoubleSetting::shared(_conf, "slowmotion_ratio", "Slow motion ratio:", 3.0));
         widgets.append(BoolSetting::shared(_conf, "pause_nonactive", "Pause when window loses focus:", false));
         widgets.append(IntSetting::shared(_conf, "autosave_interval", "Autosave interval (seconds):", 0));
         widgets.append(BoolSetting::shared(_conf, "block_sram_overwrite", "Block SRAM overwrite:", false));
         widgets.append(BoolSetting::shared(_conf, "savestate_auto_index", "Auto-increment save state slots:", false));
         widgets.append(BoolSetting::shared(_conf, "savestate_auto_save", "Autosave state on exit:", false));
         widgets.append(BoolSetting::shared(_conf, "video_post_filter_record", "Records filtered output:", false));
         widgets.append(BoolSetting::shared(_conf, "video_gpu_record", "Records post-shaded output:", false));
         widgets.append(BoolSetting::shared(_conf, "network_cmd_enable", "Enable UDP command interface:", false));

         widgets.append(DirSetting::shared(_conf, "system_directory", "System directory:", string("")));
         widgets.append(PathSetting::shared(_conf, "cheat_database_path", "XML cheat database:", string(""), "XML files (*.xml)"));
         widgets.append(PathSetting::shared(_conf, "cheat_settings_path", "XML cheat settings:", string(""), "Config files (*.cfg)"));
         widgets.append(DirSetting::shared(_conf, "savefile_directory", "Savefile/movie directory:", string("")));
         widgets.append(DirSetting::shared(_conf, "savestate_directory", "Savestate directory:", string("")));
         widgets.append(DirSetting::shared(_conf, "screenshot_directory", "Screenshot directory:", string("")));
         widgets.append(PathSetting::shared(_conf, "core_options_path", "Core options config:", string(""), "Config files (*.cfg)"));
         async_fork = BoolSetting::shared(_pconf, "async_fork", "Keep UI visible:", false);

         widgets.append(async_fork);

         foreach(i, widgets) { vbox.append(i->layout(), 3); }

         vbox.setMargin(5);
         auto minimum = vbox.minimumGeometry();
         setGeometry({128, 128, minimum.width, minimum.height});

         append(vbox);
      }

      void update() { foreach(i, widgets) i->update(); }

      bool getAsyncFork()
      {
         return async_fork->check.checked();
      }

   private:
      linear_vector<SettingLayout::APtr> widgets;
      VerticalLayout vbox;
      BoolSetting::Ptr async_fork;
};

namespace Internal
{
   static const linear_vector<combo_selection> video_drivers = {
#if defined(_WIN32)
      { "gl", "OpenGL" },
      { "d3d9", "Direct3D9" },
      { "sdl", "SDL" },
#elif defined(__APPLE__)
      { "gl", "OpenGL" },
      { "sdl", "SDL" },
#else
      { "gl", "OpenGL" },
      { "xvideo", "XVideo" },
      { "sdl", "SDL" },
#endif
   };
}

class ShaderVideo : public ToggleWindow
{
   public:
      ShaderVideo(ConfigFile &_conf) : ToggleWindow("RetroArch || Shader settings")
      {
         widgets.append(PathSetting::shared(_conf, "video_shader", "Video Shader:", "", "Cg shader, Cg meta-shader, XML/GLSL, GLSL, GLSLP (*.cg,*.cgp,*.shader,*.glsl,*.glslp)"));
         widgets.append(BoolSetting::shared(_conf, "video_shader_enable", "Enable shader:", false));
         widgets.append(DirSetting::shared(_conf, "video_shader_dir", "Shader directory:", ""));
         widgets.append(PathSetting::shared(_conf, "video_filter", "bSNES video filter:", "", "bSNES filter (*.filter)"));

         foreach(i, widgets) { vbox.append(i->layout(), 3); }
         vbox.setMargin(5);
         auto minimum = vbox.minimumGeometry();
         setGeometry({128, 128, minimum.width, minimum.height});
         append(vbox);
      }

      void update() { foreach(i, widgets) i->update(); }

   private:
      linear_vector<SettingLayout::APtr> widgets;
      VerticalLayout vbox;
};

class FontVideo : public ToggleWindow
{
   public:
      FontVideo(ConfigFile &_conf) : ToggleWindow("RetroArch || Font settings")
      {
         widgets.append(PathSetting::shared(_conf, "video_font_path", "On-screen message font:", "", "TTF font (*.ttf)"));
         widgets.append(IntSetting::shared(_conf, "video_font_size", "On-screen font size:", 48));
         widgets.append(BoolSetting::shared(_conf, "video_font_scale", "Scale font to window", true));
         widgets.append(BoolSetting::shared(_conf, "video_font_enable", "Enable on-screen messages:", true));
         widgets.append(DoubleSetting::shared(_conf, "video_message_pos_x", "On-screen message pos X:", 0.05));
         widgets.append(DoubleSetting::shared(_conf, "video_message_pos_y", "On-screen message pos Y:", 0.05));
         widgets.append(StringSetting::shared(_conf, "video_message_color", "Message color (RGB hex):", "ffff00"));

         foreach(i, widgets) { vbox.append(i->layout(), 3); }
         vbox.setMargin(5);
         auto minimum = vbox.minimumGeometry();
         setGeometry({128, 128, minimum.width, minimum.height});
         append(vbox);
      }

      void update() { foreach(i, widgets) i->update(); }

   private:
      linear_vector<SettingLayout::APtr> widgets;
      VerticalLayout vbox;
};

class Video : public ToggleWindow
{
   public:
      Video(ConfigFile &_conf) : ToggleWindow("RetroArch || Video settings"), shader_setting(_conf), font_setting(_conf)
      {
         widgets.append(ComboSetting::shared(_conf, "video_driver", "Video driver:", Internal::video_drivers, 0));
         widgets.append(DoubleSetting::shared(_conf, "video_xscale", "Windowed X scale:", 3.0));
         widgets.append(DoubleSetting::shared(_conf, "video_yscale", "Windowed Y scale:", 3.0));
         widgets.append(IntSetting::shared(_conf, "video_fullscreen_x", "Fullscreen X resolution:", 0));
         widgets.append(IntSetting::shared(_conf, "video_fullscreen_y", "Fullscreen Y resolution:", 0));
         widgets.append(DoubleSetting::shared(_conf, "video_refresh_rate", "Monitor refresh rate (Hz):", 59.95));
         widgets.append(BoolSetting::shared(_conf, "video_vsync", "VSync:", true));
         widgets.append(BoolSetting::shared(_conf, "video_fullscreen", "Start in fullscreen:", false));
         widgets.append(BoolSetting::shared(_conf, "video_windowed_fullscreen", "Prefer windowed fullscreen:", true));
         widgets.append(IntSetting::shared(_conf, "video_monitor_index", "Preferred monitor index:", 0));
         widgets.append(BoolSetting::shared(_conf, "video_disable_composition", "Disable composition (Win Vista/7):", false));
         widgets.append(BoolSetting::shared(_conf, "video_smooth", "Bilinear filtering:", true));
         widgets.append(BoolSetting::shared(_conf, "video_force_aspect", "Lock aspect ratio:", true));
         widgets.append(BoolSetting::shared(_conf, "video_scale_integer", "Integer scale:", false));
         widgets.append(BoolSetting::shared(_conf, "video_crop_overscan", "Crop overscan:", true));
         widgets.append(AspectSetting::shared(_conf, "video_aspect_ratio", "Aspect ratio:"));
         widgets.append(BoolSetting::shared(_conf, "video_aspect_ratio_auto", "Prefer game aspect over 1:1 PAR:", false));

         foreach(i, widgets) { vbox.append(i->layout(), 3); }

         shader.button.setText("Open");
         shader.button.onTick = [this]() { this->shader_setting.show(); };
         shader.label.setText("Shader/filter settings:");
         shader.layout.append(shader.label, 225, 0);
         shader.layout.append(shader.button, 0, 0);
         vbox.append(shader.layout);

         font.button.setText("Open");
         font.button.onTick = [this]() { this->font_setting.show(); };
         font.label.setText("Font settings:");
         font.layout.append(font.label, 225, 0);
         font.layout.append(font.button, 0, 0);
         vbox.append(font.layout);

         vbox.setMargin(5);
         auto minimum = vbox.minimumGeometry();
         setGeometry({128, 128, minimum.width, minimum.height});
         append(vbox);
      }

      void update() 
      { 
         foreach(i, widgets) i->update(); 
         shader_setting.update();
         font_setting.update();
      }

      void hide()
      {
         shader_setting.hide();
         font_setting.hide();
         ToggleWindow::hide();
      }

   private:
      linear_vector<SettingLayout::APtr> widgets;
      VerticalLayout vbox;
      ShaderVideo shader_setting;
      FontVideo font_setting;

      struct
      {
         HorizontalLayout layout;
         Label label;
         Button button;
      } shader, font;
};


namespace Internal
{
   static const linear_vector<combo_selection> audio_drivers = {
#ifdef _WIN32
      {"dsound", "DirectSound"},
      {"xaudio", "XAudio2"},
      {"sdl", "SDL"},
      {"rsound", "RSound"},
#elif defined(__APPLE__)
      {"coreaudio", "CoreAudio"},
      {"openal", "OpenAL"},
      {"sdl", "SDL"},
      {"rsound", "RSound"},
#else
      {"alsa", "ALSA"},
      {"pulse", "PulseAudio"},
      {"oss", "Open Sound System"},
      {"jack", "JACK Audio"},
      {"rsound", "RSound"},
      {"roar", "RoarAudio"},
      {"openal", "OpenAL"},
      {"sdl", "SDL"},
#endif
   };

#define ANALOG_BINDS(n_player) \
      { "input_player" #n_player "_l_x_plus", "Left analog X+ (right)", "", "" }, \
      { "input_player" #n_player "_l_y_plus", "Left analog Y+ (down)", "", "" }, \
      { "input_player" #n_player "_l_x_minus", "Left analog X- (left)", "", "" }, \
      { "input_player" #n_player "_l_y_minus", "Left analog Y- (up)", "", "" }, \
      { "input_player" #n_player "_r_x_plus", "Right analog X+ (right)", "", "" }, \
      { "input_player" #n_player "_r_y_plus", "Right analog Y+ (down)", "", "" }, \
      { "input_player" #n_player "_r_x_minus", "Right analog X- (left)", "", "" }, \
      { "input_player" #n_player "_r_y_minus", "Right analog Y- (up)", "", "" }

#define DEFINE_BINDS(n_player) \
      { "input_player" #n_player "_a", "A (right)", "", "x" }, \
      { "input_player" #n_player "_b", "B (down)", "", "z" }, \
      { "input_player" #n_player "_y", "Y (left)", "", "a" }, \
      { "input_player" #n_player "_x", "X (up)", "", "s" }, \
      { "input_player" #n_player "_start", "Start", "", "enter" }, \
      { "input_player" #n_player "_select", "Select", "", "rshift" }, \
      { "input_player" #n_player "_l", "L", "", "q" }, \
      { "input_player" #n_player "_r", "R", "", "w" }, \
      { "input_player" #n_player "_left", "D-pad Left", "", "left" }, \
      { "input_player" #n_player "_right", "D-pad Right", "", "right" }, \
      { "input_player" #n_player "_up", "D-pad Up", "", "up" }, \
      { "input_player" #n_player "_down", "D-pad Down", "", "down" }, \
      { "input_player" #n_player "_l2", "L2", "", "" }, \
      { "input_player" #n_player "_r2", "R2", "", "" }, \
      { "input_player" #n_player "_l3", "L3", "", "" }, \
      { "input_player" #n_player "_r3", "R3", "", "" }, \
      { "input_player" #n_player "_turbo", "Turbo", "", "" }, \
      ANALOG_BINDS(n_player)

#define DEFINE_BINDS_OTHER(n_player) \
      { "input_player" #n_player "_a", "A (right)", "", "" }, \
      { "input_player" #n_player "_b", "B (down)", "", "" }, \
      { "input_player" #n_player "_y", "Y (left)", "", "" }, \
      { "input_player" #n_player "_x", "X (up)", "", "" }, \
      { "input_player" #n_player "_start", "Start", "", "" }, \
      { "input_player" #n_player "_select", "Select", "", "" }, \
      { "input_player" #n_player "_l", "L", "", "" }, \
      { "input_player" #n_player "_r", "R", "", "" }, \
      { "input_player" #n_player "_left", "D-pad Left", "", "" }, \
      { "input_player" #n_player "_right", "D-pad Right", "", "" }, \
      { "input_player" #n_player "_up", "D-pad Up", "", "" }, \
      { "input_player" #n_player "_down", "D-pad Down", "", "" }, \
      { "input_player" #n_player "_l2", "L2", "", "" }, \
      { "input_player" #n_player "_r2", "R2", "", "" }, \
      { "input_player" #n_player "_l3", "L3", "", "" }, \
      { "input_player" #n_player "_r3", "R3", "", "" }, \
      { "input_player" #n_player "_turbo", "Turbo", "", "" }, \
      ANALOG_BINDS(n_player)
 

   static const linear_vector<input_selection> player1 = {
      DEFINE_BINDS(1)
   };

   static const linear_vector<input_selection> player2 = {
      DEFINE_BINDS_OTHER(2)
   };

   static const linear_vector<input_selection> player3 = {
      DEFINE_BINDS_OTHER(3)
   };

   static const linear_vector<input_selection> player4 = {
      DEFINE_BINDS_OTHER(4)
   };

   static const linear_vector<input_selection> player5 = {
      DEFINE_BINDS_OTHER(5)
   };

   static const linear_vector<input_selection> player6 = {
      DEFINE_BINDS_OTHER(6)
   };

   static const linear_vector<input_selection> player7 = {
      DEFINE_BINDS_OTHER(7)
   };

   static const linear_vector<input_selection> player8 = {
      DEFINE_BINDS_OTHER(8)
   };

   static const linear_vector<input_selection> misc = {
      { "input_enable_hotkey", "Hold to use hotkeys", "", "" },
      { "input_save_state", "Save state", "", "f2" },
      { "input_load_state", "Load state", "", "f4" },
      { "input_state_slot_increase", "Increase state slot", "", "f7" },
      { "input_state_slot_decrease", "Decrease state slot", "", "f6" },
      { "input_toggle_fast_forward", "Toggle fast forward", "", "space" },
      { "input_hold_fast_forward", "Hold fast forward", "", "l" },
      { "input_exit_emulator", "Exit emulator", "", "escape" },
      { "input_toggle_fullscreen", "Toggle fullscreen", "", "f" },
      { "input_pause_toggle", "Pause toggle", "", "p" },
      { "input_frame_advance", "Frame advance", "", "k" },
      { "input_movie_record_toggle", "Movie record toggle", "", "o" },
      { "input_rewind", "Rewind", "", "r" },
      { "input_reset", "Reset", "", "h" },
      { "input_shader_next", "Next shader", "", "m" },
      { "input_shader_prev", "Previous shader", "", "n" },
      { "input_cheat_index_minus", "Previous cheat index", "", "t" },
      { "input_cheat_index_plus", "Next cheat index", "", "y" },
      { "input_cheat_toggle", "Toggle cheat ON/OFF", "", "u" },
      { "input_screenshot", "Take screenshot", "", "f8" },
      { "input_dsp_config", "Open DSP plugin config", "", "c" },
      { "input_audio_mute", "Mute/unmute audio", "", "f9" },
      { "input_netplay_flip_players", "Netplay flip players", "", "i" },
      { "input_slowmotion", "Slow motion", "", "e" },
      { "input_volume_up", "Increase volume", "", "kp_plus" },
      { "input_volume_down", "Decrease volume", "", "kp_minus" },
      { "input_overlay_next", "Overlay next", "", "" },
      { "input_disk_eject_toggle", "Disk eject toggle", "", "" },
      { "input_disk_next", "Disk next", "", "" },
      { "input_menu_toggle", "RGUI menu toggle", "", "f1" },
   };

   static const linear_vector<linear_vector<input_selection>> binds = { 
      player1, player2, player3, player4, player5, player6, player7, player8, misc 
   };
}

class Audio : public ToggleWindow
{
   public:
      Audio(ConfigFile &_conf) : ToggleWindow("RetroArch || Audio settings")
      {
         widgets.append(BoolSetting::shared(_conf, "audio_enable", "Enable audio:", true));
         widgets.append(ComboSetting::shared(_conf, "audio_driver", "Audio driver:", Internal::audio_drivers, 0));
         widgets.append(PathSetting::shared(_conf, "audio_dsp_plugin", "Audio DSP plugin:", string(""), "Dynamic library (" DYNAMIC_EXTENSION ")"));
         widgets.append(IntSetting::shared(_conf, "audio_out_rate", "Audio sample rate:", 48000));
         widgets.append(StringSetting::shared(_conf, "audio_device", "Audio device:", ""));
         widgets.append(BoolSetting::shared(_conf, "audio_sync", "Audio sync:", true));
         widgets.append(IntSetting::shared(_conf, "audio_latency", "Audio latency (ms):", 64));
         widgets.append(BoolSetting::shared(_conf, "audio_rate_control", "Dynamic rate control:", true));
         widgets.append(DoubleSetting::shared(_conf, "audio_rate_control_delta", "Rate control pitch delta:", 0.005));

         foreach(i, widgets) { vbox.append(i->layout(), 3); }
         vbox.setMargin(5);
         auto minimum = vbox.minimumGeometry();
         setGeometry({128, 128, minimum.width, minimum.height});
         append(vbox);
      }

      void update() { foreach(i, widgets) i->update(); }

   private:
      linear_vector<SettingLayout::APtr> widgets;
      VerticalLayout vbox;

};


class Input : public ToggleWindow
{
   public:
      Input(ConfigFile &_conf) : ToggleWindow("RetroArch || Input settings")
      {
         widgets.append(SliderSetting::shared(_conf, "input_axis_threshold", "Input axis threshold:", 0.5, 0.0, 1.0));
         widgets.append(PathSetting::shared(_conf, "input_overlay", "Overlay config:", string(""), "Config file (*.cfg)"));
         widgets.append(BoolSetting::shared(_conf, "netplay_client_swap_input", "Use Player 1 binds as netplay client:", true));
         widgets.append(input_setting = InputSetting::shared(_conf, Internal::binds, 
                  [this](const string& msg) { this->setStatusText(msg); }, 
                  [this]() { this->setFocused(); }));

         foreach(i, widgets) { vbox.append(i->layout(), 3); }
         vbox.setMargin(5);
         auto minimum = vbox.minimumGeometry();
         setGeometry({128, 128, minimum.width, minimum.height});
         append(vbox);

         init_input();

         setStatusVisible();
      }

      void update() { foreach(i, widgets) i->update(); }
      void hide() { input_setting->cancel_poll(); ToggleWindow::hide(); }

   private:
      linear_vector<SettingLayout::APtr> widgets;
      VerticalLayout vbox;
      InputSetting::Ptr input_setting;

      void init_input()
      {
#ifdef _WIN32
         ruby::input.driver("DirectInput");
#elif defined(__APPLE__)
         ruby::input.driver("Carbon");
#elif defined(__linux)
         ruby::input.driver("LinuxRaw");
#else
         ruby::input.driver("SDL");
#endif
         ruby::input.init();
      }
};

class ExtROM : public ToggleWindow
{
   public:
      ExtROM(ConfigFile &_conf) : ToggleWindow("RetroArch || Special ROM")
      {
         sgb_bios = PathSetting::shared(_conf, "sgb_bios_path", "Super Gameboy BIOS:", string(""), "Super Famicom, Super Magicom (*.sfc,*.smc)");
         sgb_rom = PathSetting::shared(_conf, "gameboy_path", "Gameboy ROM:", string(""), "Gameboy (*.gb)");
         sufami_bios = PathSetting::shared(_conf, "sufami_bios_path", "Sufami Turbo BIOS:", string(""), "Super Famicom, Super Magicom (*.sfc,*.smc)");
         sufami_slot_a = PathSetting::shared(_conf, "sufami_slot_a_path", "Sufami ROM (Slot A):", string(""), "Sufami Turbo (*.st)");
         sufami_slot_b = PathSetting::shared(_conf, "sufami_slot_b_path", "Sufami ROM (Slot B):", string(""), "Sufami Turbo (*.st)");
         bsx_bios = PathSetting::shared(_conf, "bsx_bios_path", "BSX BIOS:", string(""), "Super Famicom, Super Magicom (*.sfc,*.smc)");
         bsx_rom = PathSetting::shared(_conf, "bsx_rom_path", "BSX ROM:", string(""), "BSX (*.bs)");

         widgets.append(sgb_bios);
         widgets.append(sgb_rom);
         widgets.append(sufami_bios);
         widgets.append(sufami_slot_a);
         widgets.append(sufami_slot_b);
         widgets.append(bsx_bios);
         widgets.append(bsx_rom);

         foreach(i, widgets) { vbox.append(i->layout(), 3); }
         vbox.setMargin(5);
         auto minimum = vbox.minimumGeometry();
         setGeometry({128, 128, minimum.width, minimum.height});

         append(vbox);
      }

      void update() { foreach(i, widgets) i->update(); }

      bool get_sgb_bios(string& str) { return get_str(sgb_bios, str); }
      bool get_sgb_rom(string& str) { return get_str(sgb_rom, str); }
      bool get_sufami_bios(string& str) { return get_str(sufami_bios, str); }
      bool get_sufami_slot_a(string& str) { return get_str(sufami_slot_a, str); }
      bool get_sufami_slot_b(string& str) { return get_str(sufami_slot_b, str); }
      bool get_bsx_bios(string& str) { return get_str(bsx_bios, str); }
      bool get_bsx_rom(string& str) { return get_str(bsx_rom, str); }

   private:
      linear_vector<SettingLayout::APtr> widgets;
      VerticalLayout vbox;

      PathSetting::Ptr sgb_bios, sgb_rom, sufami_bios, sufami_slot_a, sufami_slot_b, bsx_bios, bsx_rom;

      bool get_str(PathSetting::Ptr ptr, string& str)
      {
         string _str = ptr->edit.text();
         if (strlen(_str) > 0)
         {
            str = _str;
            return true;
         }
         else
            return false;
      }
};


#endif
