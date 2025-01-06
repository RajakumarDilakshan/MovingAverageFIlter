#include <driver/i2s.h>

// I2S0 - INMP441 Microphone
#define I2S0_WS 25
#define I2S0_SD 32
#define I2S0_SCK 33

// I2S1 - MAX98357 Amplifier
#define I2S1_WS 14 // LRC
#define I2S1_SD 22 // DIN
#define I2S1_SCK 26 // BCLK

#define I2S_PORT I2S_NUM_0
#define I2S_PORT_AMP I2S_NUM_1
#define bufferLen 64

int16_t sBuffer[bufferLen];
float filteredBuffer[bufferLen];
const int filterWindow = 9; // Moving average window size

size_t bytesWritten = 0;
size_t bytesIn=0;
int samples_read=0;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup I2S ...");

  delay(1000);

  // Initialize I2S for recording (I2S0 - INMP441)
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  // Initialize I2S for playback (I2S1 - MAX98357)
  i2s_install_playback();
  i2s_setpin_playback();
  i2s_start(I2S_PORT_AMP);

}

void loop() {
  
  esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen * sizeof(int16_t), &bytesIn, portMAX_DELAY);
  if (result == ESP_OK) {
    samples_read = bytesIn / sizeof(int16_t);
    if (samples_read > 0) {
      // Apply centered moving average filter
      for (int i = 0; i < samples_read; ++i) {
        
        filteredBuffer[i] = computeMovingAverage(sBuffer, i, samples_read, filterWindow);//centered average
        //filteredBuffer[i] = computeTriangularMA(sBuffer, i, samples_read, filterWindow);//triangular average
      }
      visualiseSignal();
    } 
    //comment the visualiseSignal() function when you call out_audio
    //out_audio();
  }
}
void out_audio(){
  esp_err_t ampWrite = i2s_write(I2S_PORT_AMP, (char *)filteredBuffer, bytesIn, &bytesWritten, portMAX_DELAY);
   if (ampWrite != ESP_OK || bytesWritten != bytesIn) {
          Serial.println("Error writing to I2S0 (amplifier)");
   }
}
void visualiseSignal(){
  // Print original and filtered data for Serial Plotter
      for (int i = 0; i < samples_read; ++i) {
        // Print both original and filtered values
        Serial.print(sBuffer[i]);     // Original signal
        Serial.print(",");            // Separator for plotter
        Serial.println(filteredBuffer[i]); // Filtered signal
      }
}
float computeMovingAverage(int16_t* data, int index, int totalSamples, int window) {
  int halfWindow = window / 2;
  float sum = 0;
  int count = 0;

  // Calculate centered average
  for (int i = index - halfWindow; i <= index + halfWindow; ++i) {
    if (i >= 0 && i < totalSamples) {
      sum += data[i];
      count++;
    }
  }
  return (count > 0) ? sum / count : 0;
}

float computeTriangularMA(int16_t* data, int index, int totalSamples, int window) {
  int halfWindow = window / 2;
  float sum = 0;
  float weightSum = 0;

  for (int i = -halfWindow; i <= halfWindow; ++i) {
    int currentIndex = index + i;
    if (currentIndex >= 0 && currentIndex < totalSamples) {
      int weight = window - abs(i); // Triangular weight
      sum += data[currentIndex] * weight;
      weightSum += weight;
    }
  }
  return (weightSum > 0) ? sum / weightSum : 0;
}

void i2s_install() {
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = 10000, // 10 kHz
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0,
      .dma_buf_count = 4,
      .dma_buf_len = bufferLen,
      .use_apll = false};

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S0_SCK,
      .ws_io_num = I2S0_WS,
      .data_out_num = -1,
      .data_in_num = I2S0_SD};

  i2s_set_pin(I2S_PORT, &pin_config);
}

void i2s_install_playback() {
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 10000,
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0,
      .dma_buf_count = 4,
      .dma_buf_len = bufferLen,
      .use_apll = false,
      .tx_desc_auto_clear = true};

  i2s_driver_install(I2S_PORT_AMP, &i2s_config, 0, NULL);
}

void i2s_setpin_playback() {
  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S1_SCK,
      .ws_io_num = I2S1_WS,
      .data_out_num = I2S1_SD,
      .data_in_num = -1};

  i2s_set_pin(I2S_PORT_AMP, &pin_config);
}
