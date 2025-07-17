#include <Arduino.h>

#include "ESP_I2S.h"
#include "ESP_SR.h"
#include "ESP_TTS.h"

ESP_TTS tts;
//  MIC for Xiaozhi 
// const int I2S_SCK = 5;
// const int I2S_WS = 4;
// const int I2S_SD = 6;

// mic 
#define I2S_PIN_BCK 5
#define I2S_PIN_WS  4
#define I2S_PIN_DIN 6

// RGB_LED
#define LIGHT_PIN 48
#define FAN_PIN   48

//speaker
#define I2S_LRC  16
#define I2S_BCLK 15
#define I2S_DIN  7

i2s_data_bit_width_t bps = I2S_DATA_BIT_WIDTH_16BIT;
i2s_mode_t mode = I2S_MODE_STD;
i2s_slot_mode_t slot = I2S_SLOT_MODE_MONO; // tts 输出声音单声道  //I2S_SLOT_MODE_STEREO; 双声道


 
const int inputSampleRate = 16000;  // sample rate in Hz
const int outputSampleRate = 24000;  // sample rate in Hz

I2SClass i2s_mic;
I2SClass i2s_speaker;

// Generated using the following command:
// python3 tools/gen_sr_commands.py "Turn on the light,Switch on the light;Turn off the light,Switch off the light,Go dark;Start fan;Stop fan"
enum {
  SR_CMD_TURN_ON_THE_LIGHT,
  SR_CMD_TURN_OFF_THE_LIGHT,
  SR_CMD_START_FAN,
  SR_CMD_STOP_FAN,
};
static const sr_cmd_t sr_commands[] = {
  {0, "Turn on the light", "TkN nN jc LiT"},
  {0, "Switch on the light", "SWgp nN jc LiT"},
  {1, "Turn off the light", "TkN eF jc LiT"},
  {1, "Switch off the light", "SWgp eF jc LiT"},
  {1, "Go dark", "Gb DnRK"},
  {2, "Start fan", "STnRT FaN"},
  {3, "Stop fan", "STnP FaN"},
};




void onSrEvent(sr_event_t event, int command_id, int phrase_id) {
  switch (event) {
    case SR_EVENT_WAKEWORD: Serial.println("WakeWord Detected!"); break;
    case SR_EVENT_WAKEWORD_CHANNEL:
      Serial.printf("WakeWord Channel %d Verified!\n", command_id);
      ESP_SR.setMode(SR_MODE_COMMAND);  // Switch to Command detection
      break;
    case SR_EVENT_TIMEOUT:
      Serial.println("Timeout Detected!");
      ESP_SR.setMode(SR_MODE_WAKEWORD);  // Switch back to WakeWord detection
      break;
    case SR_EVENT_COMMAND:
      Serial.printf("Command %d Detected! %s\n", command_id, sr_commands[phrase_id].str);
      switch (command_id) {
        case SR_CMD_TURN_ON_THE_LIGHT:    rgbLedWrite(LIGHT_PIN, 255,255,255); break;
        case SR_CMD_TURN_OFF_THE_LIGHT:   rgbLedWrite(LIGHT_PIN, 0,0,0);; break;
        case SR_CMD_START_FAN:          rgbLedWrite(LIGHT_PIN, 255,255,255); break;
        case SR_CMD_STOP_FAN:           rgbLedWrite(LIGHT_PIN, 0,0,0); break;
        default:                        Serial.println("Unknown Command!"); break;
      }
      ESP_SR.setMode(SR_MODE_COMMAND);  // Allow for more commands to be given, before timeout
      // ESP_SR.setMode(SR_MODE_WAKEWORD); // Switch back to WakeWord detection
      break;
    default: Serial.println("Unknown Event!"); break;
  }
}

void setup() {
  Serial.begin(115200);
  // if(psramInit()){
  //       Serial.println("\nPSRAM is correctly initialized");
  //       }else{
  //       Serial.println("PSRAM not available");
  //       }

  // log_d("Total heap: %d", ESP.getHeapSize());
  // log_d("Free heap: %d", ESP.getFreeHeap());
  // log_d("Total PSRAM: %d", ESP.getPsramSize());
  // log_d("Free PSRAM: %d", ESP.getFreePsram());

  // No pinMode, nothing in setup(),
  // just call "rgbLedWrite()".
  // pinMode(LIGHT_PIN, OUTPUT);
  //digitalWrite(LIGHT_PIN, HIGH);
  rgbLedWrite(LIGHT_PIN, 255,255,255);
  delay(3000);
  rgbLedWrite(LIGHT_PIN, 0,0,0);
  // pinMode(FAN_PIN, OUTPUT);
  // digitalWrite(FAN_PIN, LOW);


  i2s_speaker.setPins(I2S_BCLK, I2S_LRC, I2S_DIN);
  // start I2S at the sample rate with 16-bits per sample
  if (!i2s_speaker.begin(mode, outputSampleRate, bps, slot)) {
    Serial.println("Failed to initialize I2S Speaker!");
    // while (1);  // do nothing
  }
  if(!tts.begin(i2s_speaker)) {
        Serial.println("TTS初始化失败!");
        while(1);
    } 
  tts.speak("欢迎使用乐鑫语音合成");


  //esp-sr  speech recognition

  i2s_mic.setPins(I2S_PIN_BCK, I2S_PIN_WS, -1, I2S_PIN_DIN);
  i2s_mic.setTimeout(1000);
  i2s_mic.begin(I2S_MODE_STD, inputSampleRate, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);

  ESP_SR.onEvent(onSrEvent);
  ESP_SR.begin(i2s_mic, sr_commands, sizeof(sr_commands) / sizeof(sr_cmd_t), SR_CHANNELS_STEREO, SR_MODE_WAKEWORD);
}

void loop() {}

/*
int n_elements = 1000;
unsigned char * acc_data_all;

void setup(){
        delay(3000);
        Serial.begin(115200);
        acc_data_all = (unsigned char *) ps_malloc (n_elements * sizeof (unsigned char)); 
        if(psramInit()){
        Serial.println("\nPSRAM is correctly initialized");
        }else{
        Serial.println("PSRAM not available");
        }
}

void loop() {
  Serial.println(acc_data_all[1]);
  acc_data_all[1] = 'a';
  delay(5000);
}
  */