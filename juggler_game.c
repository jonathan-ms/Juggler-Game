#include "MPU9250.h"
#include "GLCD_Scroll.h"
#include "type.h"
#include "led.h"
#include "timer.h"
#include "stdio.h"
#include <rtl.h>
#include "joystick.h"
#include "GLCD.h"
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

//Declaring global values
#define P_COL     White
#define B_COL     White
#define B_COL2    Red
#define BG        Black

//Lives variables
U32 lives[4] = {0,1,3,7};
U32 livIndex = 3;

//Mutex Information
OS_MUT mutex1;
OS_MUT mutex2;
OS_TID player, balls, lcd;

//Platform Globals
U32 x_plat[3] = {20,75,130};
U32 y_plat = 30;
U8 joy_state = 0;

//Ball Globals
U32 x_ball[3] = {40,95,150};
double ball_speed[10] ={0,0,0,0,0,0,0,0,0,0};
U32 ball_position_x[10]={0,0,0,0,0,0,0,0,0,0};
double ball_position_y[10]={320,320,320,320,320,320,320,320,320,320};
U32 num_balls = 0;
U32 max_balls = 7;
// Ball Cordination and Physics Constants
double accel = -250;
double speed_up = 315;

//Score
U32 score = 0;
unsigned char buffer[20];

//Create Bitmaps
unsigned short platform[] = 
              
      //Platform     
              {                 BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,P_COL,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,
};
//Platform Clear
unsigned short platform_clear[] = 
            {               
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                  BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,        
};

 //Easy Level Ball
unsigned short ball[] = {       BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,BG   ,BG   ,BG   ,BG   ,
                                BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,BG   ,BG   ,
                                BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,BG   ,
                                BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,BG   ,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,
                                BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,BG   ,
                                BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,BG   ,
                                BG   ,BG   ,P_COL,P_COL,P_COL,P_COL,P_COL,P_COL,BG   ,BG   ,
                                BG   ,BG   ,BG   ,BG   ,P_COL,P_COL,BG   ,BG   ,BG   ,BG   
                               
};
 //Hard Level Ball

unsigned short ball_hard[] = {  BG   ,BG   ,BG   ,BG   ,Red  ,Red  ,BG   ,BG   ,BG   ,BG   ,
                                BG   ,BG   ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,BG   ,BG   ,
                                BG   ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,BG   ,
                                BG   ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,BG   ,
                                Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,
                                Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,
                                BG   ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,BG   ,
                                BG   ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,BG   ,
                                BG   ,BG   ,Red  ,Red  ,Red  ,Red  ,Red  ,Red  ,BG   ,BG   ,
                                BG   ,BG   ,BG   ,BG   ,Red  ,Red  ,BG   ,BG   ,BG   ,BG   
                               
};
//Ball Clear
unsigned short ball_clear[] = {       
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG,
                                BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG   ,BG,  BG  ,BG
                               
};


/*******************************************************************************
* Initialization:                                                       *
*                   Set up of Timer and Perepherals
*                                                                      *
*******************************************************************************/
void init(){

  LED_setup();
  pushbutton_setup();
  init_scroll();
  GLCD_SetTextColor(White);
  joystick_setup();
  timer_setup();
}
/*******************************************************************************
* Task_1 player Bar:                                                       *
*                   Responsible for controlling the Player  
*                                                                      *
*******************************************************************************/
__task void juggler(void){
  os_mut_init (&mutex1);
  while(1){
    os_mut_wait (&mutex1, 0xffff);
    if (joystick_read() == JOY_DOWN && joy_state!=2){
    //Move platform to the right
      joy_state = 2;
    }
    else if (joystick_read() == JOY_UP && joy_state!=0){
    //Move platform to the left    
      joy_state = 0;
    }
    else if (joystick_read() != JOY_UP && joystick_read() != JOY_DOWN && joy_state!=1){
    //Move platform to the center  
        joy_state = 1;
    }
    os_mut_release (&mutex1);
  }
}
/*******************************************************************************
* Task_2 Ball Control and Game Related Features :                                  *
*                   Responsible for controlling the position and speed of the balls
                        and game related information such as LEDs for live count and 
                        keeps score.
*                                                                      *
*******************************************************************************/
__task void compute(){
  //Needed variables and initialization for the task
  U32 time_prev = timer_read();
  U32 time_curr = 0;
  double dt = 0;
  U32 countdown = 0;
  U32 button_delay = 0;
  U32 speed_range = 40;
  U32 score_increment = 1;
  srand(time_prev);
  sprintf(buffer,"Score: %d" , score);
  LED_display(lives[livIndex]);
  os_mut_init (&mutex2);
  
  while(1){
  //For loop variables
    U32 i = 0;
    U32 j=0;
  //Mutex waiting
    os_mut_wait (&mutex2, 0xffff);
  //Changing to hard mode
    if(pushbutton_read()==0 && button_delay==0){
      if(accel==-250){
        accel = -500;
        speed_up = 440;
        speed_range = 80;
        score_increment = 2;
      }
      else{
        accel = -250;
        speed_up = 315;
        speed_range = 40;
        score_increment = 1;
      }
      button_delay = 25000;
    }
   //Adding balls
    if (countdown==0 && num_balls!=max_balls){
       countdown = 200000;
       num_balls++;
       ball_position_x[num_balls-1] = rand()%3;

    }
    time_curr = timer_read();
    dt = (time_curr-time_prev)/1000000.0;
    //Updating balls
    for (i=0; i<num_balls; i++){
      ball_position_y[i] = ball_speed[i]*dt + 0.5*accel*dt*dt + ball_position_y[i];
      ball_speed[i]=accel*dt + ball_speed[i];

      if(ball_position_y[i]<=0){
         for (j=0; j<num_balls-i; j++){
           ball_position_y[i+j] = ball_position_y[i+j+1];
           ball_position_x[i+j] = ball_position_x[i+j+1];
           ball_speed[i+j] = ball_speed[i+j+1];
         }
         num_balls--;
         countdown = 200000;

         if(livIndex>0){
           livIndex--;
           LED_display(lives[livIndex]);
         }

      }
      if (ball_position_y[i]<=40 && ball_position_y[i]>25 && joy_state == ball_position_x[i]){
        ball_position_y[i] = 40;
        ball_speed[i] = rand()%speed_range+speed_up;
        score += score_increment;
        sprintf(buffer,"Score: %d" , score); 
      }
    }
    //Preparing for next time step
    time_prev = time_curr;
    if (countdown>0){
      
      countdown--;
    }
    if (button_delay>0){
      
      button_delay--;
    }
    os_mut_release (&mutex2);
  }
}

/*******************************************************************************
* Task_3 Displays Game on the LCD Screen :                                  *
*         Takes Information from Task 1 and Task 2 and displays it on the screen
*                                                                      *
*******************************************************************************/
__task void output(){
  U32 temp_ball_position_y[10] = {320,320,320,320,320,320,320,320,320,320};
  U32 temp_ball_position_x[10] = {0,0,0,0,0,0,0,0,0,0};
  U8 temp_joy_state = 1;
  unsigned char game_over[9] = {'G','A','M','E',' ','O','V','E','R'};
  unsigned char message[50] = {'T','H','A','N','K','S',' ','F','O','R',' ','P','L','A','Y','I','N','G','.',' ','P','R','E','S','S',' ','R','E','S','E','T',' ','T','O',' ','T','R','Y',' ','A','G','A','I','N','.'};
  GLCD_Bitmap(y_plat,x_plat[1],10,50,(unsigned char*)platform);
  while(1){
    U32 i=0;
    os_mut_wait (&mutex1, 0xffff);
    os_mut_wait (&mutex2, 0xffff);
    if(livIndex == 0){
      GLCD_DisplayString(4,5,1,game_over);
      GLCD_DisplayString(15,8,0,message);
    }
    else{
      if(accel==-250){
        GLCD_Bitmap(temp_ball_position_y[num_balls],x_ball[temp_ball_position_x[num_balls]],10,10,(unsigned char*)ball_clear);
        for (i=0; i<num_balls; i++){
          GLCD_Bitmap(temp_ball_position_y[i],x_ball[temp_ball_position_x[i]],10,10,(unsigned char*)ball_clear);
          temp_ball_position_y[i]=(unsigned int)ball_position_y[i];
          temp_ball_position_x[i]=ball_position_x[i];
          GLCD_Bitmap(temp_ball_position_y[i],x_ball[temp_ball_position_x[i]],10,10,(unsigned char*)ball);
        }
        
      }
      else{
        GLCD_Bitmap(temp_ball_position_y[num_balls],x_ball[temp_ball_position_x[num_balls]],10,10,(unsigned char*)ball_clear);
        for (i=0; i<num_balls; i++){
          GLCD_Bitmap(temp_ball_position_y[i],x_ball[temp_ball_position_x[i]],10,10,(unsigned char*)ball_clear);
          temp_ball_position_y[i]=(unsigned int)ball_position_y[i];
          temp_ball_position_x[i]=ball_position_x[i];
          GLCD_Bitmap(temp_ball_position_y[i],x_ball[temp_ball_position_x[i]],10,10,(unsigned char*)ball_hard);
        }
      }
      
      if (joy_state == 2 && temp_joy_state!=2){
        temp_joy_state = 2;
        GLCD_Bitmap(y_plat,x_plat[1],10,50,(unsigned char*)platform_clear);
        GLCD_Bitmap(y_plat,x_plat[2],10,50,(unsigned char*)platform);
      }
      else if (joy_state == 0 && temp_joy_state!=0){
        temp_joy_state = 0;
        GLCD_Bitmap(y_plat,x_plat[1],10,50,(unsigned char*)platform_clear);
        GLCD_Bitmap(y_plat,x_plat[0],10,50,(unsigned char*)platform);
      }
      else if (joy_state == 1 && temp_joy_state!=1){
        temp_joy_state = 1;
        GLCD_Bitmap(y_plat,x_plat[2],10,50,(unsigned char*)platform_clear);
      GLCD_Bitmap(y_plat,x_plat[0],10,50,(unsigned char*)platform_clear);
        GLCD_Bitmap(y_plat,x_plat[1],10,50,(unsigned char*)platform);
      }
      
      GLCD_DisplayString(8,9,1,buffer);
    }
    os_mut_release (&mutex1);
    os_mut_release (&mutex2);
  }
}
/*******************************************************************************
* Initialization                                                      *
*         Initializes each task, setting ID numbers, then terminates itself 
*                                                                      *
*******************************************************************************/
__task void init_tasks(){
  player = os_tsk_create (juggler, 1);
  balls = os_tsk_create (compute, 1);
  lcd = os_tsk_create (output, 1);
  os_tsk_delete_self();
}

/*******************************************************************************
* Main:                                                       *
*             Initializes system by calling init and starts tasks  
*                                                                      *
*******************************************************************************/
int main(){
    
  init();
  printf("main");
  os_sys_init (init_tasks);
  while(1);
}


