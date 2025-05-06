#include <driver/i2s.h>

#define SAMPLE_RATE     16000  
#define BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define CHANNELS        1      
#define RECORD_TIME     10*3     // Duration in seconds
#define I2S_PORT        I2S_NUM_0
#define I2S_DMA_BUF_LEN 1024 
#define I2S_DMA_COUNT   8
#define MIC_ADC_CHANNEL ADC1_CHANNEL_7  

void initI2S() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_ADC_BUILT_IN | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = (i2s_bits_per_sample_t)I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, 
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = I2S_DMA_COUNT,
        .dma_buf_len = I2S_DMA_BUF_LEN,
        .use_apll = true,
        .tx_desc_auto_clear = false
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_adc_mode(ADC_UNIT_1, MIC_ADC_CHANNEL);
    i2s_adc_enable(I2S_PORT);
    adc1_config_channel_atten(MIC_ADC_CHANNEL, ADC_ATTEN_DB_0);
}

void logAudio() {
    uint8_t buffer[I2S_DMA_BUF_LEN];
    size_t bytesRead;

    // Corrected WAV Header Parameters
    uint32_t dataChunkSize = RECORD_TIME * SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8);
    uint32_t fileSize = dataChunkSize + 36 + 8;  // Ensure correct file size

    // WAV Header
    uint8_t wavHeader[44] = {
        'R', 'I', 'F', 'F',  
        (fileSize & 0xFF), (fileSize >> 8) & 0xFF, (fileSize >> 16) & 0xFF, (fileSize >> 24) & 0xFF, 
        'W', 'A', 'V', 'E',
        'f', 'm', 't', ' ',

        16, 0, 0, 0,  // PCM format length
        1, 0,  // Audio format (1 = PCM)
        CHANNELS, 0,  // Number of channels
        (SAMPLE_RATE & 0xFF), (SAMPLE_RATE >> 8) & 0xFF, (SAMPLE_RATE >> 16) & 0xFF, (SAMPLE_RATE >> 24) & 0xFF,  
        ((SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8)) & 0xFF), 
        ((SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8)) >> 8) & 0xFF,
        ((SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8)) >> 16) & 0xFF,
        ((SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8)) >> 24) & 0xFF,  
        ((CHANNELS * (BITS_PER_SAMPLE / 8)) & 0xFF), (((CHANNELS * (BITS_PER_SAMPLE / 8)) >> 8) & 0xFF),  
        BITS_PER_SAMPLE, 0,  // Bits per sample
        'd', 'a', 't', 'a',  // Data chunk identifier
        (dataChunkSize & 0xFF), (dataChunkSize >> 8) & 0xFF, (dataChunkSize >> 16) & 0xFF, (dataChunkSize >> 24) & 0xFF  
    };

    // Send WAV header first
    Serial.write(wavHeader, 44);

    unsigned long startTime = millis();
    while (millis() - startTime < RECORD_TIME * 1000) {  // RECORD_TIME in seconds
        // Read audio data from I2S and push directly to Serial
        i2s_read(I2S_PORT, buffer, I2S_DMA_BUF_LEN, &bytesRead, portMAX_DELAY);

        // You may need to convert the data to PCM format if required (optional step depending on your setup)
        // For now, send the raw I2S data as is
        Serial.write(buffer, bytesRead);  // Send audio data directly to serial
    }
}

void setup() {
    Serial.begin(921600);   // Ensure a high baud rate for audio streaming
    initI2S();              // Initialize I2S interface
    Serial.println("Starting audio capture...");
    logAudio();             // Start logging audio data
    i2s_adc_disable(I2S_PORT);  // Disable I2S when done
    // Serial.println("Audio capture complete.");
}

void loop() {
    // No continuous loop needed for one-shot capture
}
