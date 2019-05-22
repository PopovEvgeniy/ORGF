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

//Uncomment follow lines if you will compile the code under Visual C++ 2017 or higher
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

#define GETSCANCODE(argument) ((argument >> 16)&0x7f)

#define KEYBOARD 256
#define MOUSE 3

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 2

#define JOYSTICK_UPLEFT 31500
#define JOYSTICK_UPRIGHT 4500
#define JOYSTICK_DOWNLEFT 22500
#define JOYSTICK_DOWNRIGHT 13500

enum MIRROR_TYPE {MIRROR_HORIZONTAL=0,MIRROR_VERTICAL=1};
enum BACKGROUND_TYPE {NORMAL_BACKGROUND=0,HORIZONTAL_BACKGROUND=1,VERTICAL_BACKGROUND=2};
enum SPRITE_TYPE {SINGLE_SPRITE=0,HORIZONTAL_STRIP=1,VERTICAL_STRIP=2};
enum SURFACE {SURFACE_SMALL=0,SURFACE_LARGE=1};
enum GAMEPAD_DIRECTION {GAMEPAD_NEUTRAL_DIRECTION=0,GAMEPAD_NEGATIVE_DIRECTION=-1,GAMEPAD_POSITIVE_DIRECTION=1};
enum GAMEPAD_STICKS {GAMEPAD_LEFT_STICK=0,GAMEPAD_RIGHT_STICK=1};
enum GAMEPAD_DPAD {GAMEPAD_NONE=0,GAMEPAD_UP=1,GAMEPAD_DOWN=2,GAMEPAD_LEFT=3,GAMEPAD_RIGHT=4,GAMEPAD_UPLEFT=5,GAMEPAD_UPRIGHT=6,GAMEPAD_DOWNLEFT=7,GAMEPAD_DOWNRIGHT=8};
enum GAMEPAD_BUTTONS {GAMEPAD_BUTTON1=JOY_BUTTON1,GAMEPAD_BUTTON2=JOY_BUTTON2,GAMEPAD_BUTTON3=JOY_BUTTON3,GAMEPAD_BUTTON4=JOY_BUTTON4,GAMEPAD_BUTTON5=JOY_BUTTON5,GAMEPAD_BUTTON6=JOY_BUTTON6,GAMEPAD_BUTTON7=JOY_BUTTON7,GAMEPAD_BUTTON8=JOY_BUTTON8,GAMEPAD_BUTTON9=JOY_BUTTON9,GAMEPAD_BUTTON10=JOY_BUTTON10,GAMEPAD_BUTTON11=JOY_BUTTON11,GAMEPAD_BUTTON12=JOY_BUTTON12,GAMEPAD_BUTTON113=JOY_BUTTON13,GAMEPAD_BUTTON14=JOY_BUTTON14,GAMEPAD_BUTTON15=JOY_BUTTON15,GAMEPAD_BUTTON16=JOY_BUTTON16,GAMEPAD_BUTTON17=JOY_BUTTON17,GAMEPAD_BUTTON18=JOY_BUTTON18,GAMEPAD_BUTTON19=JOY_BUTTON19,GAMEPAD_BUTTON20=JOY_BUTTON20,GAMEPAD_BUTTON21=JOY_BUTTON21,GAMEPAD_BUTTON22=JOY_BUTTON22,GAMEPAD_BUTTON23=JOY_BUTTON23,GAMEPAD_BUTTON24=JOY_BUTTON24,GAMEPAD_BUTTON25=JOY_BUTTON25,GAMEPAD_BUTTON26=JOY_BUTTON26,GAMEPAD_BUTTON27=JOY_BUTTON27,GAMEPAD_BUTTON28=JOY_BUTTON28,GAMEPAD_BUTTON29=JOY_BUTTON29,GAMEPAD_BUTTON30=JOY_BUTTON30,GAMEPAD_BUTTON31=JOY_BUTTON31,GAMEPAD_BUTTON32=JOY_BUTTON32};

struct IMG_Pixel
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

struct Collision_Box
{
 unsigned long int x:32;
 unsigned long int y:32;
 unsigned long int width:32;
 unsigned long int height:32;
};

LRESULT CALLBACK Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam);
void Show_Error(const char *message);

namespace ORGF
{

class COM_Base
{
 public:
 COM_Base();
 ~COM_Base();
};

class Synchronization
{
 private:
 HANDLE timer;
 protected:
 void create_timer();
 void set_timer(const unsigned long int interval);
 void wait_timer();
 public:
 Synchronization();
 ~Synchronization();
};

class Engine
{
 private:
 WNDCLASS window_class;
 HWND window;
 unsigned long int width;
 unsigned long int height;
 protected:
 HWND get_window();
 HDC get_context();
 void prepare_engine();
 void create_window();
 void destroy_window();
 void capture_mouse();
 bool process_message();
 public:
 Engine();
 ~Engine();
 unsigned long int get_width();
 unsigned long int get_height();
};

class Frame
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
 void set_size(const SURFACE surface);
 unsigned int *create_buffer(const char *error);
 void create_buffers();
 unsigned int *get_buffer();
 public:
 Frame();
 ~Frame();
 void draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned char red,const unsigned char green,const unsigned char blue);
 void clear_screen();
 void save();
 void restore();
 unsigned long int get_frame_width();
 unsigned long int get_frame_height();
};

class FPS
{
 private:
 time_t start;
 unsigned long int current;
 unsigned long int fps;
 protected:
 void update_counter();
 public:
 FPS();
 ~FPS();
 unsigned long int get_fps();
};

class Display
{
 private:
 DEVMODE display;
 void get_video_mode();
 void set_video_mode();
 protected:
 void check_video_mode();
 void set_display_mode(const unsigned long int screen_width,const unsigned long int screen_height,const unsigned long int depth);
 void set_display_mode(const unsigned long int screen_width,const unsigned long int screen_height);
 public:
 Display();
 ~Display();
 unsigned long int get_color();
};

class Render:public Display, public Engine, public Frame
{
 private:
 BITMAPINFO setting;
 protected:
 void set_render_setting();
 void create_render();
 void refresh();
 public:
 Render();
 ~Render();
};

class Screen:public FPS, public Synchronization, public Render
{
 public:
 void initialize();
 void initialize(const unsigned long int surface_width,const unsigned long int surface_height);
 void initialize(const SURFACE surface);
 void set_mode(const unsigned long int screen_width,const unsigned long int screen_height);
 void set_mode(const unsigned long int screen_width,const unsigned long int screen_height,const unsigned long int depth);
 bool update();
 bool sync();
 Screen* get_handle();
};

class Keyboard
{
 private:
 unsigned char *preversion;
 unsigned char *create_buffer(const char *error);
 public:
 Keyboard();
 ~Keyboard();
 void initialize();
 bool check_hold(const unsigned char code);
 bool check_press(const unsigned char code);
 bool check_release(const unsigned char code);
};

class Mouse
{
 private:
 unsigned char preversion[MOUSE];
 public:
 Mouse();
 ~Mouse();
 void show();
 void hide();
 void set_position(const unsigned long int x,const unsigned long int y);
 unsigned long int get_x();
 unsigned long int get_y();
 bool check_hold(const unsigned char button);
 bool check_press(const unsigned char button);
 bool check_release(const unsigned char button);
};

class Gamepad
{
 private:
 JOYINFOEX current;
 JOYINFOEX preversion;
 JOYCAPS configuration;
 unsigned int active;
 unsigned int max_amount;
 bool read_configuration();
 bool read_state();
 void clear_state();
 bool check_button(const GAMEPAD_BUTTONS button,const JOYINFOEX &target);
 public:
 Gamepad();
 ~Gamepad();
 unsigned int get_amount();
 unsigned int get_button_amount();
 unsigned int get_last_index();
 bool check_connection();
 void update();
 unsigned long int get_sticks_amount();
 void set_active(const unsigned int gamepad);
 unsigned int get_max_amount();
 unsigned int get_active();
 GAMEPAD_DPAD get_dpad();
 GAMEPAD_DIRECTION get_stick_x(const GAMEPAD_STICKS stick);
 GAMEPAD_DIRECTION get_stick_y(const GAMEPAD_STICKS stick);
 bool check_hold(const GAMEPAD_BUTTONS button);
 bool check_press(const GAMEPAD_BUTTONS button);
 bool check_release(const GAMEPAD_BUTTONS button);
};

class Multimedia:public COM_Base
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
 void create_loader();
 void create_player();
 void create_controler();
 void create_video_player();
 public:
 Multimedia();
 ~Multimedia();
 void initialize();
 void load(const char *target);
 bool check_playing();
 void stop();
 void play();
};

class Memory
{
 private:
 MEMORYSTATUSEX memory;
 void get_status();
 public:
 Memory();
 ~Memory();
 unsigned long long int get_total_physical();
 unsigned long long int get_free_physical();
 unsigned long long int get_total_virtual();
 unsigned long long int get_free_virtual();
 unsigned long int get_usage();
};

class System
{
 public:
 System();
 ~System();
 unsigned long int get_random(const unsigned long int number);
 void quit();
 void run(const char *command);
 char* read_environment(const char *variable);
 void enable_logging(const char *name);
};

class Binary_File
{
 private:
 FILE *target;
 public:
 Binary_File();
 ~Binary_File();
 void open_read(const char *name);
 void open_write(const char *name);
 void close();
 void set_position(const long int offset);
 long int get_position();
 long int get_length();
 void read(void *buffer,const size_t length);
 void write(void *buffer,const size_t length);
 bool check_error();
};

class Timer
{
 private:
 unsigned long int interval;
 time_t start;
 public:
 Timer();
 ~Timer();
 void set_timer(const unsigned long int seconds);
 bool check_timer();
};

class Primitive
{
 private:
 IMG_Pixel color;
 Screen *surface;
 public:
 Primitive();
 ~Primitive();
 void initialize(Screen *Screen);
 void set_color(const unsigned char red,const unsigned char green,const unsigned char blue);
 void draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2);
 void draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
 void draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
};

class Image
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
 Image();
 ~Image();
 void load_tga(const char *name);
 void load_pcx(const char *name);
 unsigned long int get_width();
 unsigned long int get_height();
 size_t get_data_length();
 unsigned char *get_data();
 void destroy_image();
};

class Surface
{
 private:
 Screen *surface;
 protected:
 unsigned long int width;
 unsigned long int height;
 IMG_Pixel *image;
 void save();
 void restore();
 void clear_buffer();
 IMG_Pixel *create_buffer(const unsigned long int image_width,const unsigned long int image_height);
 void set_width(const unsigned long int image_width);
 void set_height(const unsigned long int image_height);
 size_t get_offset(const unsigned long int start,const unsigned long int x,const unsigned long int y);
 void draw_image_pixel(const size_t offset,const unsigned long int x,const unsigned long int y);
 public:
 Surface();
 ~Surface();
 void initialize(Screen *screen);
 size_t get_length();
 IMG_Pixel *get_image();
};

class Canvas:public Surface
{
 private:
 unsigned long int frames;
 public:
 Canvas();
 ~Canvas();
 unsigned long int get_image_width();
 unsigned long int get_image_height();
 void set_frames(const unsigned long int amount);
 unsigned long int get_frames();
 void load_image(Image &buffer);
 void mirror_image(const MIRROR_TYPE kind);
 void resize_image(const unsigned long int new_width,const unsigned long int new_height);
};

class Background:public Canvas
{
 private:
 unsigned long int start;
 unsigned long int background_width;
 unsigned long int background_height;
 unsigned long int frame;
 unsigned long int current;
 BACKGROUND_TYPE current_kind;
 void draw_background_pixel(const unsigned long int x,const unsigned long int y);
 void slow_draw_background();
 public:
 Background();
 ~Background();
 void set_kind(BACKGROUND_TYPE kind);
 void set_target(const unsigned long int target);
 void draw_background();
};

class Sprite:public Canvas
{
 private:
 bool transparent;
 unsigned long int current_x;
 unsigned long int current_y;
 unsigned long int sprite_width;
 unsigned long int sprite_height;
 unsigned long int frame;
 unsigned long int start;
 SPRITE_TYPE current_kind;
 bool compare_pixels(const IMG_Pixel &first,const IMG_Pixel &second);
 void draw_sprite_pixel(const size_t offset,const unsigned long int x,const unsigned long int y);
 public:
 Sprite();
 ~Sprite();
 void set_transparent(const bool enabled);
 bool get_transparent();
 void set_x(const unsigned long int x);
 void set_y(const unsigned long int y);
 unsigned long int get_x();
 unsigned long int get_y();
 unsigned long int get_width();
 unsigned long int get_height();
 Sprite* get_handle();
 Collision_Box get_box();
 void set_kind(const SPRITE_TYPE kind);
 SPRITE_TYPE get_kind();
 void set_target(const unsigned long int target);
 void set_position(const unsigned long int x,const unsigned long int y);
 void clone(Sprite &target);
 void draw_sprite();
};

class Tileset:public Surface
{
 private:
 unsigned long int offset;
 unsigned long int tile_width;
 unsigned long int tile_height;
 unsigned long int rows;
 unsigned long int columns;
 public:
 Tileset();
 ~Tileset();
 unsigned long int get_tile_width();
 unsigned long int get_tile_height();
 unsigned long int get_rows();
 unsigned long int get_columns();
 void select_tile(const unsigned long int row,const unsigned long int column);
 void draw_tile(const unsigned long int x,const unsigned long int y);
 void load_tileset(Image &buffer,const unsigned long int row_amount,const unsigned long int column_amount);
};

class Text
{
 private:
 unsigned long int current_x;
 unsigned long int current_y;
 unsigned long int step_x;
 Sprite *font;
 void draw_character(const char target);
 public:
 Text();
 ~Text();
 void set_position(const unsigned long int x,const unsigned long int y);
 void load_font(Sprite *target);
 void draw_text(const char *text);
};

class Collision
{
 public:
 Collision_Box generate_box(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
 bool check_horizontal_collision(const Collision_Box &first,const Collision_Box &second);
 bool check_vertical_collision(const Collision_Box &first,const Collision_Box &second);
 bool check_collision(const Collision_Box &first,const Collision_Box &second);
};

};