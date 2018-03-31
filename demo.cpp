#include "ORGF.h"

int main(void)
{
 long int x,y,screen_width,screen_height;
 unsigned char frame;
 unsigned long int fps;
 char perfomance[8];
 ORGF_Timer timer;
 ORGF_Screen screen;
 ORGF_System System;
 ORGF_Keyboard keyboard;
 ORGF_Mouse mouse;
 ORGF_Gamepad gamepad;
 ORGF_Multimedia media;
 ORGF_Image image;
 ORGF_Background space;
 ORGF_Sprite ship,font;
 ORGF_Text text;
 System.enable_logging("log.txt");
 keyboard.initialize();
 space.initialize(screen.get_handle());
 ship.initialize(screen.get_handle());
 font.initialize(screen.get_handle());
 screen_width=screen.get_frame_width();
 screen_height=screen.get_frame_height();
 x=screen_width/2;
 y=screen_height/2;
 image.load_tga("space.tga");
 space.load_image(image);
 image.load_tga("ship.tga");
 ship.load_image(image);
 image.load_tga("font.tga");
 font.load_image(image);
 text.load_font(font.get_handle());
 text.set_position(font.get_sprite_width(),font.get_sprite_width());
 ship.set_frames(2);
 screen.clear_screen();
 space.resize_image(screen_width,screen_height);
 frame=1;
 mouse.hide();
 timer.set_timer(1);
 media.initialize();
 media.load("space.mp3");
 fps=0;
 memset(perfomance,0,8);
 screen.initialize();
 while(screen.sync()==false)
 {
  fps++;
  gamepad.update();
  if(media.check_playing()==false) media.play();
  if(mouse.check_press(ORGF_MOUSE_LEFT)==true) break;
  if(keyboard.check_hold(57)==true) break;
  if(keyboard.check_hold(72)==true) y-=2;
  if(keyboard.check_hold(80)==true) y+=2;
  if(keyboard.check_hold(75)==true) x-=2;
  if(keyboard.check_hold(77)==true) x+=2;
  if(keyboard.check_press(71)==true) ship.mirror_image(0);
  if(keyboard.check_press(79)==true) ship.mirror_image(1);
  if(gamepad.check_button_press(ORGF_GAMEPAD_X)==true) break;
  if(gamepad.check_button_hold(ORGF_GAMEPAD_UP)==true) y-=2;
  if(gamepad.check_button_hold(ORGF_GAMEPAD_DOWN)==true) y+=2;
  if(gamepad.check_button_hold(ORGF_GAMEPAD_LEFT)==true) x-=2;
  if(gamepad.check_button_hold(ORGF_GAMEPAD_RIGHT)==true) x+=2;
  if(gamepad.get_stick_x(ORGF_GAMEPAD_LEFT_STICK)==1) x+=2;
  if(gamepad.get_stick_x(ORGF_GAMEPAD_LEFT_STICK)==-1) x-=2;
  if(gamepad.get_stick_y(ORGF_GAMEPAD_LEFT_STICK)==1) y-=2;
  if(gamepad.get_stick_y(ORGF_GAMEPAD_LEFT_STICK)==-1) y+=2;
  if(gamepad.check_trigger_hold(ORGF_GAMEPAD_LEFT_TRIGGER)==true) gamepad.set_vibration(65535,65535);
  if(gamepad.check_trigger_hold(ORGF_GAMEPAD_RIGHT_TRIGGER)==true) gamepad.set_vibration(0,0);
  if(gamepad.check_button_press(ORGF_GAMEPAD_A)==true) ship.mirror_image(0);
  if(gamepad.check_button_press(ORGF_GAMEPAD_B)==true) ship.mirror_image(1);
  if((x<=0)||(x>=screen_width)) x=screen_width/2;
  if((y<=0)||(y>=screen_height)) y=screen_height/2;
  space.draw_background();
  text.draw_text(perfomance);
  ship.draw_sprite_frame(x,y,frame);
  if (timer.check_timer()==true)
  {
   itoa(fps,perfomance,10);
   fps=0;
   frame++;
   if (frame>ship.get_frames()) frame=1;
  }

 }
 return 0;
}