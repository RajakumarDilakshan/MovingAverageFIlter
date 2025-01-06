# Moving Average Filter for I2S Audio Processing on ESP32

This project implements a moving average filter on audio data captured from an INMP441 microphone and plays back the filtered audio through a MAX98357 amplifier. The project uses the ESP32 microcontroller to handle I2S audio input and output.

## Features
- Captures audio data using the INMP441 I2S microphone.
- Applies a centered moving average filter or triangular moving average filter to the audio samples.
- Outputs filtered audio through the MAX98357 I2S amplifier.
- Visualizes raw and filtered audio signals in the Serial Plotter.

## Hardware Requirements
- **ESP32** microcontroller.
- **INMP441 I2S microphone** 
- **MAX98357 I2S amplifier** 

## Software Requirements
- **Arduino IDE** with the ESP32 board package installed.

## Pin Configuration
### I2S0 - INMP441 Microphone
- `WS`: GPIO 25
- `SD`: GPIO 32
- `SCK`: GPIO 33

### I2S1 - MAX98357 Amplifier
- `WS (LRC)`: GPIO 14
- `SD (DIN)`: GPIO 22
- `SCK (BCLK)`: GPIO 26

## Installation
1. Clone this repository to your local machine.
2. Open the code in the Arduino IDE.
3. Install the required libraries and dependencies for I2S.
4. Configure the pins and sample rate as per your hardware setup if different from the default.
5. Upload the code to your ESP32 board.

## How It Works
1. **Recording Audio**: The ESP32 uses I2S0 to capture audio data from the INMP441 microphone at a sample rate of 10 kHz.
2. **Filtering Audio**: A moving average filter is applied to the captured audio data to smooth the signal:
   - **Centered Moving Average**: Averages data points symmetrically around the current sample.
   - **Triangular Moving Average**: Applies weighted averaging, giving higher weight to the center samples.
3. **Playback Audio**: The filtered audio is sent to the MAX98357 amplifier for playback.
4. **Visualization**: Both raw and filtered audio signals are printed to the Serial Plotter for visualization.

## Functions
- `computeMovingAverage`: Applies a centered moving average filter.
- `computeTriangularMA`: Applies a triangular moving average filter.
- `visualiseSignal`: Sends raw and filtered audio data to the Serial Plotter for real-time visualization.
- `out_audio`: Sends filtered audio to the MAX98357 amplifier.

## Customization
- **Filter Type**: Uncomment the desired filter type in the `loop()` function:
  ```cpp
  // filteredBuffer[i] = computeMovingAverage(sBuffer, i, samples_read, filterWindow);
  // filteredBuffer[i] = computeTriangularMA(sBuffer, i, samples_read, filterWindow);
  ```
- **Filter Window Size**: Adjust the size of the moving average window by modifying `filterWindow`:
  ```cpp
  const int filterWindow = 9; // Moving average window size
  ```
- **Sample Rate**: Change the sample rate in `i2s_install()` and `i2s_install_playback()`.

## Serial Plotter
For debugging or visualization, use the Serial Plotter in the Arduino IDE. It will display both the raw and filtered signals:
- Blue represents raw audio data.
- Red color represents filtered audio data.

  ![RawSoundVsFiltered](https://github.com/user-attachments/assets/294310ab-9fbe-4af1-a5ac-11861f192df2)



