 /*
  Playing notes based on sliding pot position
  https://wokwi.com/projects/351175246893548120
  by dlloydev, December 2022.
*/



#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

#define TEMPO 60000/140 // 120 BPM
#define D_H TEMPO*2
#define D_HDOT TEMPO*3
#define D_Q TEMPO
#define D_QDOT TEMPO*1.5
#define D_E TEMPO/2
#define D_EDOT TEMPO*1.5/2
#define D_S TEMPO/4
#define D_T TEMPO/8
 
 int rh[] = {
// M1
NOTE_E5, D_Q,
NOTE_B4, D_E,
NOTE_C5, D_E,
NOTE_D5, D_E,
NOTE_E5, D_S,
NOTE_D5, D_S,
NOTE_C5, D_E,
NOTE_B4, D_E,
// M2
NOTE_A4, D_E,
REST, D_E,
NOTE_A4, D_E,
NOTE_C5, D_E,
NOTE_E5, D_Q,
NOTE_D5, D_E,  
NOTE_C5, D_E,
// M3
NOTE_B4, D_Q,
REST, D_E,
//NOTE_B4, D_E,
NOTE_C5, D_E,  
NOTE_D5, D_Q,  
NOTE_E5, D_Q,
// M4
NOTE_C5, D_Q,  
NOTE_A4, D_E,
REST, D_E,  
NOTE_A4, D_Q,
REST, D_Q,  
// M5
REST, D_E,
NOTE_D5, D_Q,  
NOTE_F5, D_E,  
NOTE_A5, D_Q,  
NOTE_G5, D_E,  
NOTE_F5, D_E,
// M6
NOTE_E5, D_QDOT,  
NOTE_C5, D_E,  
NOTE_E5, D_Q,  
NOTE_D5, D_E,  
NOTE_C5, D_E,
// M7
NOTE_B4, D_E,  
REST, D_E,
NOTE_B4, D_E,  
NOTE_C5, D_E,  
NOTE_D5, D_Q,  
NOTE_E5, D_Q,
// M8
NOTE_C5, D_Q,  
NOTE_A4, D_E,  
REST, D_E,
NOTE_A4, D_Q, 
REST, D_Q,
// M9
NOTE_E5, D_Q,
NOTE_B4, D_E,
NOTE_C5, D_E,
NOTE_D5, D_E,
NOTE_E5, D_S,
NOTE_D5, D_S,
NOTE_C5, D_E,
NOTE_B4, D_E,
// M10
NOTE_A4, D_E,
REST, D_E,
NOTE_A4, D_E,
NOTE_C5, D_E,
NOTE_E5, D_Q,
NOTE_D5, D_E,  
NOTE_C5, D_E,
// M11
NOTE_B4, D_Q,
REST, D_E,
//NOTE_B4, D_E,
NOTE_C5, D_E,  
NOTE_D5, D_Q,  
NOTE_E5, D_Q,
// M12
NOTE_C5, D_Q,  
NOTE_A4, D_E,
REST, D_E,  
NOTE_A4, D_Q,
REST, D_Q,  
// M13
REST, D_E,
NOTE_D5, D_Q,  
NOTE_F5, D_E,  
NOTE_A5, D_Q,  
NOTE_G5, D_E,  
NOTE_F5, D_E,
// M14
NOTE_E5, D_QDOT,  
NOTE_C5, D_E,  
NOTE_E5, D_Q,  
NOTE_D5, D_E,  
NOTE_C5, D_E,
// M15
NOTE_B4, D_E,  
REST, D_E,
NOTE_B4, D_E,  
NOTE_C5, D_E,  
NOTE_D5, D_Q,  
NOTE_E5, D_Q,
// M16
NOTE_C5, D_Q,  
NOTE_A4, D_E,  
REST, D_E,
NOTE_A4, D_Q, 
REST, D_Q,
// M17
NOTE_E5, D_H,  
NOTE_C5, D_H,
// M18
NOTE_D5, D_H,   
NOTE_B4, D_H,
// M19
NOTE_C5, D_H,   
NOTE_A4, D_H,
// M20
NOTE_GS4, D_H,  
NOTE_B4, D_H,  
// M21
NOTE_E5, D_H,   
NOTE_C5, D_H,
// M22
NOTE_D5, D_H,   
NOTE_B4, D_H,
// M23
NOTE_C5, D_Q,   
NOTE_E5, D_Q,  
NOTE_A5, D_H,
// M24
NOTE_GS5, D_HDOT,
REST, D_Q 
};

//int rh[] = {NOTE_C, 5, NOTE_D, 5, NOTE_E, 5, NOTE_F, 5, NOTE_G, 5, NOTE_A, 5, NOTE_B, 5, NOTE_C, 6};
//int rh[] = {NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
//int dur_rh[] = {500, 500, 500, 500, 500, 500, 500, 500};
int notes_rh = sizeof(rh)/sizeof(rh[0]);

 int lh[] = {
// M1
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
// M2
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
// M3
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
// M4
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_S,
REST, D_S,
NOTE_A2, D_S,
REST, D_S,
NOTE_B2, D_S,
REST, D_S,
NOTE_C3, D_S,
REST, D_S,
// M5
NOTE_D3, D_E,
NOTE_D4, D_E,
NOTE_D3, D_E,
NOTE_D4, D_E,
NOTE_D3, D_E,
NOTE_D4, D_E,
NOTE_D3, D_E,
NOTE_D4, D_E,
// M6
NOTE_C3, D_E,
NOTE_C4, D_E,
NOTE_C3, D_E,
NOTE_C4, D_E,
NOTE_C3, D_E,
NOTE_C4, D_E,
NOTE_C3, D_E,
NOTE_C4, D_E,
// M7
NOTE_B2, D_E,
NOTE_B3, D_E,
NOTE_B2, D_E,
NOTE_B3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
// M8
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
// M9
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
// M10
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
// M11
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
// M12
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_S,
REST, D_S,
NOTE_A2, D_S,
REST, D_S,
NOTE_B2, D_S,
REST, D_S,
NOTE_C3, D_S,
REST, D_S,
// M13
NOTE_D3, D_E,
NOTE_D4, D_E,
NOTE_D3, D_E,
NOTE_D4, D_E,
NOTE_D3, D_E,
NOTE_D4, D_E,
NOTE_D3, D_E,
NOTE_D4, D_E,
// M14
NOTE_C3, D_E,
NOTE_C4, D_E,
NOTE_C3, D_E,
NOTE_C4, D_E,
NOTE_C3, D_E,
NOTE_C4, D_E,
NOTE_C3, D_E,
NOTE_C4, D_E,
// M15
NOTE_B2, D_E,
NOTE_B3, D_E,
NOTE_B2, D_E,
NOTE_B3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
// M16
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
// M17
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
// M18
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_GS2, D_E,
NOTE_GS3, D_E,
// M19
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
// M20
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
// M21
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
// M22
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_GS2, D_E,
NOTE_GS3, D_E,
NOTE_GS2, D_E,
NOTE_GS3, D_E,
// M23
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
NOTE_A2, D_E,
NOTE_A3, D_E,
// M24
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
NOTE_E2, D_E,
NOTE_E3, D_E,
 };
int notes_lh = sizeof(lh)/sizeof(lh[0]);
int rh_pos, lh_pos;
unsigned long int last_rh, last_lh;
