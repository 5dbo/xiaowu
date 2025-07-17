#include "ESP_TTS.h"


//#include "esp_tts_voice_xiaole.h"  // 中文语音
//#include "esp_tts_voice_espeak.h" // 英文语音
  

ESP_TTS::ESP_TTS() : tts_handle(nullptr), initialized(false) {
}

ESP_TTS::~ESP_TTS() {
    end();
}

bool ESP_TTS::begin(I2SClass &_i2s) {
    if(initialized) end();

    i2s = &_i2s;
    
  /*** 1. create esp tts handle ***/
    // initial voice set from separate voice data partition

    const esp_partition_t* part=esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "voice_data");
    if (part==NULL) { 
        Serial.printf("Couldn't find voice data partition!\n"); 
        return 0;
    } else {
        printf("voice_data paration size:%d\n", part->size);
    }
    const void* voicedata;
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
         printf("ESP_IDF_VERSION:%d\n", ESP_IDF_VERSION);
        esp_partition_mmap_handle_t mmap;
        esp_err_t err=esp_partition_mmap(part, 0, part->size, ESP_PARTITION_MMAP_DATA, &voicedata, &mmap);
    #else
        spi_flash_mmap_handle_t mmap;
        esp_err_t err=esp_partition_mmap(part, 0, part->size, SPI_FLASH_MMAP_DATA, &voicedata, &mmap);
    #endif
    if (err != ESP_OK) {
        printf("Couldn't map voice data partition!\n"); 
        return 0;
    }
    //  esp_tts_voice_template
    esp_tts_voice_t *voice=esp_tts_voice_set_init(&esp_tts_voice_xiaole, (int16_t*)voicedata); 
    

    //esp_tts_handle_t *tts_handle= ( void**)esp_tts_create(voice);
 
    tts_handle =  (esp_tts_handle_t *)esp_tts_create(voice);

    if(!tts_handle) return false;
    
 
    initialized = true;
    return true;
}

void ESP_TTS::end() {
    if(tts_handle) {
       
        esp_tts_destroy(tts_handle);
        tts_handle = nullptr;
    }
    initialized = false;
}
//https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2s.html
/*** 2. play prompt text ***/
bool ESP_TTS::speak(const char* text) {
    if(!initialized || !text) return false;

    //  const char *prompt1  ="欢迎使用"; // "欢迎使用乐鑫语音合成";  
    printf("%s\n", text);
    
    if (esp_tts_parse_chinese(tts_handle, text)) {
        
            int len[1]={0};
            do {
                short *pcm_data=esp_tts_stream_play(tts_handle, len, 1);  // range:0~5, 0: the slowest speed, 5: the fastest speech   
               //pcm_data：：voice raw data 非PCM 数据
              // printf("data:%d \n", len[0]);
               
               i2s->write((uint8_t *)pcm_data, len[0] * 2);
               
               // i2s->playWAV((uint8_t *)pcm_data, (size_t )len);  非PCM 数据，不可用.
            } while(len[0]>0); 
    }
    esp_tts_stream_reset(tts_handle); // 要释放
    return esp_tts_parse_chinese(tts_handle, text) == ESP_OK;
}



