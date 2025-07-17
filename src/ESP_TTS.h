#ifndef ESP_TTS_H
#define ESP_TTS_H

#include <Arduino.h>
#include "ESP_I2S.h"
#include "esp_tts.h"
#include "esp_tts_voice_template.h"
#include "esp_tts_voice_xiaole.h"

#include "esp_partition.h"
#include "esp_idf_version.h"
#include "esp_system.h"
#include "esp_tts_player.h"


class ESP_TTS {
public:
 
    
    ESP_TTS();
    ~ESP_TTS();
    
    bool begin(I2SClass &i2s);
    void end();
    
    bool speak(const char* text);
    bool speak(String text) { return speak(text.c_str()); }
    
   // void setVolume(uint8_t volume);  // 0-100
    void setSpeed(float speed);     // 0.5-2.0
    void setPitch(float pitch);     // 0.5-2.0
    
    bool isSpeaking();
    void waitUntilFinish();
    
private:
    esp_tts_handle_t* tts_handle;
    bool initialized;
    I2SClass *i2s;
};

#endif