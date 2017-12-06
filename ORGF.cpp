/*
Orthodox game framework was create by Popov Evgeniy Alekseyevich
Some code bases on code from SVGALib(http://www.svgalib.org/).

Orthodox game framework license

Copyright � 2017, Popov Evgeniy Alekseyevich

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Third�party code license

SVGALib is public domain. SVGALib homepage: http://www.svgalib.org/
*/

#include "ORGF.h"

unsigned char ORGF_Keys[ORGF_KEYBOARD];
unsigned char ORGF_Buttons[ORGF_MOUSE];

LRESULT CALLBACK ORGF_Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam)
{
 switch (Message)
 {
  case WM_CLOSE:
  DestroyWindow(window);
  break;
  case WM_DESTROY:
  PostQuitMessage(0);
  break;
  case WM_CREATE:
  memset(ORGF_Keys,ORGFKEY_NONE,ORGF_KEYBOARD);
  memset(ORGF_Buttons,ORGFKEY_NONE,ORGF_MOUSE);
  break;
  case WM_LBUTTONDOWN:
  ORGF_Buttons[ORGF_MOUSE_LEFT]=ORGFKEY_PRESS;
  break;
  case WM_LBUTTONUP:
  ORGF_Buttons[ORGF_MOUSE_LEFT]=ORGFKEY_RELEASE;
  break;
  case WM_RBUTTONDOWN:
  ORGF_Buttons[ORGF_MOUSE_RIGHT]=ORGFKEY_PRESS;
  break;
  case WM_RBUTTONUP:
  ORGF_Buttons[ORGF_MOUSE_RIGHT]=ORGFKEY_RELEASE;
  break;
  case WM_MBUTTONDOWN:
  ORGF_Buttons[ORGF_MOUSE_MIDDLE]=ORGFKEY_PRESS;
  break;
  case WM_MBUTTONUP:
  ORGF_Buttons[ORGF_MOUSE_MIDDLE]=ORGFKEY_RELEASE;
  break;
  case WM_KEYDOWN:
  ORGF_Keys[(lParam >> 16)&0x7f]=ORGFKEY_PRESS;
  break;
  case WM_KEYUP:
  ORGF_Keys[(lParam >> 16)&0x7f]=ORGFKEY_RELEASE;
  break;
 }
 return DefWindowProc(window,Message,wParam,lParam);
}

ORGF_Base::ORGF_Base()
{
 HRESULT status;
 status=CoInitialize(NULL);
 if(status!=S_OK)
 {
  if(status!=S_FALSE)
  {
   puts("Can't initialize COM");
   exit(EXIT_FAILURE);
  }

 }

}

ORGF_Base::~ORGF_Base()
{
 CoUninitialize();
}

ORGF_Synchronization::ORGF_Synchronization()
{
 timer=CreateWaitableTimer(NULL,FALSE,NULL);
 if (timer==NULL)
 {
  puts("Can't create synchronization timer");
  exit(EXIT_FAILURE);
 }

}

ORGF_Synchronization::~ORGF_Synchronization()
{
 CancelWaitableTimer(timer);
 CloseHandle(timer);
}

void ORGF_Synchronization::set_timer(unsigned long int interval)
{
 LARGE_INTEGER start;
 start.QuadPart=0;
 if(SetWaitableTimer(timer,&start,interval,NULL,NULL,FALSE)==FALSE)
 {
  puts("Can't set timer");
  exit(EXIT_FAILURE);
 }

}

void ORGF_Synchronization::wait_timer()
{
 WaitForSingleObject(timer,INFINITE);
}

ORGF_Engine::ORGF_Engine()
{
 window_class.lpszClassName=TEXT("ORGF");
 window_class.hInstance=GetModuleHandle(NULL);
 window_class.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
 window_class.lpfnWndProc=(WNDPROC)ORGF_Process_Message;
 window_class.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
 window_class.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 window_class.hCursor=LoadCursor(NULL,IDC_ARROW);
 window_class.cbClsExtra=0;
 window_class.cbWndExtra=0;
 if (window_class.hCursor==NULL)
 {
  puts("Can't load the standart cursor");
  exit(EXIT_FAILURE);
 }
 if (window_class.hIcon==NULL)
 {
  puts("Can't load the standart program icon");
  exit(EXIT_FAILURE);
 }
 if (RegisterClass(&window_class)==0)
 {
  puts("Can't register window class");
  exit(EXIT_FAILURE);
 }
 width=GetSystemMetrics(SM_CXSCREEN);
 height=GetSystemMetrics(SM_CYSCREEN);
}

ORGF_Engine::~ORGF_Engine()
{
 if(window!=NULL) CloseWindow(window);
 UnregisterClass(window_class.lpszClassName,window_class.hInstance);
}

void ORGF_Engine::create_window()
{
 window=CreateWindow(window_class.lpszClassName,NULL,WS_VISIBLE|WS_POPUP,0,0,width,height,NULL,NULL,window_class.hInstance,NULL);
 if (window==NULL)
 {
  puts("Can't create window");
  exit(EXIT_FAILURE);
 }
 EnableWindow(window,TRUE);
 SetFocus(window);
}

void ORGF_Engine::capture_mouse()
{
 RECT border;
 if(GetClientRect(window,&border)==FALSE)
 {
  puts("Can't capture window");
  exit(EXIT_FAILURE);
 }
 if(ClipCursor(&border)==FALSE)
 {
  puts("Can't capture cursor");
  exit(EXIT_FAILURE);
 }

}

bool ORGF_Engine::process_message()
{
 bool quit;
 MSG Message;
 quit=false;
 while(PeekMessage(&Message,window,0,0,PM_NOREMOVE)==TRUE)
 {
  if(GetMessage(&Message,window,0,0)==TRUE)
  {
   TranslateMessage(&Message);
   DispatchMessage(&Message);
  }
  else
  {
   quit=true;
   break;
  }

 }
 return quit;
}

unsigned long int ORGF_Engine::get_width()
{
 return width;
}

unsigned long int ORGF_Engine::get_height()
{
 return height;
}

ORGF_Frame::ORGF_Frame()
{
 frame_width=512;
 frame_height=512;
 buffer=NULL;
}

ORGF_Frame::~ORGF_Frame()
{
 if(buffer!=NULL)
 {
  free(buffer);
  buffer=NULL;
 }

}

void ORGF_Frame::create_render_buffer()
{
 length=frame_width*frame_height*sizeof(unsigned long int);
 buffer=(unsigned long int*)calloc(length,1);
 if(buffer==NULL)
 {
  puts("Can't allocate memory for render buffer");
  exit(EXIT_FAILURE);
 }

}

void ORGF_Frame::draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned long int red,const unsigned long int green,const unsigned long int blue)
{
 if((x<frame_width)&&(y<frame_height))
 {
  buffer[x+y*frame_width]=blue+256*green+65536*red; //This code bases on code from SVGALib
 }

}

void ORGF_Frame::clear_screen()
{
 memset(buffer,0,length);
}

unsigned long int ORGF_Frame::get_frame_width()
{
 return frame_width;
}

unsigned long int ORGF_Frame::get_frame_height()
{
 return frame_height;
}

ORGF_Render::ORGF_Render()
{
 memset(&setting,0,sizeof(BITMAPINFO));
 context=NULL;
}

ORGF_Render::~ORGF_Render()
{

}

void ORGF_Render::set_render_setting()
{
 setting.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
 setting.bmiHeader.biWidth=frame_width;
 setting.bmiHeader.biHeight=-1*(long int)frame_height;
 setting.bmiHeader.biPlanes=1;
 setting.bmiHeader.biBitCount=32;
 setting.bmiHeader.biCompression=BI_RGB;
}

void ORGF_Render::refresh()
{
 context=GetDC(window);
 if(context==NULL)
 {
  puts("Can't get window context");
  exit(EXIT_FAILURE);
 }
 StretchDIBits(context,0,0,width,height,0,0,frame_width,frame_height,buffer,&setting,DIB_RGB_COLORS,SRCCOPY);
 ReleaseDC(window,context);
}

void ORGF_Screen::initialize()
{
 this->create_render_buffer();
 this->create_window();
 this->capture_mouse();
 this->set_render_setting();
 this->set_timer(17);
}

bool ORGF_Screen::sync()
{
 bool quit;
 this->refresh();
 quit=this->process_message();
 this->wait_timer();
 return quit;
}

ORGF_Screen* ORGF_Screen::get_handle()
{
 return this;
}

ORGF_Keyboard::ORGF_Keyboard()
{
 preversion=NULL;
}

ORGF_Keyboard::~ORGF_Keyboard()
{
 if(preversion!=NULL) free(preversion);
}

void ORGF_Keyboard::initialize()
{
 preversion=(unsigned char*)calloc(ORGF_KEYBOARD,1);
 if(preversion==NULL)
 {
  puts("Can't allocate memory for keyboard state buffer");
  exit(EXIT_FAILURE);
 }

}

bool ORGF_Keyboard::check_hold(const unsigned char code)
{
 bool result;
 result=false;
 if(ORGF_Keys[code]==ORGFKEY_PRESS) result=true;
 return result;
}

bool ORGF_Keyboard::check_press(const unsigned char code)
{
 bool result;
 result=false;
 if(ORGF_Keys[code]==ORGFKEY_PRESS)
 {
  if(preversion[code]!=ORGFKEY_PRESS) result=true;
 }
 preversion[code]=ORGF_Keys[code];
 return result;
}

bool ORGF_Keyboard::check_release(const unsigned char code)
{
 bool result;
 result=false;
 if(ORGF_Keys[code]==ORGFKEY_RELEASE)
 {
  result=true;
  ORGF_Keys[code]=ORGFKEY_NONE;
 }
 return result;
}

ORGF_Mouse::ORGF_Mouse()
{
 memset(preversion,ORGFKEY_NONE,ORGF_MOUSE);
}

ORGF_Mouse::~ORGF_Mouse()
{
 while(ShowCursor(TRUE)<1) ;
}

void ORGF_Mouse::show()
{
 while(ShowCursor(TRUE)<1) ;
}

void ORGF_Mouse::hide()
{
 while(ShowCursor(FALSE)>-2) ;
}

void ORGF_Mouse::set_position(const unsigned long int x,const unsigned long int y)
{
 if(SetCursorPos(x,y)==FALSE)
 {
  puts("Can't set the mouse cursor position");
  exit(EXIT_FAILURE);
 }

}

unsigned long int ORGF_Mouse::get_x()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  puts("Can't get the mouse cursor position");
  exit(EXIT_FAILURE);
 }
 return position.x;
}

unsigned long int ORGF_Mouse::get_y()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  puts("Can't get the mouse cursor position");
  exit(EXIT_FAILURE);
 }
 return position.y;
}

bool ORGF_Mouse::check_hold(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=ORGF_MOUSE_MIDDLE)
 {
  if(ORGF_Buttons[button]==ORGFKEY_PRESS) result=true;
 }
 return result;
}

bool ORGF_Mouse::check_press(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=ORGF_MOUSE_MIDDLE)
 {
  if(ORGF_Buttons[button]==ORGFKEY_PRESS)
  {
   if(preversion[button]!=ORGFKEY_PRESS) result=true;
  }

 }
 preversion[button]=ORGF_Buttons[button];
 return result;
}

bool ORGF_Mouse::check_release(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=ORGF_MOUSE_MIDDLE)
 {
  if(ORGF_Buttons[button]==ORGFKEY_RELEASE)
  {
   result=true;
   ORGF_Buttons[button]=ORGFKEY_NONE;
  }

 }
 return result;
}

ORGF_Gamepad::ORGF_Gamepad()
{
 active=ORGF_GAMEPAD1;
 length=sizeof(JOYINFOEX);
 memset(&current,0,length);
 memset(&preversion,0,length);
 current.dwSize=length;
 preversion.dwSize=length;
 current.dwFlags=JOY_RETURNALL;
 preversion.dwFlags=JOY_RETURNALL;
 current.dwPOV=JOY_POVCENTERED;
 preversion.dwPOV=JOY_POVCENTERED;
}

ORGF_Gamepad::~ORGF_Gamepad()
{

}

bool ORGF_Gamepad::read_state()
{
 bool result;
 result=false;
 if(joyGetPosEx(active,&current)==JOYERR_NOERROR) result=true;
 return result;
}

void ORGF_Gamepad::clear_state()
{
 memset(&current,0,length);
 memset(&preversion,0,length);
 current.dwSize=length;
 preversion.dwSize=length;
 current.dwFlags=JOY_RETURNALL;
 preversion.dwFlags=JOY_RETURNALL;
 current.dwPOV=JOY_POVCENTERED;
 preversion.dwPOV=JOY_POVCENTERED;
}

bool ORGF_Gamepad::check_button(const unsigned long int button,JOYINFOEX &target)
{
 bool result;
 result=false;
 if(target.dwButtons&button) result=true;
 return result;
}

void ORGF_Gamepad::set_active(const unsigned int gamepad)
{
 if(active<=ORGF_GAMEPAD15) active=gamepad;
}

unsigned int ORGF_Gamepad::get_active()
{
 return active;
}

unsigned int ORGF_Gamepad::get_amount()
{
 return joyGetNumDevs();
}

bool ORGF_Gamepad::check_connection()
{
 return this->read_state();
}

void ORGF_Gamepad::update()
{
 preversion=current;
 if(this->read_state()==false) this->clear_state();
}

unsigned char ORGF_Gamepad::get_dpad()
{
 unsigned char result;
 result=ORGF_GAMEPAD_NONE;
 switch (current.dwPOV)
 {
  case JOY_POVFORWARD:
  result=ORGF_GAMEPAD_UP;
  break;
  case JOY_POVBACKWARD:
  result=ORGF_GAMEPAD_DOWN;
  break;
  case JOY_POVLEFT:
  result=ORGF_GAMEPAD_LEFT;
  break;
  case JOY_POVRIGHT:
  result=ORGF_GAMEPAD_RIGHT;
  break;
 }
 return result;
}

bool ORGF_Gamepad::check_hold(const unsigned long int button)
{
 return this->check_button(button,current);
}

bool ORGF_Gamepad::check_press(const unsigned long int button)
{
 bool result;
 result=false;
 if(this->check_button(button,current)==true)
 {
  if(this->check_button(button,preversion)==false) result=true;
 }
 return result;
}

bool ORGF_Gamepad::check_release(const unsigned long int button)
{
 bool result;
 result=false;
 if(this->check_button(button,current)==false)
 {
  if(this->check_button(button,preversion)==true) result=true;
 }
 return result;
}

ORGF_Multimedia::ORGF_Multimedia()
{
 loader=NULL;
 player=NULL;
 controler=NULL;
 video=NULL;
}

ORGF_Multimedia::~ORGF_Multimedia()
{
 if(player!=NULL) player->Stop();
 if(video!=NULL) video->Release();
 if(controler!=NULL) controler->Release();
 if(player!=NULL) player->Release();
 if(loader!=NULL) loader->Release();
}

wchar_t *ORGF_Multimedia::convert_file_name(const char *target)
{
 wchar_t *name;
 unsigned long int index,length;
 length=strlen(target);
 name=(wchar_t*)calloc(length+1,sizeof(wchar_t));
 if(name==NULL)
 {
  puts("Can't allocate memory");
  exit(EXIT_FAILURE);
 }
 for(index=0;index<length;index++) name[index]=btowc(target[index]);
 return name;
}

void ORGF_Multimedia::open(const wchar_t *target)
{
 player->Stop();
 if(loader->RenderFile(target,NULL)!=S_OK)
 {
  puts("Can't load a multimedia file");
  exit(EXIT_FAILURE);
 }
 if(controler->SetRate(1)!=S_OK)
 {
  puts("Can't set playing rate");
  exit(EXIT_FAILURE);
 }
 video->put_FullScreenMode(OATRUE);
}

void ORGF_Multimedia::initialize()
{
 if(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&loader)!=S_OK)
 {
  puts("Can't create a multimedia loader");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IMediaControl,(void**)&player)!=S_OK)
 {
  puts("Can't create a multimedia player");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IMediaSeeking,(void**)&controler)!=S_OK)
 {
  puts("Can't create a player controler");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IVideoWindow,(void**)&video)!=S_OK)
 {
  puts("Can't create a video player");
  exit(EXIT_FAILURE);
 }

}

void ORGF_Multimedia::load(const char *target)
{
 wchar_t *name;
 name=this->convert_file_name(target);
 this->open(name);
 free(name);
}

void ORGF_Multimedia::play()
{
 long long position;
 position=0;
 player->Stop();
 if(controler->SetPositions(&position,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning)!=S_OK)
 {
  puts("Can't set start position");
  exit(EXIT_FAILURE);
 }
 player->Run();
}

void ORGF_Multimedia::stop()
{
 player->Stop();
}

bool ORGF_Multimedia::check_playing()
{
 bool result;
 long long current,stop;
 result=false;
 if(controler->GetPositions(&current,&stop)==S_OK)
 {
  if(current<stop) result=true;
 }
 return result;
}

ORGF_Memory::ORGF_Memory()
{
 memory.dwLength=sizeof(MEMORYSTATUSEX);
}

ORGF_Memory::~ORGF_Memory()
{

}

unsigned long long int ORGF_Memory::get_total_memory()
{
 GlobalMemoryStatusEx(&memory);
 return memory.ullTotalPhys;
}

unsigned long long int ORGF_Memory::get_free_memory()
{
 GlobalMemoryStatusEx(&memory);
 return memory.ullAvailPhys;
}

ORGF_System::ORGF_System()
{
 srand(time(NULL));
}

ORGF_System::~ORGF_System()
{

}

unsigned long int ORGF_System::get_random(const unsigned long int number)
{
 return rand()%number;
}

void ORGF_System::quit()
{
 exit(EXIT_SUCCESS);
}

void ORGF_System::run(const char *command)
{
 system(command);
}

char* ORGF_System::read_environment(const char *variable)
{
 return getenv(variable);
}

void ORGF_System::enable_logging(const char *name)
{
 if(freopen(name,"wt",stdout)==NULL)
 {
  puts("Can't create log file");
  exit(EXIT_FAILURE);
 }

}

ORGF_Timer::ORGF_Timer()
{
 interval=0;
 start=time(NULL);
}

ORGF_Timer::~ORGF_Timer()
{

}

void ORGF_Timer::set_timer(const unsigned long int seconds)
{
 interval=seconds;
 start=time(NULL);
}

bool ORGF_Timer::check_timer()
{
 bool result;
 time_t stop;
 result=false;
 stop=time(NULL);
 if(difftime(stop,start)>=interval)
 {
  result=true;
  start=time(NULL);
 }
 return result;
}

ORGF_Primitive::ORGF_Primitive()
{
 surface=NULL;
}

ORGF_Primitive::~ORGF_Primitive()
{
 surface=NULL;
}

void ORGF_Primitive::initialize(ORGF_Screen *Screen)
{
 surface=Screen;
}

void ORGF_Primitive::draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 unsigned long int delta_x,delta_y,index,steps;
 float x,y,shift_x,shift_y;
 if (x1>x2)
 {
  delta_x=x1-x2;
 }
 else
 {
  delta_x=x2-x1;
 }
 if (y1>y2)
 {
  delta_y=y1-y2;
 }
 else
 {
  delta_y=y2-y1;
 }
 steps=delta_x;
 if (steps<delta_y) steps=delta_y;
 x=x1;
 y=y1;
 shift_x=(float)delta_x/(float)steps;
 shift_y=(float)delta_y/(float)steps;
 for (index=steps;index>0;index--)
 {
  x+=shift_x;
  y+=shift_y;
  surface->draw_pixel(x,y,red,green,blue);
 }

}

void ORGF_Primitive::draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 unsigned long int stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 this->draw_line(x,y,stop_x,y,red,green,blue);
 this->draw_line(x,stop_y,stop_x,stop_y,red,green,blue);
 this->draw_line(x,y,x,stop_y,red,green,blue);
 this->draw_line(stop_x,y,stop_x,stop_y,red,green,blue);
}

void ORGF_Primitive::draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 unsigned long int step_x,step_y,stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 for(step_x=x;step_x<stop_x;step_x++)
 {
  for(step_y=y;step_y<stop_y;step_y++)
  {
   surface->draw_pixel(step_x,step_y,red,green,blue);
  }

 }

}

ORGF_Image::ORGF_Image()
{
 width=0;
 height=0;
 data=NULL;
}

ORGF_Image::~ORGF_Image()
{
 if(data!=NULL) free(data);
}

void ORGF_Image::load_tga(const char *name)
{
 FILE *target;
 unsigned long int index,position,amount,compressed_length,uncompressed_length;
 unsigned char *compressed;
 unsigned char *uncompressed;
 TGA_head head;
 TGA_map color_map;
 TGA_image image;
 target=fopen(name,"rb");
 if(target==NULL)
 {
  puts("Can't open a image file");
  exit(EXIT_FAILURE);
 }
 if(data!=NULL)
 {
  free(data);
  data=NULL;
 }
 fseek(target,0,SEEK_END);
 compressed_length=ftell(target)-18;
 rewind(target);
 fread(&head,3,1,target);
 fread(&color_map,5,1,target);
 fread(&image,10,1,target);
 if((head.color_map!=0)||(image.color!=24))
 {
  puts("Invalid image format");
  exit(EXIT_FAILURE);
 }
 if(head.type!=2)
 {
  if(head.type!=10)
  {
   puts("Invalid image format");
   exit(EXIT_FAILURE);
  }

 }
 index=0;
 position=0;
 uncompressed_length=3*(unsigned long int)image.width*(unsigned long int)image.height;
 uncompressed=(unsigned char*)calloc(uncompressed_length,1);
 if(uncompressed==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 if(head.type==2)
 {
  fread(uncompressed,uncompressed_length,1,target);
 }
 if(head.type==10)
 {
  compressed=(unsigned char*)calloc(compressed_length,1);
  if(compressed==NULL)
  {
   puts("Can't allocate memory for image buffer");
   exit(EXIT_FAILURE);
  }
  fread(compressed,compressed_length,1,target);
  while(index<uncompressed_length)
  {
   if(compressed[position]<128)
   {
    amount=compressed[position]+1;
    amount*=3;
    memmove(uncompressed+index,compressed+(position+1),amount);
    index+=amount;
    position+=1+amount;
   }
   else
   {
    for(amount=compressed[position]-127;amount>0;amount--)
    {
     memmove(uncompressed+index,compressed+(position+1),3);
     index+=3;
    }
    position+=4;
   }

  }
  free(compressed);
 }
 fclose(target);
 width=image.width;
 height=image.height;
 data=uncompressed;
}

void ORGF_Image::load_pcx(const char *name)
{
 FILE *target;
 unsigned long int x,y,index,position,line,row,length,uncompressed_length;
 unsigned char repeat;
 unsigned char *original;
 unsigned char *uncompressed;
 PCX_head head;
 target=fopen(name,"rb");
 if(target==NULL)
 {
  puts("Can't open a image file");
  exit(EXIT_FAILURE);
 }
 if(data!=NULL)
 {
  free(data);
  data=NULL;
 }
 fseek(target,0,SEEK_END);
 length=ftell(target)-128;
 rewind(target);
 fread(&head,128,1,target);
 if((head.color*head.planes!=24)&&(head.compress!=1))
 {
  puts("Incorrect image format");
  exit(EXIT_FAILURE);
 }
 width=head.max_x-head.min_x+1;
 height=head.max_y-head.min_y+1;
 row=3*width;
 line=head.planes*head.plane_length;
 uncompressed_length=row*height;
 index=0;
 position=0;
 original=(unsigned char*)calloc(length,1);
 if(original==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 uncompressed=(unsigned char*)calloc(uncompressed_length,1);
 if(uncompressed==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 fread(original,length,1,target);
 fclose(target);
 while (index<length)
 {
  if (original[index]<192)
  {
   uncompressed[position]=original[index];
   position++;
   index++;
  }
  else
  {
   for (repeat=original[index]-192;repeat>0;repeat--)
   {
    uncompressed[position]=original[index+1];
    position++;
   }
   index+=2;
  }

 }
 free(original);
 original=(unsigned char*)calloc(uncompressed_length,1);
 if(original==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 for(x=0;x<width;x++)
 {
  for(y=0;y<height;y++)
  {
   index=x*3+y*row;
   position=x+y*line;
   original[index]=uncompressed[position+2*head.plane_length];
   original[index+1]=uncompressed[position+head.plane_length];
   original[index+2]=uncompressed[position];
  }

 }
 free(uncompressed);
 data=original;
}

unsigned long int ORGF_Image::get_width()
{
 return width;
}

unsigned long int ORGF_Image::get_height()
{
 return height;
}

unsigned long int ORGF_Image::get_data_length()
{
 return width*height*3;
}

unsigned char *ORGF_Image::get_data()
{
 return data;
}

void ORGF_Image::destroy_image()
{
 if(data!=NULL)
 {
  width=0;
  height=0;
  free(data);
  data=NULL;
 }

}

ORGF_Canvas::ORGF_Canvas()
{
 image=NULL;
 surface=NULL;
 width=0;
 height=0;
 frames=1;
}

ORGF_Canvas::~ORGF_Canvas()
{
 surface=NULL;
 if(image!=NULL) free(image);
}

unsigned long int ORGF_Canvas::get_width()
{
 return width;
}

unsigned long int ORGF_Canvas::get_height()
{
 return height;
}

void ORGF_Canvas::set_frames(const unsigned long int amount)
{
 if(amount>1) frames=amount;
}

unsigned long int ORGF_Canvas::get_frames()
{
 return frames;
}

void ORGF_Canvas::initialize(ORGF_Screen *Screen)
{
 surface=Screen;
}

void ORGF_Canvas::load_image(ORGF_Image &buffer)
{
 unsigned long int length;
 width=buffer.get_width();
 height=buffer.get_height();
 length=buffer.get_data_length();
 if(image!=NULL) free(image);
 image=(ORGF_Color*)calloc(length,1);
 if (image==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 memmove(image,buffer.get_data(),length);
 buffer.destroy_image();
}

void ORGF_Canvas::mirror_image(const unsigned char kind)
{
 unsigned long int x,y,index,index2;
 ORGF_Color *mirrored_image;
 mirrored_image=(ORGF_Color*)calloc(width*height,3);
 if (mirrored_image==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 if (kind==0)
 {
  for (x=0;x<width;x++)
  {
   for (y=0;y<height;y++)
   {
    index=x+(y*width);
    index2=(width-x-1)+(y*width);
    mirrored_image[index]=image[index2];
   }

  }

 }
 else
 {
   for (x=0;x<width;x++)
  {
   for (y=0;y<height;y++)
   {
    index=x+(y*width);
    index2=x+(height-y-1)*width;
    mirrored_image[index]=image[index2];
   }

  }

 }
 free(image);
 image=mirrored_image;
}

void ORGF_Canvas::resize_image(const unsigned long int new_width,const unsigned long int new_height)
{
 float x_ratio,y_ratio;
 unsigned long int x,y,index,index2;
 ORGF_Color *scaled_image;
 scaled_image=(ORGF_Color*)calloc(new_width*new_height,3);
 if (scaled_image==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 x_ratio=(float)width/(float)new_width;
 y_ratio=(float)height/(float)new_height;
 for (x=0;x<new_width;x++)
 {
  for (y=0;y<new_height;y++)
  {
   index=x+(y*new_width);
   index2=(unsigned long int)(x_ratio*(float)x)+width*(unsigned long int)(y_ratio*(float)y);
   scaled_image[index]=image[index2];
  }

 }
 free(image);
 image=scaled_image;
 width=new_width;
 height=new_height;
}

void ORGF_Background::draw_background()
{
 unsigned long int x,y,offset;
 for (x=0;x<width;x++)
 {
  for (y=0;y<height;y++)
  {
   offset=x+(width*y);
   surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
  }

 }

}

void ORGF_Background::draw_horizontal_background(const unsigned long int frame)
{
 unsigned long int x,y,offset,start,frame_width;
 frame_width=width/frames;
 start=(frame-1)*frame_width;
 for (x=0;x<frame_width;x++)
 {
  for (y=0;y<height;y++)
  {
   offset=start+x+(width*y);
   surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
  }

 }

}

void ORGF_Background::draw_vertical_background(const unsigned long int frame)
{
 unsigned long int x,y,offset,start,frame_height;
 frame_height=height/frames;
 start=(frame-1)*frame_height;
 for (x=0;x<width;x++)
 {
  for (y=0;y<frame_height;y++)
  {
   offset=start+x+(width*y);
   surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
  }

 }

}

ORGF_Sprite::ORGF_Sprite()
{
 current_x=0;
 current_y=0;
}

ORGF_Sprite::~ORGF_Sprite()
{

}

void ORGF_Sprite::draw_sprite_frame(const unsigned long int x,const unsigned long int y,const unsigned long int frame)
{
 unsigned long int sprite_x,sprite_y,offset,start,frame_width;
 current_x=x;
 current_y=y;
 frame_width=width/frames;
 start=(frame-1)*frame_width;
 for(sprite_x=0;sprite_x<frame_width;sprite_x++)
 {
  for(sprite_y=0;sprite_y<height;sprite_y++)
  {
   offset=start+sprite_x+(sprite_y*width);
   if(memcmp(&image[0],&image[offset],3)!=0) surface->draw_pixel(x+sprite_x,y+sprite_y,image[offset].red,image[offset].green,image[offset].blue);
  }

 }

}

void ORGF_Sprite::draw_sprite(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
 this->draw_sprite_frame(x,y,1);
}

unsigned long int ORGF_Sprite::get_x()
{
 return current_x;
}

unsigned long int ORGF_Sprite::get_y()
{
 return current_y;
}

unsigned long int ORGF_Sprite::get_sprite_width()
{
 return width/frames;
}

unsigned long int ORGF_Sprite::get_sprite_height()
{
 return height;
}

ORGF_Sprite* ORGF_Sprite::get_handle()
{
 return this;
}

ORGF_Box ORGF_Sprite::get_box()
{
 ORGF_Box target;
 target.x=current_x;
 target.y=current_y;
 target.width=width/frames;
 target.height=height;
 return target;
}

ORGF_Text::ORGF_Text()
{
 current_x=0;
 current_y=0;
 sprite=NULL;
}

ORGF_Text::~ORGF_Text()
{
 sprite=NULL;
}

void ORGF_Text::set_position(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
}

void ORGF_Text::load_font(ORGF_Sprite *font)
{
 sprite=font;
 sprite->set_frames(256);
}

void ORGF_Text::draw_text(const char *text)
{
 unsigned long int index,length,step_x,step_y;
 length=strlen(text);
 step_x=current_x;
 step_y=current_y;
 for (index=0;index<length;index++)
 {
  if ((text[index]>31)||(text[index]<0))
  {
   sprite->draw_sprite_frame(step_x,step_y,(unsigned char)text[index]+1);
   step_x+=sprite->get_sprite_width();
  }

 }

}

bool ORGF_Collision::check_horizontal_collision(ORGF_Box first,ORGF_Box second)
{
 bool result;
 result=false;
 if((first.x+first.width)>=second.x)
 {
  if(first.x<=(second.x+second.width)) result=true;
 }
 return result;
}

bool ORGF_Collision::check_vertical_collision(ORGF_Box first,ORGF_Box second)
{
 bool result;
 result=false;
 if((first.y+first.height)>=second.y)
 {
  if(first.y<=(second.y+second.height)) result=true;
 }
 return result;
}

bool ORGF_Collision::check_collision(ORGF_Box first,ORGF_Box second)
{
 bool result;
 result=false;
 if((this->check_horizontal_collision(first,second)==true)||(this->check_vertical_collision(first,second)==true)) result=true;
 return result;
}