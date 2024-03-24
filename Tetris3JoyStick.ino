//*************************************************************************************************
// Tetris3Joy.ino
//
//  Modified for Joystick Controls
//  by RCI
//
//  3-23-2024
//*************************************************************************************************
//#include <MsTimer2.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <esp_now.h>
#include "music.h"
#include "logo.h"
#include "Font3x5FixedNumMic.h"
#include "Font4x5Fixed.h"
#include "Font5x5Fixed.h"
#include "Font5x7FixedMono.h"
#include <LittleFS.h>
/*--------------------- MATRIX PANEL CONFIG -------------------------*/
#define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 64  // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1   // Total number of panels chained one to another

//**********************************************
//   WEMOS D1 MINI 32 Daughterboard PINOUTS
//**********************************************
//        VAR   IO PIN   BOARD LABEL    COLOR        DIN
#define R1_PIN  33    // IO33           BROWN 1       1        
#define G1_PIN  26    // IO26-D0        RED 1         2
#define B1_PIN  27    // IO27           ORANGE 1      3
                      // GND            YELLOW 1      4
#define R2_PIN  0     // IO0            GREEN 1       5
#define G2_PIN  18    // IO18-D5-SCK    BLUE 1        6
#define B2_PIN  19    // IO19-D6-MISO   PURPLE        7
#define E_PIN   32    // IO32           GREY          8
#define A_PIN   23    // IO23-D7-MOSI   WHITE         9   
#define B_PIN   12    // IO12-TDI       BLACK         10
#define C_PIN   5     // IO5-D8-CS      BROWN 2       11
#define D_PIN   17    // IO17           RED 2         12         
#define CLK_PIN 16    // IO16           ORANGE 2      13
#define LAT_PIN 4     // IO4            YELLOW 2      14
#define OE_PIN  25    // IO25           GREEN 2       15
                      //                BLUE 2 SPARE  16
#define UP_PB   21    // Joystick Pin 1
#define DN_PB   14    // Joystick Pin 2
#define LT_PB   13    // Joystick Pin 3
#define RT_PB   15    // Joystick Pin 4
#define TRIGG   22    // Joystick PB Pin 6

//#include "esp32fota.h"
#define VERSION 17
//esp32FOTA esp32FOTA("tetris", VERSION, false, true);
#define FOTAURL "https://MYSERVER/fotatetris.json"

//const char *ssid = "MYWIFI";
//const char *password = "MYPASSWORD";

// Replace with your network credentials
//const char *ssid = "RCINet2.4";
//const char *password = "cae4844bb2";

// Replace with your network credentials
const char *ssid = "josephine";
const char *password = "drain_8490follow";

#define B1 19
#define B2 18
#define MOVE NOTE_C7
#define DROP NOTE_C4
#define PREBEEPREST 30
#define BEEPDURATION 100
#define POSTBEEPREST 30
#define BEEPOFF 0
#define BEEPREST 1
#define BEEPING 2
#define BEEPPOST 3


#define BUZZER_PIN1 56
#define BUZZER_PIN2 57

#define BUZZER_CHANNEL1 0
#define BUZZER_CHANNEL2 2

TaskHandle_t music;
unsigned long int lastbeepon;
bool beepon;
bool shouldresume;
int beepnote;
bool musicset;
bool audioset;
bool playnow;
short beepstatus;

MatrixPanel_I2S_DMA *display = nullptr;
//MatrixPanel_I2S_DMA display = MatrixPanel_I2S_DMA();

uint16_t myDARK = display->color565(192, 192, 0);
uint16_t myWHITE = display->color565(192, 192, 192);
uint16_t myRED = display->color565(255, 0, 0);
uint16_t myGREEN = display->color565(0, 255, 0);
uint16_t myBLUE = display->color565(0, 0, 255);

uint16_t colours[5] = { myDARK, myWHITE, myRED, myGREEN, myBLUE };

#define MATRIXLED_Y_COUNT 64
#define MATRIXLED_X_COUNT 64
#define MATRIXLED_COLOR_COUNT 3

#define BLOCK_SIZE 3
#define TETRIS_WINDOW_SIZE_X 10
#define TETRIS_WINDOW_SIZE_Y 20
#define TETRIS_WINDOW_POSITION_X ((MATRIXLED_X_COUNT - (TETRIS_WINDOW_SIZE_X * BLOCK_SIZE)) / 2)  // == (64 - ( 10 * 3 )) / 2 == 17
#define TETRIS_WINDOW_POSITION_Y ((MATRIXLED_Y_COUNT - (TETRIS_WINDOW_SIZE_Y * BLOCK_SIZE)) - 2)  // == (64 - ( 20 * 3) ) - 2 == 2
#define TETRIS_BOARD_SIZE_X (TETRIS_WINDOW_SIZE_X + 2)                                            // == 10 + 2 == 12
#define TETRIS_BOARD_SIZE_Y (TETRIS_WINDOW_SIZE_Y + 2)                                            // == 20 + 2 == 22

#define TETRIMINO_WINDOW_POSITION_X (TETRIS_WINDOW_POSITION_X + (TETRIS_WINDOW_SIZE_X * BLOCK_SIZE) + 3)
#define TETRIMINO_WINDOW_POSITION_Y TETRIS_WINDOW_POSITION_Y

#define TETRIMINO_SIZE 4
#define NEXT_TETRIMINO_LIST_COUNT 5

#define DAS 167
#define ARR 33

#define BLACK 0
#define LIGHT_BLUE 1
#define YELLOW 2
#define GREEN 3
#define RED 4
#define BLUE 5
#define ORANGE 6
#define PURPLE 7
#define TETRIMINO_COLOR_COUNT 8

#define WALL 255

#define MASTER 0
#define SLAVE 1

#define ONE_PLAYER 0
#define TWO_PLAYERS 1

#define GAME_OVER 0
#define YOU_WIN 1
#define YOU_LOSE 2

#define PAIRING 0
#define HEIGHT 1
#define ADDING 2
#define LOST 3

#define LEFT 3
#define RIGHT 2
#define DOWN 4
#define UP 1
#define TRIGGER 0

#define NBUTTONS 5

//uint8_t buttonPins[] = { 33, 0, 2, 34, 35 };
//                      TRIGGER   ROT RT   RT     LT   ROT LT
uint8_t buttonPins[] = {TRIGG, UP_PB, RT_PB, LT_PB, DN_PB};

int readings[NBUTTONS];
int buttonStates[NBUTTONS];
bool justPressed[NBUTTONS] = { false, false, false, false, false };
bool isPressed[NBUTTONS] = { false, false, false, false, false };
int64_t lastPressed[NBUTTONS] = { 0, 0, 0, 0, 0 };
int64_t lastReleased[NBUTTONS] = { 0, 0, 0, 0, 0 };
int64_t lastRepeated[NBUTTONS] = { 0, 0, 0, 0, 0 };
int lastButtonStates[] = { LOW, LOW, LOW, LOW, LOW };
uint64_t lastDebounceTimes[] = { 0, 0, 0, 0, 0 };
unsigned long debounceDelay = 20;  // the debounce time; increase if the output flickers
bool autorepeat;
unsigned long lastMillisDown = 0;

unsigned short role, party;
unsigned short mode;
bool paired;
bool game_over;
unsigned int randomseed;
unsigned short myheight, partyheight;
unsigned short linestoadd;
unsigned short hole;

uint8_t macs[][6] = {
  { 0xEC, 0x94, 0xCB, 0x6B, 0xDB, 0x50 },  // This is the Master
  { 0xEC, 0x94, 0xCB, 0x6B, 0xDB, 0xAC }   // This is the Slave
};

typedef struct struct_message {
  unsigned short type;
  unsigned short data;
} struct_message;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message outgoingmsg;

// Create a struct_message to hold incoming sensor readings
struct_message incomingmsg;

esp_now_peer_info_t peerInfo;

unsigned int level, rows, score;
// gameboy tetris delays by level
unsigned int delays[] = { 887, 820, 753, 686, 619, 552, 468, 368, 284, 184, 167, 150, 133, 117, 100, 100, 83, 83, 66, 66, 50 };

const unsigned char Tetrimino_Color[TETRIMINO_COLOR_COUNT][BLOCK_SIZE][BLOCK_SIZE][MATRIXLED_COLOR_COUNT] = {
  { { { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 } },
    { { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 } },
    { { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 } } },
  { { { 0xDA, 0xFF, 0xFF }, { 0xA4, 0xF9, 0xF9 }, { 0x6E, 0xEE, 0xEE } },
    { { 0xA4, 0xF9, 0xF9 }, { 0x70, 0xEF, 0xEF }, { 0x3A, 0xE4, 0xE4 } },
    { { 0x70, 0xEF, 0xEF }, { 0x3A, 0xE4, 0xE4 }, { 0x05, 0xDA, 0xDA } } },
  { { { 0xFF, 0xFF, 0xDD }, { 0xFF, 0xF1, 0xAA }, { 0xF8, 0xE3, 0x78 } },
    { { 0xFF, 0xF1, 0xAA }, { 0xF8, 0xE3, 0x79 }, { 0xF1, 0xD5, 0x47 } },
    { { 0xF8, 0xE3, 0x79 }, { 0xF1, 0xD5, 0x47 }, { 0xE9, 0xC6, 0x14 } } },
  { { { 0xD6, 0xFF, 0xDB }, { 0x9D, 0xF2, 0xA7 }, { 0x64, 0xE4, 0x73 } },
    { { 0x9D, 0xF2, 0xA7 }, { 0x65, 0xE4, 0x74 }, { 0x2D, 0xD7, 0x41 } },
    { { 0x65, 0xE4, 0x74 }, { 0x2D, 0xD7, 0x41 }, { 0x00, 0xC9, 0x0D } } },
  { { { 0xF6, 0xCC, 0xCC }, { 0xDD, 0x88, 0x88 }, { 0xC5, 0x45, 0x45 } },
    { { 0xDD, 0x88, 0x88 }, { 0xC5, 0x46, 0x46 }, { 0xAD, 0x03, 0x03 } },
    { { 0xC5, 0x46, 0x46 }, { 0xAD, 0x03, 0x03 }, { 0x94, 0x00, 0x00 } } },
  { { { 0xC8, 0xCE, 0xF2 }, { 0x82, 0x8F, 0xD7 }, { 0x3B, 0x4E, 0xBB } },
    { { 0x82, 0x8F, 0xD7 }, { 0x3D, 0x50, 0xBC }, { 0x00, 0x0F, 0xA0 } },
    { { 0x3D, 0x50, 0xBC }, { 0x00, 0x0F, 0xA0 }, { 0x00, 0x00, 0x84 } } },
  { { { 0xFE, 0xE5, 0xD4 }, { 0xED, 0xBA, 0x98 }, { 0xDD, 0x90, 0x5D } },
    { { 0xED, 0xBA, 0x98 }, { 0xDD, 0x91, 0x5E }, { 0xCE, 0x68, 0x24 } },
    { { 0xDD, 0x91, 0x5E }, { 0xCE, 0x68, 0x24 }, { 0xBD, 0x3D, 0x00 } } },
  { { { 0xE9, 0xCC, 0xF6 }, { 0xC4, 0x8A, 0xDF }, { 0x9E, 0x47, 0xC7 } },
    { { 0xC4, 0x8A, 0xDF }, { 0xA0, 0x49, 0xC8 }, { 0x7A, 0x06, 0xB0 } },
    { { 0xA0, 0x49, 0xC8 }, { 0x7A, 0x06, 0xB0 }, { 0x54, 0x00, 0x98 } } }
};

const unsigned char Tetrimino_Shape[TETRIMINO_COLOR_COUNT][4][TETRIMINO_SIZE][2] = {
  //dummy
  {
    0 },

  //i_shape
  {
    { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } },
    { { 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 } },
    { { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 } },
    { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 } } },

  //o_shape
  {
    { { 1, 1 }, { 2, 1 }, { 1, 2 }, { 2, 2 } },
    { { 1, 1 }, { 2, 1 }, { 1, 2 }, { 2, 2 } },
    { { 1, 1 }, { 2, 1 }, { 1, 2 }, { 2, 2 } },
    { { 1, 1 }, { 2, 1 }, { 1, 2 }, { 2, 2 } } },

  //s_shape
  {
    { { 1, 0 }, { 2, 0 }, { 0, 1 }, { 1, 1 } },
    { { 1, 0 }, { 1, 1 }, { 2, 1 }, { 2, 2 } },
    { { 1, 1 }, { 2, 1 }, { 0, 2 }, { 1, 2 } },
    { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 2 } } },

  //z_shape
  {
    { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 2, 1 } },
    { { 2, 0 }, { 1, 1 }, { 2, 1 }, { 1, 2 } },
    { { 0, 1 }, { 1, 1 }, { 1, 2 }, { 2, 2 } },
    { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 0, 2 } } },

  //j_shape
  {
    { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },
    { { 1, 0 }, { 2, 0 }, { 1, 1 }, { 1, 2 } },
    { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 2, 2 } },
    { { 1, 0 }, { 1, 1 }, { 0, 2 }, { 1, 2 } } },

  //l_shape
  {
    { { 2, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },
    { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 2, 2 } },
    { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 0, 2 } },
    { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 1, 2 } } },

  //t_shape
  {
    { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },
    { { 1, 0 }, { 1, 1 }, { 2, 1 }, { 1, 2 } },
    { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 1, 2 } },
    { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 1, 2 } } }
};

unsigned char Tetris_Board[TETRIS_BOARD_SIZE_Y][TETRIS_BOARD_SIZE_X] = { 0 };
unsigned char Next_Tetrimino_List[NEXT_TETRIMINO_LIST_COUNT] = { 0 };

unsigned char Pos_Y;
//*************************************************************************************************
void send_command(unsigned char draw_start_pixel_y, unsigned char draw_end_pixel_y, unsigned char draw_start_pixel_x, unsigned char draw_end_pixel_x) 
{
  if ((draw_start_pixel_y < MATRIXLED_Y_COUNT) && (draw_end_pixel_y < MATRIXLED_Y_COUNT) && (draw_start_pixel_y <= draw_end_pixel_y)) 
    {
    if ((draw_start_pixel_x < MATRIXLED_X_COUNT) && (draw_end_pixel_x < MATRIXLED_X_COUNT) && (((draw_start_pixel_x <= draw_end_pixel_x) && (draw_start_pixel_y == draw_end_pixel_y)) || (draw_start_pixel_y < draw_end_pixel_y))) 
      {
      display->fillRect(draw_start_pixel_x, draw_start_pixel_y, draw_end_pixel_x - draw_start_pixel_x, draw_end_pixel_y - draw_start_pixel_y, 0);
      }
    }
}
//*************************************************************************************************
unsigned short computeXscore(unsigned int toplace) 
{
  if (toplace >= 1000)
    return 0;
  if (toplace >= 100)
    return 4;
  if (toplace >= 10)
    return 8;
  return 12;
}
//******************************************************************************************
void draw_score() 
{
  display->setFont(&Font3x5FixedNumMic);  // choose font
  display->setTextSize(1);
  display->fillRect(0, 7, 15, 5, 0x0000);
  display->fillRect(0, 20, 15, 5, 0x0000);
  display->fillRect(0, 33, 15, 5, 0x0000);
  display->setCursor(computeXscore(level), 12);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println(level);
  display->setCursor(computeXscore(rows), 25);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println(rows);
  display->setCursor(computeXscore(score), 38);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println(score);
  display->setFont();  // return to the system font
}
//******************************************************************************************
void beep(int sound) 
{
  if (audioset) 
    {
    beepon = true;
    lastbeepon = millis();
    beepnote = sound;
    }
}   
//******************************************************************************************
void read_buttons() 
{
  // 
  if(digitalRead(RT_PB) == 0)
    {
    buttonPins[2] = 0; 
    readings[2] = buttonPins[2]; 
    delay(80);
    //while(digitalRead(RT_PB) == 0){}
    justPressed[RIGHT] = true;    
    }
  else
    {
    buttonPins[2] = true;
    readings[2] = buttonPins[2];
    justPressed[RIGHT] = false; 
    }
  if(digitalRead(LT_PB) == 0)
    {
    buttonPins[3] = 0;
    readings[3] = buttonPins[3]; 
    delay(80); 
    //while(digitalRead(LT_PB) == 0){}
    justPressed[LEFT] = true;
    }
  else
    {
    buttonPins[3] = true;
    readings[3] = buttonPins[3]; 
    justPressed[LEFT] = false;
    }    
  if(digitalRead(UP_PB) == 0)
    {
    buttonPins[1] = 0; 
    readings[1] = buttonPins[1]; 
    while(digitalRead(UP_PB) == 0){}
    justPressed[UP] = true;
    }
  else
    {
    buttonPins[1] = true;
    readings[1] = buttonPins[1]; 
    justPressed[UP] = false;
    }
  if(digitalRead(DN_PB) == 0)
    {
    buttonPins[4] = 0; 
    readings[4] = buttonPins[4]; 
    while(digitalRead(DN_PB) == 0){}
    justPressed[DOWN] = true;
    }
  else
    {
    buttonPins[4] = true;
    readings[4] = buttonPins[4]; 
    justPressed[DOWN] = false;
    }  
  if(digitalRead(TRIGG) == 0)
    {
    delay(70);
    buttonPins[0] = 0; 
    readings[0] = buttonPins[0];    
    justPressed[TRIGGER] = true;
    }
  else
    {
    buttonPins[0] = true;
    readings[0] = buttonPins[0]; 
    justPressed[TRIGGER] = false;
    }
 
  for (uint8_t i = 0; i < NBUTTONS; i++) 
    {
    lastButtonStates[i] = readings[i];
    }
    
}// End read_buttons
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
void print_board() 
{
  return;
  Serial.println("------------");
  for (short y = 1; y < TETRIS_BOARD_SIZE_Y - 1; y++) {
    Serial.print("|");
    for (short x = 1; x < TETRIS_BOARD_SIZE_X - 1; x++) {
      if (Tetris_Board[y][x] == BLACK)
        Serial.print(" ");
      else
        Serial.print("X");
    }
    Serial.println("|");
  }
  Serial.println("------------");
}
//*************************************************************************************************
bool is_pressed(uint8_t button) 
{
  return (buttonStates[button] == LOW);
}
//*************************************************************************************************
uint64_t last_moved = 0;
/*
  void buzzer_play_melody(int melody[], int durations[], int note_count, bool toflush) {
  buzzer1.playMelody(melody, durations, note_count);
  if (toflush)
    while (buzzer1.getState() != BUZZER_IDLE) buzzer1.loop();
  //buzzer2.playMelody(melody, durations, note_count);
  //if (toflush)
  //  while (buzzer2.getState() != BUZZER_IDLE) buzzer2.loop();
  }
*/
//*************************************************************************************************
void set_role() 
{
  Serial.println(WiFi.macAddress());
  if (WiFi.macAddress() == "EC:94:CB:6B:DB:50") 
    {
    Serial.println("Master");
    role = MASTER;
    party = SLAVE;
    } 
  else 
    {
    Serial.println("Slave");
    role = SLAVE;
    party = MASTER;
    }
}
//*************************************************************************************************
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  //Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    Serial.println("Delivery Success :)");
  } else {
    //Serial.println("Delivery FAIL :(");
    Serial.print(".");
    Serial.flush();
    esp_now_send(macs[party], (uint8_t *)&outgoingmsg, sizeof(outgoingmsg));
  }
}
//*************************************************************************************************
void draw_game_over(short outcome);

//*************************************************************************************************
// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *msg, int len) 
{
  memcpy(&incomingmsg, msg, sizeof(incomingmsg));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  /*
    incomingTemp = incomingReadings.temp;
    incomingHum = incomingReadings.hum;
    incomingPres = incomingReadings.pres;
  */
  Serial.println(incomingmsg.type);
  Serial.println(incomingmsg.data);
  switch (incomingmsg.type) {
    case PAIRING:
      Serial.println("RECEIVED PAIRING");

      if (incomingmsg.data > randomseed)
        randomseed = incomingmsg.data;
      paired = true;
      break;
    case HEIGHT:
      Serial.println("RECEIVED HEIGHT");

      if (paired) {
        partyheight = incomingmsg.data;
        display->drawLine(TETRIMINO_WINDOW_POSITION_X - 2, 62, TETRIMINO_WINDOW_POSITION_X - 2, 2, 0x0000);
        if (partyheight) {
          Serial.println("partyheight: " + String(partyheight));
          display->drawLine(TETRIMINO_WINDOW_POSITION_X - 2, 62, TETRIMINO_WINDOW_POSITION_X - 2, 62 - 3 * (partyheight), myRED);
        }
      }
      break;
    case ADDING:
      Serial.println("RECEIVED ADDING");
      if (paired) {
        linestoadd = incomingmsg.data;
        Serial.println("set linestoadd");
      }
      break;
    case LOST:
      Serial.println("RECEIVED LOST");
      if (paired) {
        draw_game_over(YOU_WIN);
        restart();
      }
      break;
  }
  return;
}
//*************************************************************************************************
void draw_game_over(short outcome) 
{
  game_over = true;
  display->fillRect(0, 0, 64, 64, 0x0000);
  switch (outcome) {
    case GAME_OVER:
      draw_text_fill_back(4, 16, "GAME OVER", 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 1);
      break;
    case YOU_WIN:
      draw_text_fill_back(10, 16, "YOU WIN!", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
      break;
    case YOU_LOSE:
      draw_text_fill_back(7, 16, "YOU LOSE!", 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 1);
      break;
  }
  stop_audio();
  delay(5000);
  return;
}
//*************************************************************************************************
void game_start() 
{
  Serial.println("  inside game start");
  game_over = false;
  linestoadd = 0;
  display->fillRect(0, 0, 64, 64, 0x0000);
  for (int k = 9; k >= 0; k--) 
    {
    display->fillRect(0, k + 1, 64, 44, 0x0000);
    
    for (int i = k; i < 44 + k; i++) 
      {
      for (int j = 0; j < 64; j++) 
        {
        if (logo[(i - k) * 64 + j] != 65535)
          display->drawPixel(j, i, logo[(i - k) * 64 + j]);
        }
      }
    delay(33);
    }
  draw_text(9, 35, "*", 0xff, 0xff, 0xff, 1);
  draw_text(9, 45, "1 Player", 0xff, 0xff, 0xff, 1);
  draw_text(9, 55, "2 Players", 0xff, 0xff, 0xff, 1);
  display->fillTriangle(2, 51, 2, 45, 7, 48, 0xffff);
  uint8_t selection = 1;
  Serial.println("waited");
  
  while (true) 
  {
    read_buttons();
    if (justPressed[TRIGGER]) 
      {
      break;
      }
    if (justPressed[DOWN]) 
      {
      switch (selection) 
        {
        case 0:
          display->fillTriangle(2, 41, 2, 35, 7, 38, 0x0000);
          display->fillTriangle(2, 51, 2, 45, 7, 48, 0xffff);
          selection = 1;
          break;
        case 1:
          display->fillTriangle(2, 51, 2, 45, 7, 48, 0x0000);
          display->fillTriangle(2, 61, 2, 55, 7, 58, 0xffff);
          selection = 2;
          break;
        }
      }
    if (justPressed[UP]) 
      {
      switch (selection) 
        {
        case 2:
          display->fillTriangle(2, 61, 2, 55, 7, 58, 0x0000);
          display->fillTriangle(2, 51, 2, 45, 7, 48, 0xffff);
          selection = 1;
          break;
        case 1:
          display->fillTriangle(2, 51, 2, 45, 7, 48, 0x0000);
          display->fillTriangle(2, 41, 2, 35, 7, 38, 0xffff);
          selection = 0;
          break;
        }
      }
  }// Ed While True
  //******************************************
  randomseed = millis() % 1024;
  switch (selection) 
    {
    case 0:
      {
      short settingselection = 0;
      display->fillRect(0, 0, 64, 64, 0x0000);
      display->fillTriangle(2, 14, 2, 8, 7, 11, 0xffff);
      draw_text_fill_back(9, 8, "Music:", 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 1);
      if (musicset) 
        {
        draw_text_fill_back(40, 18, "On", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
        } 
      else 
        {
        draw_text_fill_back(40, 18, "Off", 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 1);
        }
        draw_text_fill_back(9, 28, "Audio:", 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 1);
      if (audioset) 
        {
        draw_text_fill_back(40, 38, "On", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
        } 
      else 
        {
        draw_text_fill_back(40, 38, "Off", 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 1);
        }
        draw_text_fill_back(9, 48, "SAVE", 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 1);
        
        // **************Loop Here ***********************************
        while (true) 
          {
          read_buttons();
          if (justPressed[TRIGGER]) 
            {
            switch (settingselection) 
              {
              case 0:
                {
                musicset = !musicset;
                draw_text_fill_back(40, 18, "   ", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
                if (musicset) 
                  {
                  draw_text_fill_back(40, 18, "On", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
                  init_audio();
                  } 
                else 
                  {
                  draw_text_fill_back(40, 18, "Off", 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 1);
                  stop_audio();
                  }
                }
                break;
              case 1:
                {
                audioset = !audioset;
                draw_text_fill_back(40, 38, "   ", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
                if (audioset) 
                  {
                  draw_text_fill_back(40, 38, "On", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
                  } 
                else 
                  {
                  draw_text_fill_back(40, 38, "Off", 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 1);
                  }
                }
                break;
              case 2:
                {
                write_settings();
                restart();
                return;
                }
                break;
              }
            }
          if (justPressed[DOWN]) 
            {
            switch (settingselection)
              {
              case 0:
                {
                settingselection = 1;
                display->fillTriangle(2, 14, 2, 8, 7, 11, 0x0000);
                display->fillTriangle(2, 34, 2, 28, 7, 31, 0xffff);
                }
                break;
              case 1:
                {
                settingselection = 2;
                display->fillTriangle(2, 34, 2, 28, 7, 31, 0x0000);
                display->fillTriangle(2, 54, 2, 48, 7, 51, 0xffff);
                }
                break;
              }
            }
          if (justPressed[UP]) 
            {
            switch (settingselection) 
              {
              case 2:
                {
                settingselection = 1;
                display->fillTriangle(2, 34, 2, 28, 7, 31, 0xffff);
                display->fillTriangle(2, 54, 2, 48, 7, 51, 0x0000);
                }
                break;
              case 1:
                {
                settingselection = 0;
                display->fillTriangle(2, 14, 2, 8, 7, 11, 0xffff);
                display->fillTriangle(2, 34, 2, 28, 7, 31, 0x0000);
                }
                break;
              }
            }
          }
        }
      break;
    case 1:
      {
      mode = ONE_PLAYER;
      Serial.println("One player");
      }
      break;
    case 2:
      {
      mode = TWO_PLAYERS;
      Serial.println("Two players");
      display->fillRect(0, 45, 64, 25, 0x0000);
      draw_text(9, 35, "*", 0xff, 0xff, 0xff, 1);

      draw_text(3, 50, "Connecting", 0xff, 0xff, 0xff, 1);
      init_connection();
      display->fillRect(0, 45, 64, 25, 0x0000);
      draw_text(3, 50, "Connected!", 0xff, 0xff, 0xff, 1);
      hole = 1 + random() % TETRIS_WINDOW_SIZE_X;
      delay(3000);
      }
      break;
    }
  display->clearScreen();
  randomSeed(randomseed);
  init_board();
  init_score();
  init_next_tetrimino_board();
  get_next_tetrimino();
  delay(500);
  justPressed[TRIGGER] = isPressed[TRIGGER] = lastPressed[TRIGGER] = lastRepeated[TRIGGER] = 0;
  //autorepeat = true;
  playnow = true;
  if (musicset) 
    {
    ledcWriteTone(BUZZER_CHANNEL1, NOTE_E5);
    ledcWriteTone(BUZZER_CHANNEL2, NOTE_E2);
    last_rh = last_lh = millis();
    rh_pos = 0;
    lh_pos = 0;
    }
  Pos_Y = 0;
  Serial.println("exiting game start");
  return;
}
//*******************************************************************************************
void init_connection() 
{
  WiFi.mode(WIFI_STA);
  //If the initialization was successful
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  //If there was an error
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, macs[party], 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
  paired = false;
  outgoingmsg.type = PAIRING;
  outgoingmsg.data = randomseed;

  esp_err_t result = esp_now_send(macs[party], (uint8_t *)&outgoingmsg, sizeof(outgoingmsg));

  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  } else {
    //Serial.println("Error sending the data");
  }
  while (!paired)
    yield();
  Serial.println("Paired, randomseed=" + String(randomseed));
  return;
}
//*******************************************************************************************
void send_height() 
{
  short h;
  if (mode != TWO_PLAYERS)
    return;

  unsigned char loop_x, loop_y, loop_block_x, loop_block_y;
  outgoingmsg.type = HEIGHT;

  for (loop_y = 0; loop_y < (TETRIS_BOARD_SIZE_Y - 1); loop_y++) {
    for (loop_x = 1; loop_x < (TETRIS_BOARD_SIZE_X - 1); loop_x++) {
      if (Tetris_Board[loop_y][loop_x] != BLACK) {
        //Serial.println("height: " + String(TETRIS_BOARD_SIZE_Y - loop_y - 1));
        outgoingmsg.data = TETRIS_BOARD_SIZE_Y - loop_y - 1;
        esp_err_t result = esp_now_send(macs[party], (uint8_t *)&outgoingmsg, sizeof(outgoingmsg));
        return;
      }
    }
  }
  //Serial.println("height: 0");
  outgoingmsg.data = 0;
  esp_err_t result = esp_now_send(macs[party], (uint8_t *)&outgoingmsg, sizeof(outgoingmsg));
  return;
}
//*******************************************************************************************
void add_lines() 
{
  unsigned short h = TETRIS_WINDOW_SIZE_Y;
  unsigned char loop_x, loop_y;
  bool shall_continue = true;
  for (loop_y = 1; (loop_y <= TETRIS_WINDOW_SIZE_Y) && shall_continue; loop_y++) {
    for (loop_x = 1; (loop_x <= TETRIS_WINDOW_SIZE_X) && shall_continue; loop_x++) {
      if (Tetris_Board[loop_y][loop_x] != BLACK) {
        h = loop_y;
        shall_continue = false;
      }
    }
  }
  Serial.println("Adding lines: " + String(linestoadd) + " lines and " + String(h) + " position");
  Serial.println("TETRISWINDOWSIZE: " + String(TETRIS_WINDOW_SIZE_Y) + " -h+1=  " + String((TETRIS_WINDOW_SIZE_Y - h + 1) + linestoadd) + " ");
  if ((TETRIS_WINDOW_SIZE_Y - h + 1) + linestoadd > TETRIS_WINDOW_SIZE_Y) {
    display->fillRect(TETRIS_WINDOW_POSITION_Y, TETRIS_WINDOW_POSITION_Y,
                     TETRIS_WINDOW_SIZE_X * BLOCK_SIZE, TETRIS_WINDOW_SIZE_Y * BLOCK_SIZE, myRED);
    Serial.println("sending LOST");
    outgoingmsg.type = LOST;
    outgoingmsg.data = 0;
    esp_now_send(macs[party], (uint8_t *)&outgoingmsg, sizeof(outgoingmsg));
    delay(1000);
    draw_game_over(YOU_LOSE);
    restart();
    return;
  }
  Serial.println("copio su");
  Serial.println(h);
  Serial.println(linestoadd);
  Serial.println(h - linestoadd);

  for (loop_y = h - linestoadd; loop_y <= h - linestoadd + (TETRIS_WINDOW_SIZE_Y - h); loop_y++) {
    for (loop_x = 1; loop_x <= TETRIS_WINDOW_SIZE_X; loop_x++) {
      Serial.println(String(loop_y + linestoadd) + " " + String(loop_x) + " ");
      Tetris_Board[loop_y][loop_x] = Tetris_Board[loop_y + linestoadd][loop_x];
    }
  }

  Serial.println("aggiungo linee");
  for (loop_y = TETRIS_WINDOW_SIZE_Y - linestoadd + 1; loop_y <= TETRIS_WINDOW_SIZE_Y; loop_y++) {
    Serial.println(loop_y);
    for (loop_x = 1; loop_x <= TETRIS_WINDOW_SIZE_X; loop_x++) {
      if (loop_x != hole) {
        Tetris_Board[loop_y][loop_x] = PURPLE;
      } else {
        Tetris_Board[loop_y][loop_x] = BLACK;
      }
    }
  }

  Serial.println("finito con add lines");
  linestoadd = 0;
  //needs_redraw = true;
  return;
}
//*******************************************************************************************
void send_lines(short lines) 
{
  if (mode != TWO_PLAYERS)
    return;
  if (lines < 2)
    return;
  outgoingmsg.type = ADDING;
  if (lines < 4)
    outgoingmsg.data = lines - 1;
  else
    outgoingmsg.data = 4;

  esp_err_t result = esp_now_send(macs[party], (uint8_t *)&outgoingmsg, sizeof(outgoingmsg));
  return;
}

//*******************************************************************************************
void wifi_disconnect() 
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}
//*******************************************************************************************
void wifi_connect() 
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

/*
bool ota_update(short wifiattempts) {
  char ver[10];
  //esp32FOTA.checkURL = FOTAURL;
  esp32FOTA.setManifestURL(FOTAURL);
  short found = 0;
  int attempts = 0;
  display->fillRect(0, 0, 64, 64, 0x0000);
  sprintf(ver, "VER: %d", VERSION);
  draw_text_fill_back(4, 16, ver, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
  draw_text_fill_back(4, 26, "Connecting", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);

  while (WiFi.status() != WL_CONNECTED && attempts < wifiattempts) {
    delay(500);
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    draw_text_fill_back(4, 36, "Connected", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);

    bool updatedNeeded = esp32FOTA.execHTTPcheck();
    if (updatedNeeded) {
      draw_text_fill_back(4, 46, "Updating", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);

      esp32FOTA.execOTA();
    } else {
      draw_text_fill_back(4, 46, "No update", 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 1);
    }
    return true;
  } else {
  }
  return false;
}
*/
//*************************************************************************************************
void playaudio(void *parameter) 
{
  for (;;) {
    unsigned long int now = millis();
    if (playnow && musicset) {
      if (now - last_rh >= rh[rh_pos + 1]) {
        rh_pos = rh_pos + 2;
        if (rh_pos >= notes_rh)
          rh_pos = 0;
        ledcWriteTone(BUZZER_CHANNEL1, rh[rh_pos]);
        last_rh = now;
      }

      if (now - last_lh >= lh[lh_pos + 1]) {
        lh_pos = lh_pos + 2;
        if (lh_pos >= notes_lh)
          lh_pos = 0;
        if (!beepon) {
          ledcWriteTone(BUZZER_CHANNEL2, lh[lh_pos]);
        }
        last_lh = now;
      } else {
        if (shouldresume) {
          ledcWriteTone(BUZZER_CHANNEL2, lh[lh_pos]);
          shouldresume = false;
        }
      }
    }  // end of music
    if (beepon) {
      if (now - lastbeepon < PREBEEPREST) {
        if (beepstatus == BEEPOFF) {
          beepstatus = BEEPREST;
        }
      }
      if (now - lastbeepon >= PREBEEPREST) {
        if (now - lastbeepon < PREBEEPREST + BEEPDURATION) {
          if (beepstatus == BEEPREST) {
            ledcWriteTone(BUZZER_CHANNEL2, beepnote);
            beepstatus = BEEPING;
          }
        } else if (now - lastbeepon < PREBEEPREST + BEEPDURATION + POSTBEEPREST) {
          if (beepstatus == BEEPING) {
            ledcWriteTone(BUZZER_CHANNEL2, REST);
            beepstatus = BEEPPOST;
          }
        } else {  // beep completed
          if (beepstatus == BEEPPOST) {
            beepon = false;
            shouldresume = true;
            beepstatus = BEEPOFF;
          }
        }
      }
    }
  }
}
//*******************************************************************************************
void init_settings() 
{
  File f;
  if (LittleFS.exists("/settings")) 
    {
    Serial.println("=====SETTINGS FILE exists\n");
    } 
  else 
    {
    Serial.println("=====SETTINGS FILE does NOT exist\n");
    f = LittleFS.open("/settings", "w");
    f.println("on");
    f.println("on");
    f.close();
    }
  f = LittleFS.open("/settings", "r");
  String s = f.readString();
  Serial.println("[" + s + "]");
  if (s[0] == '1') 
    {
    musicset = true;
    Serial.println("music is ON");
    } 
  else 
    {
    musicset = false;
    Serial.println("music is OFF");
    }
  if (s[1] == '1') 
    {
    audioset = true;
    Serial.println("audio is ON");
    } 
  else  
    {
    audioset = false;
    Serial.println("audio is OFF");
    }
}
//*******************************************************************************************
void write_settings() 
{
  File f;
  f = LittleFS.open("/settings", "w");
  if (musicset)
    f.print("1");
  else
    f.print("0");
  if (audioset)
    f.print("1");
  else
    f.print("0");
  f.close();
}
//*******************************************************************************************
void init_audio() 
{
  pinMode(BUZZER_PIN1, OUTPUT);
  pinMode(BUZZER_PIN2, OUTPUT);
  digitalWrite(BUZZER_PIN1, LOW);
  digitalWrite(BUZZER_PIN2, LOW);
  ledcAttachPin(BUZZER_PIN1, BUZZER_CHANNEL1);
  ledcAttachPin(BUZZER_PIN2, BUZZER_CHANNEL2);
  ledcWriteTone(BUZZER_CHANNEL1, REST);
  ledcWriteTone(BUZZER_CHANNEL2, REST);

  rh_pos = 0;
  lh_pos = 0;
  playnow = false;
  shouldresume = false;
  beepon = false;
  beepstatus = BEEPOFF;
}
//*******************************************************************************************
void stop_audio() 
{
  ledcWriteTone(BUZZER_CHANNEL1, REST);
  if (beepstatus == BEEPOFF)
    ledcWriteTone(BUZZER_CHANNEL2, REST);
  rh_pos = 0;
  lh_pos = 0;
  playnow = false;
}
//*******************************************************************************************
void restart() 
{
  Serial.println("  inside restart");
  init_logo();
  autorepeat = false;
  give_chance_to_update();
  Serial.println("starting game");
  game_start();
  Serial.println("exiting restart");

  /*

    ledcWriteTone(BUZZER_CHANNEL1, REST);
    ledcWriteTone(BUZZER_CHANNEL2, REST);
    ledcDetachPin(BUZZER_PIN1);
    ledcDetachPin(BUZZER_PIN2);
    digitalWrite(BUZZER_PIN1, LOW);
    digitalWrite(BUZZER_PIN2, LOW);
    delay(100);
    ESP.restart();
  */
  return;
}
//*************************************************************************************************
void init_fs() 
{
  if (!LittleFS.begin(false /* false: Do not format if mount failed */)) 
    {
    Serial.println("Failed to mount LittleFS");
    if (!LittleFS.begin(true /* true: format */)) 
      {
      Serial.println("Failed to format LittleFS");
      } 
    else 
      {
      Serial.println("LittleFS formatted successfully");
      //filesystemOK = true;
      }
    } 
  else 
    {  // Initial mount success
    // filesystemOK = true;
    }
}
//*************************************************************************************************
void init_buttons() 
{
  
  for (short i = 0; i < NBUTTONS; i++) 
    {
    digitalWrite(buttonPins[i], HIGH);
    buttonStates[i] = HIGH;
    pinMode(buttonPins[i], INPUT_PULLUP);
    }

}
//*************************************************************************************************
void init_logo() 
{
  display->fillRect(0, 0, 64, 64, 0x0000);
  for (int i = 10; i < 54; i++) {
    for (int j = 0; j < 64; j++) {
      if (logo[(i - 10) * 64 + j] != 65535)
        display->drawPixel(j, i, logo[(i - 10) * 64 + j]);
    }
  }
}
//*************************************************************************************************
void give_chance_to_update() 
{
  for (int i = 0; i < 1000; i += 5) 
    {
    read_buttons();
    if (justPressed[RIGHT] || justPressed[LEFT] || isPressed[LEFT] || isPressed[RIGHT]) 
      {
      Serial.println("connecting");
      wifi_connect();
      //ota_update(20);
      wifi_disconnect();
      break;
      } 
    else 
    {
      /*  
    if (justPressed[TRIGGER] || isPressed[TRIGGER]) {
      Serial.println("testing");
     display->fillRect(0, 0, 64, 64, 0x0000);
     delay(1000);
     display->fillRect(0, 0, 64, 64, 0xffff);
     delay(2000);
     display->fillRect(0, 0, 64, 64, display->color565(255,0,0));
     delay(2000);
     display->fillRect(0, 0, 64, 64, display->color565(0,255,0));
     delay(2000);
     display->fillRect(0, 0, 64, 64, display->color565(0,0,255));
     delay(2000);
     
      break;
    } 
    */
    }
    delay(5);
  }
}
//**************************************************************************************************
void setup() 
{
  delay(100);
  Serial.begin(115200);
  delay(100);
  Serial.println("START");

  init_audio();
  xTaskCreatePinnedToCore(
    playaudio, /* Function to implement the task */
    "Task1",   /* Name of the task */
    10000,     /* Stack size in words */
    NULL,      /* Task input parameter */
    0,         /* Priority of the task */
    &music,    /* Task handle. */
    0);        /* Core where the task should run */

  init_fs();
  init_settings();
  init_buttons();

  set_role();
  Serial.println("...Starting Display");

    HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
    );

  //******* 64x64 RGB MAtirx Display Setup *******
  display = new MatrixPanel_I2S_DMA(mxconfig);
  //display->begin();  // setup display with pins as pre-defined in the library
  display->begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN);   
  display->setBrightness8(127);

  init_logo();
  autorepeat = false;

  //delay(900);
  give_chance_to_update();
  game_start();
}
//**************************************************************************************************
void loop() 
{
  if (game_over)
    return;
  bool needs_redraw = false;
  static unsigned char shape = 1, prev_shape;
  static unsigned char rotate = 0, prev_rotate;
  static char pos_x = ((TETRIS_BOARD_SIZE_X - TETRIMINO_SIZE) / 2), prev_pos_x, prev_pos_y;
  if (millis() - last_moved >= delays[level]) 
    {
    Pos_Y++;
    print_board();
    set_ghost_block(pos_x, Pos_Y, shape, rotate);
    last_moved = millis();
    needs_redraw = true;
    }

  read_buttons();
  
  if (justPressed[UP]) 
    {
    needs_redraw = true;
    print_board();
    set_ghost_block(pos_x, Pos_Y, shape, rotate);
    if (rotate >= 3) 
      {
      rotate = 0;
      } 
    else 
      {
      rotate++;
      }
    }
    
  if (justPressed[DOWN]) 
    {
    needs_redraw = true;
    print_board();
    set_ghost_block(pos_x, Pos_Y, shape, rotate);
    if (rotate == 0) 
      {
      rotate = 3;
      } 
    else 
      {
      rotate--;
      }
    }

  if (set_block(pos_x, Pos_Y, shape, rotate, true) == false) 
    {
    rotate = prev_rotate;
    }

  if (justPressed[RIGHT]) 
    {
    needs_redraw = true;
    print_board();
    set_ghost_block(pos_x, Pos_Y, shape, rotate);
    if (pos_x >= (TETRIS_BOARD_SIZE_X - 1)) 
      {
      pos_x = 0;
      } 
    else 
      {
      pos_x++;
      }
    justPressed[RIGHT] = 0; 
    }

  if (justPressed[LEFT]) 
    {
    needs_redraw = true;
    print_board();
    set_ghost_block(pos_x, Pos_Y, shape, rotate);

    if (pos_x <= 0) 
      {
      pos_x = (TETRIS_BOARD_SIZE_X - 1);
      } 
    else 
      {
      pos_x--;
      }
    justPressed[LEFT] = 0;  
    }

  if (set_block(pos_x, Pos_Y, shape, rotate, true) == false) 
    {
    pos_x = prev_pos_x;
    }
    
  if (justPressed[TRIGGER]) 
    {
    //Serial.println("mandato giu'");
    if (role == MASTER) 
      {
      send_lines(4);
      }
    needs_redraw = true;
    print_board();
    set_ghost_block(pos_x, Pos_Y, shape, rotate);
    last_moved = millis();
    Pos_Y++;
    }
    
  if (set_block(pos_x, Pos_Y, shape, rotate, true) == false) 
    {
    //Serial.println("atterrato");
    beep(DROP);
    lastPressed[TRIGGER] += 60000;
    justPressed[LEFT] = justPressed[RIGHT] = false;
    print_board();

    if (set_block(prev_pos_x, prev_pos_y, shape, prev_rotate, false) == false) 
      {
      //Serial.println("Game OVER");
      if (mode == ONE_PLAYER) 
        {
        draw_game_over(GAME_OVER);
        } 
      else 
        {
        outgoingmsg.type = LOST;
        outgoingmsg.data = 0;
        esp_now_send(macs[party], (uint8_t *)&outgoingmsg, sizeof(outgoingmsg));
        draw_game_over(YOU_LOSE);
        }
      Serial.println("Restarting");
      restart();
      Serial.println("Returning from loop");
      return;
      }
    Pos_Y = 0;
    pos_x = ((TETRIS_BOARD_SIZE_X - TETRIMINO_SIZE) / 2);
    rotate = 0;

    short cleared = clear_block();
    if (cleared > 0) 
      {
      rows += cleared;
      switch (cleared) 
        {
        case 1:
          score += (level + 1);
          break;
        case 2:
          score += 3 * (level + 1);
          break;
        case 3:
          score += 5 * (level + 1);
          break;
        case 4:
          score += 8 * (level + 1);
          break;
        }
      send_lines(cleared);
      if (rows % 10 == 0 && level < 19) 
        {
        level += 1;
        }
      draw_score();
      }
    shape = get_next_tetrimino();
    needs_redraw = true;
    //print_board();

    } 
  else 
    {
    // non atterrato
    }

  prev_shape = shape;
  prev_rotate = rotate;
  prev_pos_x = pos_x;
  prev_pos_y = Pos_Y;
  if (needs_redraw) 
    {
    draw_all_block();
    } 
  else 
    {
    //Serial.println("no need to redraw");
    }
}
//**************************************************************************************************
void init_board(void) 
{
  //Serial.println("init board");
  // 17 - 1, 3 - 1, 17 + ( 10 * 3 ), 3 + ( 20 * 3 ) == 16, 2, 47, 63
  draw_rectangle(TETRIS_WINDOW_POSITION_X - 1,
                 TETRIS_WINDOW_POSITION_Y - 1,
                 TETRIS_WINDOW_POSITION_X + (TETRIS_WINDOW_SIZE_X * BLOCK_SIZE) + 1,
                 TETRIS_WINDOW_POSITION_Y + (TETRIS_WINDOW_SIZE_Y * BLOCK_SIZE) + 1,
                 0xff,
                 0xff,
                 0xff);

  unsigned char loop_x, loop_y;
  for (loop_y = 0; loop_y < TETRIS_BOARD_SIZE_Y; loop_y++) 
    {
    for (loop_x = 0; loop_x < TETRIS_BOARD_SIZE_X; loop_x++) 
      {
      if ((loop_x == 0) || (loop_x == (TETRIS_BOARD_SIZE_X - 1)) || (loop_y == (TETRIS_BOARD_SIZE_Y - 1))) 
        {
        Tetris_Board[loop_y][loop_x] = WALL;
        //Tetris_Board_Overlay[loop_y][loop_x] = WALL;
        } 
      else 
        {
        Tetris_Board[loop_y][loop_x] = BLACK;
        //Tetris_Board_Overlay[loop_y][loop_x] = BLACK;
        }
      }
    }
}
//*************************************************************************************************
void init_score() 
{
  level = rows = score = 0;
  display->setFont(&Font4x5Fixed);  // choose font
  display->setTextSize(1);
  display->setTextColor(display->color565(0, 255, 255));
  display->setCursor(1, 5);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println("LVL");
  display->setTextColor(display->color565(255, 0, 255));
  display->setCursor(1, 18);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println("ROW");
  display->setTextColor(display->color565(0, 255, 0));
  display->setCursor(1, 31);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println("SCO");
  display->setTextColor(display->color565(255, 255, 255));

  display->setFont(&Font3x5FixedNumMic);  // choose font
  display->setTextSize(1);
  display->setCursor(12, 12);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println("0");
  display->setCursor(12, 25);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println("0");
  display->setCursor(12, 38);  // as per Adafruit convention custom fonts draw up from line so move cursor
  display->println("0");

  display->setFont();  // return to the system font
}
//*************************************************************************************************
void init_next_tetrimino_board(void) 
{
  //Serial.println("init next tetrimino board");
  // 17 + ( 10 * 3 ) + 2, 3 - 1, 17 + (( 10 + 4 ) * 3 ) + 3, 3 + (5 * 4 * 3) == 49, 2, 62
  //#define TETRIMINO_WINDOW_POSITION_X (TETRIS_WINDOW_POSITION_X + (TETRIS_WINDOW_SIZE_X * BLOCK_SIZE) + 2)
  //#define TETRIMINO_WINDOW_POSITION_Y TETRIS_WINDOW_POSITION_Y
  draw_rectangle(TETRIMINO_WINDOW_POSITION_X - 1,
                 TETRIMINO_WINDOW_POSITION_Y - 1,
                 TETRIMINO_WINDOW_POSITION_X + (TETRIMINO_SIZE * BLOCK_SIZE) + 1,
                 TETRIMINO_WINDOW_POSITION_Y + (NEXT_TETRIMINO_LIST_COUNT * TETRIMINO_SIZE * BLOCK_SIZE) + 1,
                 0xff,
                 0xff,
                 0xff);

  draw_rectangle_fill(TETRIMINO_WINDOW_POSITION_X,
                      TETRIMINO_WINDOW_POSITION_Y,
                      TETRIMINO_WINDOW_POSITION_X + (TETRIMINO_SIZE * BLOCK_SIZE),
                      TETRIMINO_WINDOW_POSITION_Y + (NEXT_TETRIMINO_LIST_COUNT * TETRIMINO_SIZE * BLOCK_SIZE),
                      0x00,
                      0x00,
                      0x00);

  for (unsigned char tetrimino_list_clear_loop = 0; tetrimino_list_clear_loop < NEXT_TETRIMINO_LIST_COUNT; tetrimino_list_clear_loop++) 
    {
    Next_Tetrimino_List[tetrimino_list_clear_loop] = 0;
    }

  //randomSeed(analogRead(3));
  return;
}
//*************************************************************************************************
unsigned char get_next_tetrimino(void) 
{
  //return 'o';
  //Serial.println("get next tetrimino");

  bool fill_tetrimino_list_flag;
  unsigned char tetrimino_list_loop_1, tetrimino_list_loop_2;
  bool same_tetrimino_find_flag = false;
  unsigned char temp_random_value;
  unsigned char next_tetrimino;

  unsigned char next_tetrimino_board[NEXT_TETRIMINO_LIST_COUNT * TETRIMINO_SIZE][TETRIMINO_SIZE] = { 0 };

  unsigned char loop_block;
  unsigned char loop_x, loop_y, loop_block_x, loop_block_y;



  for (tetrimino_list_loop_1 = 0; tetrimino_list_loop_1 < NEXT_TETRIMINO_LIST_COUNT; tetrimino_list_loop_1++) 
    {
    if ((Next_Tetrimino_List[tetrimino_list_loop_1] <= BLACK) || (Next_Tetrimino_List[tetrimino_list_loop_1] >= TETRIMINO_COLOR_COUNT)) 
      {
      do {
        temp_random_value = rand() % (TETRIMINO_COLOR_COUNT - 1) + 1;
        same_tetrimino_find_flag = false;

        for (tetrimino_list_loop_2 = 0; tetrimino_list_loop_2 < NEXT_TETRIMINO_LIST_COUNT; tetrimino_list_loop_2++) 
          {
          if (temp_random_value == Next_Tetrimino_List[tetrimino_list_loop_2]) 
            {
            same_tetrimino_find_flag = true;
            }
          }
        } while (same_tetrimino_find_flag == true);
      Next_Tetrimino_List[tetrimino_list_loop_1] = temp_random_value;
      }
    }

  next_tetrimino = Next_Tetrimino_List[0];

  for (tetrimino_list_loop_1 = 0; tetrimino_list_loop_1 < (NEXT_TETRIMINO_LIST_COUNT - 1); tetrimino_list_loop_1++) 
    {
    Next_Tetrimino_List[tetrimino_list_loop_1] = Next_Tetrimino_List[tetrimino_list_loop_1 + 1];
    }

  do {
    temp_random_value = rand() % (TETRIMINO_COLOR_COUNT - 1) + 1;
    same_tetrimino_find_flag = false;

    for (tetrimino_list_loop_2 = 0; tetrimino_list_loop_2 < NEXT_TETRIMINO_LIST_COUNT; tetrimino_list_loop_2++) 
      {
      if (temp_random_value == Next_Tetrimino_List[tetrimino_list_loop_2]) 
        {
        same_tetrimino_find_flag = true;
        }
      }
    } while (same_tetrimino_find_flag == true);
  Next_Tetrimino_List[NEXT_TETRIMINO_LIST_COUNT - 1] = temp_random_value;

  for (tetrimino_list_loop_1 = 0; tetrimino_list_loop_1 < NEXT_TETRIMINO_LIST_COUNT; tetrimino_list_loop_1++) 
    {
    for (loop_block = 0; loop_block < TETRIMINO_SIZE; loop_block++) 
      {
      next_tetrimino_board[(Tetrimino_Shape[Next_Tetrimino_List[tetrimino_list_loop_1]][0][loop_block][1] + (tetrimino_list_loop_1 * TETRIMINO_SIZE))][(Tetrimino_Shape[Next_Tetrimino_List[tetrimino_list_loop_1]][0][loop_block][0])] = Next_Tetrimino_List[tetrimino_list_loop_1];
      }
    }

  for (loop_y = 0; loop_y < (NEXT_TETRIMINO_LIST_COUNT * TETRIMINO_SIZE); loop_y++) 
    {
    for (loop_block_y = 0; loop_block_y < BLOCK_SIZE; loop_block_y++) 
      {
      send_command((TETRIMINO_WINDOW_POSITION_Y + loop_y * BLOCK_SIZE + loop_block_y),
                   (TETRIMINO_WINDOW_POSITION_Y + loop_y * BLOCK_SIZE + loop_block_y + 1),
                   TETRIMINO_WINDOW_POSITION_X,
                   TETRIMINO_WINDOW_POSITION_X + (TETRIMINO_SIZE * BLOCK_SIZE));

      for (loop_x = 0; loop_x < TETRIMINO_SIZE; loop_x++) 
        {
        uint16_t c = random(1000, 300000);
        for (loop_block_x = 0; loop_block_x < BLOCK_SIZE; loop_block_x++) 
          {
          display->drawPixel
            (
            TETRIMINO_WINDOW_POSITION_X + loop_x * BLOCK_SIZE + loop_block_x,
            TETRIMINO_WINDOW_POSITION_Y + loop_y * BLOCK_SIZE + loop_block_y,
            display->color565(Tetrimino_Color[next_tetrimino_board[loop_y][loop_x]][loop_block_y][loop_block_x][0],
                             Tetrimino_Color[next_tetrimino_board[loop_y][loop_x]][loop_block_y][loop_block_x][1],
                             Tetrimino_Color[next_tetrimino_board[loop_y][loop_x]][loop_block_y][loop_block_x][2]));
          }
        }
      }
    }
  send_height();
  if (linestoadd > 0)
    add_lines();
  return next_tetrimino;
  
}
//*************************************************************************************************
unsigned short clear_block(void) 
{
  short loop_x, loop_y;
  bool fill_block_flag_x = false;
  bool fill_block_flag_y[TETRIS_BOARD_SIZE_Y] = { false };

  unsigned char first_fill_block_y = 0, first_nofill_block_y = 0;

  unsigned short point_counter = 0;
  for (loop_y = 0; loop_y < (TETRIS_BOARD_SIZE_Y - 1); loop_y++) 
    {
    fill_block_flag_x = true;
    for (loop_x = 1; loop_x < (TETRIS_BOARD_SIZE_X - 1); loop_x++) 
      {
      if (Tetris_Board[loop_y][loop_x] == BLACK) 
        {
        fill_block_flag_x = false;
        }
      }
    if (fill_block_flag_x == true) 
      {
      fill_block_flag_y[loop_y] = true;
      for (loop_x = 1; loop_x < (TETRIS_BOARD_SIZE_X - 1); loop_x++) 
        {
        Tetris_Board[loop_y][loop_x] = BLACK;
        }
      point_counter++;
      } 
    else 
      {
      fill_block_flag_y[loop_y] = false;
      }
    }

  do {
    first_fill_block_y = 0;
    first_nofill_block_y = 0;
    for (loop_y = (TETRIS_BOARD_SIZE_Y - 2); loop_y >= 0; loop_y--) 
      {
      if ((fill_block_flag_y[loop_y] == true) && (first_fill_block_y == 0)) 
        {
        first_fill_block_y = loop_y;
        }
      if ((fill_block_flag_y[loop_y] == false) && (first_fill_block_y != 0) && (first_nofill_block_y == 0)) 
        {
        first_nofill_block_y = loop_y;
        }
      }

    for (loop_y = first_nofill_block_y; loop_y >= 0; loop_y--) 
      {
      for (loop_x = 1; loop_x < (TETRIS_BOARD_SIZE_X - 1); loop_x++)      
        {
        Tetris_Board[first_fill_block_y - first_nofill_block_y + loop_y][loop_x] = Tetris_Board[loop_y][loop_x];
        Tetris_Board[loop_y][loop_x] = BLACK;
        }
      fill_block_flag_y[first_fill_block_y - first_nofill_block_y + loop_y] = fill_block_flag_y[loop_y];
      fill_block_flag_y[loop_y] = false;
      }
    } while ((first_fill_block_y != 0) || (first_nofill_block_y != 0));
  return point_counter;
}
//*************************************************************************************************
void set_ghost_block(unsigned char position_x, unsigned char position_y, unsigned char shape, unsigned char rotate) 
{
  static char prev_position_x, prev_position_y, prev_shape, prev_rotate;
  unsigned char loop_block;

  for (int nextY = position_y + 1; nextY < TETRIS_BOARD_SIZE_Y; nextY++) 
    {
    for (loop_block = 0; loop_block < TETRIMINO_SIZE; loop_block++) 
      {
      short blockx = Tetrimino_Shape[shape][rotate][loop_block][0] + position_x;
      short blocky = Tetrimino_Shape[shape][rotate][loop_block][1] + nextY;
      if (Tetris_Board[blocky][blockx] != BLACK) 
        {
        // found
        // add ghost to nextY-1
        //Serial.println("land at: " + String(nextY - 1));
        return;
        }
      }
    }
};
//*************************************************************************************************
bool set_block(unsigned char position_x, unsigned char position_y, unsigned char shape, unsigned char rotate, bool clear_flag) 
{
  static char prev_position_x, prev_position_y, prev_shape, prev_rotate;
  static bool prev_clear_flag;
  unsigned char loop_block;


  if (prev_clear_flag == true) 
    {
    for (loop_block = 0; loop_block < TETRIMINO_SIZE; loop_block++) 
      {
      Tetris_Board[(Tetrimino_Shape[prev_shape][prev_rotate][loop_block][1] + prev_position_y)][(Tetrimino_Shape[prev_shape][prev_rotate][loop_block][0] + prev_position_x)] = BLACK;
      }
    }

  for (loop_block = 0; loop_block < TETRIMINO_SIZE; loop_block++) 
    {
    if ((Tetris_Board[(Tetrimino_Shape[shape][rotate][loop_block][1] + position_y)][(Tetrimino_Shape[shape][rotate][loop_block][0] + position_x)] != BLACK) && (Tetris_Board[(Tetrimino_Shape[prev_shape][prev_rotate][loop_block][1] + prev_position_y)][(Tetrimino_Shape[prev_shape][prev_rotate][loop_block][0] + prev_position_x)] != Tetris_Board[(Tetrimino_Shape[shape][rotate][loop_block][1] + position_y)][(Tetrimino_Shape[shape][rotate][loop_block][0] + position_x)])) 
      {
      Tetris_Board[(Tetrimino_Shape[prev_shape][prev_rotate][loop_block][1] + prev_position_y)][(Tetrimino_Shape[prev_shape][prev_rotate][loop_block][0] + prev_position_x)] = prev_shape;
      return false;
      }
    }

  for (loop_block = 0; loop_block < TETRIMINO_SIZE; loop_block++) 
    {
    Tetris_Board[(Tetrimino_Shape[shape][rotate][loop_block][1] + position_y)][(Tetrimino_Shape[shape][rotate][loop_block][0] + position_x)] = shape;
    }


  prev_position_x = position_x;
  prev_position_y = position_y;
  prev_shape = shape;
  prev_rotate = rotate;
  prev_clear_flag = clear_flag;

  return true;
};
//*************************************************************************************************
void draw_all_block(void) 
{
  unsigned char loop_x, loop_y, loop_block_x, loop_block_y;
  //Serial.println("redrawing "+String(TETRIS_WINDOW_POSITION_X)+" "+String(TETRIS_WINDOW_POSITION_Y)+" "+String(TETRIS_WINDOW_SIZE_X)+" "+String(TETRIS_WINDOW_SIZE_Y)+" "+String(BLOCK_SIZE));
  /*display->fillRect(TETRIS_WINDOW_POSITION_X, TETRIS_WINDOW_POSITION_Y,
    TETRIS_WINDOW_SIZE_X*BLOCK_SIZE,TETRIS_WINDOW_SIZE_Y*BLOCK_SIZE,0x0000);
  */
  for (loop_y = 0; loop_y < TETRIS_WINDOW_SIZE_Y; loop_y++) 
    {
    //Serial.println(loop_y);
    for (loop_block_y = 0; loop_block_y < BLOCK_SIZE; loop_block_y++) 
      {
      //Serial.print("cancello blocco ");
      //Serial.flush();
      send_command((TETRIS_WINDOW_POSITION_Y + loop_y * BLOCK_SIZE + loop_block_y),
                   (TETRIS_WINDOW_POSITION_Y + loop_y * BLOCK_SIZE + loop_block_y + 1),
                   TETRIS_WINDOW_POSITION_X,
                   (TETRIS_WINDOW_POSITION_X + (TETRIS_WINDOW_SIZE_X * BLOCK_SIZE) - 1));

      //Serial.print(" ricreo blocco ");
      //Serial.flush();
      for (loop_x = 0; loop_x < TETRIS_WINDOW_SIZE_X; loop_x++) 
        {
        //Serial.print(loop_y);
        //Serial.print(" ");
        //Serial.println(loop_x);
        for (loop_block_x = 0; loop_block_x < BLOCK_SIZE; loop_block_x++) 
          {
          display->drawPixel(
            TETRIS_WINDOW_POSITION_X + loop_x * BLOCK_SIZE + loop_block_x,
            TETRIS_WINDOW_POSITION_Y + loop_y * BLOCK_SIZE + loop_block_y,
            display->color565(
              Tetrimino_Color[Tetris_Board[loop_y + 1][loop_x + 1]][loop_block_y][loop_block_x][0],
              Tetrimino_Color[Tetris_Board[loop_y + 1][loop_x + 1]][loop_block_y][loop_block_x][1],
              Tetrimino_Color[Tetris_Board[loop_y + 1][loop_x + 1]][loop_block_y][loop_block_x][2]));
          }
        }
      }
    }
}// end draw_all_blocks
//*************************************************************************************************
void draw_pixel(unsigned char pixel_x, unsigned char pixel_y, unsigned char color_r, unsigned char color_g, unsigned char color_b) 
{
  if ((pixel_x < MATRIXLED_X_COUNT) && (pixel_y < MATRIXLED_Y_COUNT)) {
    display->drawPixelRGB888(pixel_x, pixel_y, color_r, color_g, color_b);
  }
}
//*************************************************************************************************
void draw_line(unsigned char start_pixel_x, unsigned char start_pixel_y, unsigned char end_pixel_x, unsigned char end_pixel_y, unsigned char color_r, unsigned char color_g, unsigned char color_b) 
{
  if ((start_pixel_x < MATRIXLED_X_COUNT) && (end_pixel_x < MATRIXLED_X_COUNT) && (start_pixel_y < MATRIXLED_Y_COUNT) && (end_pixel_y < MATRIXLED_Y_COUNT)) {
    uint16_t c = display->color565(color_r, color_g, color_b);
    display->drawLine(start_pixel_x, start_pixel_y, end_pixel_x, end_pixel_y, c);
  }
}
//*************************************************************************************************
void draw_rectangle(unsigned char start_pixel_x, unsigned char start_pixel_y, unsigned char end_pixel_x, unsigned char end_pixel_y, unsigned char color_r, unsigned char color_g, unsigned char color_b) 
{

  if ((end_pixel_x < MATRIXLED_X_COUNT) && (end_pixel_y < MATRIXLED_Y_COUNT) && (start_pixel_x <= end_pixel_x) && (start_pixel_y <= end_pixel_y)) {
    uint16_t c = display->color565(color_r, color_g, color_b);
    display->drawRect(start_pixel_x, start_pixel_y, end_pixel_x - start_pixel_x, end_pixel_y - start_pixel_y, c);
  }
}
//*************************************************************************************************
void draw_rectangle_fill(unsigned char start_pixel_x, unsigned char start_pixel_y, unsigned char end_pixel_x, unsigned char end_pixel_y, unsigned char color_r, unsigned char color_g, unsigned char color_b) 
{

  if ((end_pixel_x < MATRIXLED_X_COUNT) && (end_pixel_y < MATRIXLED_Y_COUNT) && (start_pixel_x <= end_pixel_x) && (start_pixel_y <= end_pixel_y)) {
    uint16_t c = display->color565(color_r, color_g, color_b);
    display->fillRect(start_pixel_x, start_pixel_y, end_pixel_x - start_pixel_x, end_pixel_y - start_pixel_y, c);
  }
}
//*************************************************************************************************
void draw_text(unsigned char start_pixel_x, unsigned char start_pixel_y, char *chara_array, unsigned char font_color_r, unsigned char font_color_g, unsigned char font_color_b, uint8_t size) 
{
  display->setCursor(start_pixel_x, start_pixel_y);
  display->setTextColor(display->color565(font_color_r, font_color_g, font_color_b));
  display->setTextSize(size);
  display->print(chara_array);
}
//*************************************************************************************************
void draw_text_fill_back(unsigned char start_pixel_x, unsigned char start_pixel_y, char *chara_array, unsigned char font_color_r, unsigned char font_color_g, unsigned char font_color_b, unsigned char back_color_r, unsigned char back_color_g, unsigned char back_color_b, uint8_t size) 
{
  display->setCursor(start_pixel_x, start_pixel_y);
  //display->setTextColor(display->color565(font_color_r, font_color_g, font_color_b));
  display->setTextColor(display->color565(font_color_r, font_color_g, font_color_b), display->color565(back_color_r, back_color_g, back_color_b));
  display->setTextSize(size);
  //display->setTextWrap(boolean w);
  display->print(chara_array);
}
//*************************************************************************************************
