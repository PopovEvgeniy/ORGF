/*
Copyright (C) 2016-2019 Popov Evgeniy Alekseyevich

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"winmm.lib")

//Uncomment follow lines if you will compile the code under Visual C++ 2017
/*
#pragma warning(disable : 4995)
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
*/

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <windows.h>
#include <unknwn.h>
#include <dshow.h>
#include <mmsystem.h>

#define ORGF_GETSCANCODE(argument) ((argument >> 16)&0x7f)

#define ORGF_KEYBOARD 256
#define ORGF_MOUSE 3

#define ORGFKEY_RELEASE 0
#define ORGFKEY_PRESS 1

#define ORGF_MOUSE_LEFT 0
#define ORGF_MOUSE_RIGHT 1
#define ORGF_MOUSE_MIDDLE 2

#define JOYSTICK_UPLEFT 31500
#define JOYSTICK_UPRIGHT 4500
#define JOYSTICK_DOWNLEFT 22500
#define JOYSTICK_DOWNRIGHT 13500

enum ORGF_MIRROR_TYPE {ORGF_MIRROR_HORIZONTAL=0,ORGF_MIRROR_VERTICAL=1};
enum ORGF_BACKGROUND_TYPE {ORGF_NORMAL_BACKGROUND=0,ORGF_HORIZONTAL_BACKGROUND=1,ORGF_VERTICAL_BACKGROUND=2};
enum ORGF_SPRITE_TYPE {ORGF_SINGE_SPRITE=0,ORGF_HORIZONTAL_STRIP=1,ORGF_VERTICAL_STRIP=2};
enum ORGF_GAMEPAD_DIRECTION {ORGF_NEUTRAL_DIRECTION=0,ORGF_NEGATIVE_DIRECTION=-1,ORGF_POSITIVE_DIRECTION=1};
enum ORGF_GAMEPAD_STICKS {ORGF_GAMEPAD_LEFT_STICK=0,ORGF_GAMEPAD_RIGHT_STICK=1};
enum ORGF_GAMEPAD_DPAD {ORGF_GAMEPAD_NONE=0,ORGF_GAMEPAD_UP=1,ORGF_GAMEPAD_DOWN=2,ORGF_GAMEPAD_LEFT=3,ORGF_GAMEPAD_RIGHT=4,ORGF_GAMEPAD_UPLEFT=5,ORGF_GAMEPAD_UPRIGHT=6,ORGF_GAMEPAD_DOWNLEFT=7,ORGF_GAMEPAD_DOWNRIGHT=8};
enum ORGF_GAMEPAD_BUTTONS {ORGF_GAMEPAD_BUTTON1=JOY_BUTTON1,ORGF_GAMEPAD_BUTTON2=JOY_BUTTON2,ORGF_GAMEPAD_BUTTON3=JOY_BUTTON3,ORGF_GAMEPAD_BUTTON4=JOY_BUTTON4,ORGF_GAMEPAD_BUTTON5=JOY_BUTTON5,ORGF_GAMEPAD_BUTTON6=JOY_BUTTON6,ORGF_GAMEPAD_BUTTON7=JOY_BUTTON7,ORGF_GAMEPAD_BUTTON8=JOY_BUTTON8,ORGF_GAMEPAD_BUTTON9=JOY_BUTTON9,ORGF_GAMEPAD_BUTTON10=JOY_BUTTON10,ORGF_GAMEPAD_BUTTON11=JOY_BUTTON11,ORGF_GAMEPAD_BUTTON12=JOY_BUTTON12,ORGF_GAMEPAD_BUTTON113=JOY_BUTTON13,ORGF_GAMEPAD_BUTTON14=JOY_BUTTON14,ORGF_GAMEPAD_BUTTON15=JOY_BUTTON15,ORGF_GAMEPAD_BUTTON16=JOY_BUTTON16,ORGF_GAMEPAD_BUTTON17=JOY_BUTTON17,ORGF_GAMEPAD_BUTTON18=JOY_BUTTON18,ORGF_GAMEPAD_BUTTON19=JOY_BUTTON19,ORGF_GAMEPAD_BUTTON20=JOY_BUTTON20,ORGF_GAMEPAD_BUTTON21=JOY_BUTTON21,ORGF_GAMEPAD_BUTTON22=JOY_BUTTON22,ORGF_GAMEPAD_BUTTON23=JOY_BUTTON23,ORGF_GAMEPAD_BUTTON24=JOY_BUTTON24,ORGF_GAMEPAD_BUTTON25=JOY_BUTTON25,ORGF_GAMEPAD_BUTTON26=JOY_BUTTON26,ORGF_GAMEPAD_BUTTON27=JOY_BUTTON27,ORGF_GAMEPAD_BUTTON28=JOY_BUTTON28,ORGF_GAMEPAD_BUTTON29=JOY_BUTTON29,ORGF_GAMEPAD_BUTTON30=JOY_BUTTON30,ORGF_GAMEPAD_BUTTON31=JOY_BUTTON31,ORGF_GAMEPAD_BUTTON32=JOY_BUTTON32};

struct ORGF_Color
{
 unsigned char blue:8;
 unsigned char green:8;
 unsigned char red:8;
};

struct TGA_head
{
 unsigned char id:8;
 unsigned char color_map:8;
 unsigned char type:8;
};

struct TGA_map
{
 unsigned short int index:16;
 unsigned short int length:16;
 unsigned char map_size:8;
};

struct TGA_image
{
 unsigned short int x:16;
 unsigned short int y:16;
 unsigned short int width:16;
 unsigned short int height:16;
 unsigned char color:8;
 unsigned char alpha:3;
 unsigned char direction:5;
};

struct PCX_head
{
 unsigned char vendor:8;
 unsigned char version:8;
 unsigned char compress:8;
 unsigned char color:8;
 unsigned short int min_x:16;
 unsigned short int min_y:16;
 unsigned short int max_x:16;
 unsigned short int max_y:16;
 unsigned short int vertical_dpi:16;
 unsigned short int horizontal_dpi:16;
 unsigned char palette[48];
 unsigned char reversed:8;
 unsigned char planes:8;
 unsigned short int plane_length:16;
 unsigned short int palette_type:16;
 unsigned short int screen_width:16;
 unsigned short int screen_height:16;
 unsigned char filled[54];
};

struct ORGF_Box
{
 unsigned long int x:32;
 unsigned long int y:32;
 unsigned long int width:32;
 unsigned long int height:32;
};

LRESULT CALLBACK ORGF_Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam);
void ORGF_Show_Error(const char *message);

class ORGF_Base
{
 public:
 ORGF_Base();
 ~ORGF_Base();
};

class ORGF_Synchronization
{
 private:
 HANDLE timer;
 protected:
 void create_timer();
 void set_timer(const unsigned long int interval);
 void wait_timer();
 public:
 ORGF_Synchronization();
 ~ORGF_Synchronization();
};

class ORGF_Engine
{
 private:
 WNDCLASS window_class;
 HWND window;
 unsigned long int width;
 unsigned long int height;
 protected:
 HWND get_window();
 void prepare_engine();
 void create_window();
 void destroy_window();
 void capture_mouse();
 bool process_message();
 public:
 ORGF_Engine();
 ~ORGF_Engine();
 unsigned long int get_width();
 unsigned long int get_height();
};

class ORGF_Frame
{
 private:
 size_t pixels;
 unsigned long int frame_width;
 unsigned long int frame_height;
 unsigned int *buffer;
 unsigned int *shadow;
 unsigned int get_rgb(const unsigned int red,const unsigned int green,const unsigned int blue);
 size_t get_offset(const unsigned long int x,const unsigned long int y);
 protected:
 void set_size(const unsigned long int surface_width,const unsigned long int surface_height);
 unsigned int *create_buffer(const char *error);
 void create_buffers();
 unsigned int *get_buffer();
 public:
 ORGF_Frame();
 ~ORGF_Frame();
 void draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned char red,const unsigned char green,const unsigned char blue);
 void clear_screen();
 void save();
 void restore();
 unsigned long int get_frame_width();
 unsigned long int get_frame_height();
};

class ORGF_FPS
{
 private:
 time_t start;
 unsigned long int current;
 unsigned long int fps;
 protected:
 void update_counter();
 public:
 ORGF_FPS();
 ~ORGF_FPS();
 unsigned long int get_fps();
};

class ORGF_Display
{
 private:
 DEVMODE display;
 void get_video_mode();
 void set_video_mode();
 protected:
 void check_video_mode();
 void set_display_mode(const unsigned long int screen_width,const unsigned long int screen_height);
 public:
 ORGF_Display();
 ~ORGF_Display();
};

class ORGF_Render:public ORGF_Display, public ORGF_Engine, public ORGF_Frame
{
 private:
 BITMAPINFO setting;
 protected:
 void set_render_setting();
 void create_render();
 void refresh();
 public:
 ORGF_Render();
 ~ORGF_Render();
};

class ORGF_Screen:public ORGF_FPS, public ORGF_Synchronization, public ORGF_Render
{
 public:
 void initialize();
 void initialize(const unsigned long int surface_width,const unsigned long int surface_height);
 void set_mode(const unsigned long int screen_width,const unsigned long int screen_height);
 bool update();
 bool sync();
 ORGF_Screen* get_handle();
};

class ORGF_Keyboard
{
 private:
 unsigned char *preversion;
 public:
 ORGF_Keyboard();
 ~ORGF_Keyboard();
 void initialize();
 bool check_hold(const unsigned char code);
 bool check_press(const unsigned char code);
 bool check_release(const unsigned char code);
};

class ORGF_Mouse
{
 private:
 unsigned char preversion[ORGF_MOUSE];
 public:
 ORGF_Mouse();
 ~ORGF_Mouse();
 void show();
 void hide();
 void set_position(const unsigned long int x,const unsigned long int y);
 unsigned long int get_x();
 unsigned long int get_y();
 bool check_hold(const unsigned char button);
 bool check_press(const unsigned char button);
 bool check_release(const unsigned char button);
};

class ORGF_Gamepad
{
 private:
 JOYINFOEX current;
 JOYINFOEX preversion;
 JOYCAPS configuration;
 unsigned int active;
 bool read_configuration();
 bool read_state();
 void clear_state();
 bool check_button(const ORGF_GAMEPAD_BUTTONS button,const JOYINFOEX &target);
 public:
 ORGF_Gamepad();
 ~ORGF_Gamepad();
 unsigned int get_amount();
 unsigned int get_button_amount();
 bool check_connection();
 void update();
 unsigned long int get_sticks_amount();
 void set_active(const unsigned int gamepad);
 unsigned int get_active();
 ORGF_GAMEPAD_DPAD get_dpad();
 ORGF_GAMEPAD_DIRECTION get_stick_x(const ORGF_GAMEPAD_STICKS stick);
 ORGF_GAMEPAD_DIRECTION get_stick_y(const ORGF_GAMEPAD_STICKS stick);
 bool check_hold(const ORGF_GAMEPAD_BUTTONS button);
 bool check_press(const ORGF_GAMEPAD_BUTTONS button);
 bool check_release(const ORGF_GAMEPAD_BUTTONS button);
};

class ORGF_Multimedia:public ORGF_Base
{
 private:
 IGraphBuilder *loader;
 IMediaControl *player;
 IMediaSeeking *controler;
 IVideoWindow *video;
 wchar_t *convert_file_name(const char *target);
 void open(const wchar_t *target);
 bool is_end();
 void rewind();
 public:
 ORGF_Multimedia();
 ~ORGF_Multimedia();
 void initialize();
 void load(const char *target);
 bool check_playing();
 void stop();
 void play();
};

class ORGF_Memory
{
 private:
 MEMORYSTATUSEX memory;
 void get_status();
 public:
 ORGF_Memory();
 ~ORGF_Memory();
 unsigned long long int get_total_memory();
 unsigned long long int get_free_memory();
};

class ORGF_System
{
 public:
 ORGF_System();
 ~ORGF_System();
 unsigned long int get_random(const unsigned long int number);
 void quit();
 void run(const char *command);
 char* read_environment(const char *variable);
 void enable_logging(const char *name);
};

class ORGF_File
{
 private:
 FILE *target;
 public:
 ORGF_File();
 ~ORGF_File();
 void open(const char *name);
 void close();
 void set_position(const off_t offset);
 long int get_position();
 long int get_length();
 void read(void *buffer,const size_t length);
 void write(void *buffer,const size_t length);
 bool check_error();
};

class ORGF_Timer
{
 private:
 unsigned long int interval;
 time_t start;
 public:
 ORGF_Timer();
 ~ORGF_Timer();
 void set_timer(const unsigned long int seconds);
 bool check_timer();
};

class ORGF_Primitive
{
 private:
 ORGF_Color color;
 ORGF_Screen *surface;
 public:
 ORGF_Primitive();
 ~ORGF_Primitive();
 void initialize(ORGF_Screen *Screen);
 void set_color(const unsigned char red,const unsigned char green,const unsigned char blue);
 void draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2);
 void draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
 void draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
};

class ORGF_Image
{
 private:
 unsigned long int width;
 unsigned long int height;
 unsigned char *data;
 unsigned char *create_buffer(const size_t length);
 void clear_buffer();
 FILE *open_image(const char *name);
 unsigned long int get_file_size(FILE *target);
 public:
 ORGF_Image();
 ~ORGF_Image();
 void load_tga(const char *name);
 void load_pcx(const char *name);
 unsigned long int get_width();
 unsigned long int get_height();
 size_t get_data_length();
 unsigned char *get_data();
 void destroy_image();
};

class ORGF_Canvas
{
 private:
 unsigned long int width;
 unsigned long int height;
 unsigned long int frames;
 ORGF_Screen *surface;
 void clear_buffer();
 protected:
 ORGF_Color *image;
 void save();
 void restore();
 void set_width(const unsigned long int image_width);
 void set_height(const unsigned long int image_height);
 ORGF_Color *create_buffer(const unsigned long int image_width,const unsigned long int image_height);
 void draw_image_pixel(const size_t offset,const unsigned long int x,const unsigned long int y);
 size_t get_offset(const unsigned long int start,const unsigned long int x,const unsigned long int y);
 public:
 ORGF_Canvas();
 ~ORGF_Canvas();
 ORGF_Color *get_image();
 size_t get_length();
 unsigned long int get_image_width();
 unsigned long int get_image_height();
 void set_frames(const unsigned long int amount);
 unsigned long int get_frames();
 void initialize(ORGF_Screen *Screen);
 void load_image(ORGF_Image &buffer);
 void mirror_image(const ORGF_MIRROR_TYPE kind);
 void resize_image(const unsigned long int new_width,const unsigned long int new_height);
};

class ORGF_Background:public ORGF_Canvas
{
 private:
 unsigned long int start;
 unsigned long int background_width;
 unsigned long int background_height;
 unsigned long int frame;
 unsigned long int current;
 ORGF_BACKGROUND_TYPE current_kind;
 void draw_background_pixel(const unsigned long int x,const unsigned long int y);
 void slow_draw_background();
 public:
 ORGF_Background();
 ~ORGF_Background();
 void set_kind(ORGF_BACKGROUND_TYPE kind);
 void set_target(const unsigned long int target);
 void draw_background();
};

class ORGF_Sprite:public ORGF_Canvas
{
 private:
 unsigned long int current_x;
 unsigned long int current_y;
 unsigned long int sprite_width;
 unsigned long int sprite_height;
 unsigned long int frame;
 unsigned long int start;
 ORGF_SPRITE_TYPE current_kind;
 bool compare_pixels(const ORGF_Color &first,const ORGF_Color &second);
 void draw_sprite_pixel(const size_t offset,const unsigned long int x,const unsigned long int y);
 public:
 ORGF_Sprite();
 ~ORGF_Sprite();
 void set_x(const unsigned long int x);
 void set_y(const unsigned long int y);
 unsigned long int get_x();
 unsigned long int get_y();
 unsigned long int get_width();
 unsigned long int get_height();
 ORGF_Sprite* get_handle();
 ORGF_Box get_box();
 void set_kind(const ORGF_SPRITE_TYPE kind);
 ORGF_SPRITE_TYPE get_kind();
 void set_target(const unsigned long int target);
 void set_position(const unsigned long int x,const unsigned long int y);
 void clone(ORGF_Sprite &target);
 void draw_sprite();
};

class ORGF_Text
{
 private:
 unsigned long int current_x;
 unsigned long int current_y;
 unsigned long int step_x;
 ORGF_Sprite *sprite;
 void draw_character(const char target);
 public:
 ORGF_Text();
 ~ORGF_Text();
 void set_position(const unsigned long int x,const unsigned long int y);
 void load_font(ORGF_Sprite *font);
 void draw_text(const char *text);
};

class ORGF_Collision
{
 public:
 bool check_horizontal_collision(const ORGF_Box &first,const ORGF_Box &second);
 bool check_vertical_collision(const ORGF_Box &first,const ORGF_Box &second);
 bool check_collision(const ORGF_Box &first,const ORGF_Box &second);
};