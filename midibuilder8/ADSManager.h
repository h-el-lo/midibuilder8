#include <stdint.h>
#pragma once

#include <Adafruit_ADS1X15.h>
#include <Wire.h>

#define ADS_ALRT_MCU_PIN 48

#define ADS_EXPR_CHANNEL 0
#define ADS_PITCH_CHANNEL 1
#define ADS_SLIDER_CHANNEL 2
#define ADS_MUX4_CHANNEL 3


#define ADS_RAW_MAX 26400

class ADS_Manager {
private:
  Adafruit_ADS1115 _ads;
  uint16_t _selectedChannel = 255;  // 255 as "unset" sentinel

  static constexpr uint16_t _muxChannels[4] = {
    ADS1X15_REG_CONFIG_MUX_SINGLE_0,  ///< Single-ended A2
    ADS1X15_REG_CONFIG_MUX_SINGLE_1,  ///< Single-ended A2
    ADS1X15_REG_CONFIG_MUX_SINGLE_2,  ///< Single-ended A0
    ADS1X15_REG_CONFIG_MUX_SINGLE_3   ///< Single-ended A0
  };

  volatile bool _convReady = false;

public:
  void begin() {
    bool adsfound = false;

    // while (!adsfound) {
    //   continue;
    // }

    if (_ads.begin(0x48, &Wire)) {
      Serial.println("ADS found on 0x48.");
      adsfound = true;
      delay(500);
    } else if (_ads.begin(0x49, &Wire)) {
      Serial.println("ADS found on 0x49.");
      adsfound = true;
      delay(500);
    } else if (_ads.begin(0x4A, &Wire)) {
      Serial.println("ADS found on 0x4A.");
      adsfound = true;
      delay(500);
    } else if (_ads.begin(0x4B, &Wire)) {
      Serial.println("ADS found on 0x4B.");
      adsfound = true;
      delay(500);
    } else {
      Serial.println("ADS not found, retrying.");
      delay(500);
    }

    pinMode(ADS_ALRT_MCU_PIN, INPUT_PULLUP);

    _ads.setGain(GAIN_ONE);
    _ads.setDataRate(RATE_ADS1115_860SPS);
    Serial.println("ADS1115 Ready");
  };

  void onConvReady() {
    _convReady = true;  // called from ISR
  }

  //   Switch to channel and begin conversion
  void selectChannel(uint8_t channel) {
    if (_selectedChannel != channel) {
      _selectedChannel = channel;
      _convReady = false;
      if (_selectedChannel > 4) {
        // Prevents any other out of range error
        Serial.println("Selected ADS channel out of range");
        return;
      } else {
        _ads.startADCReading(_muxChannels[_selectedChannel], /*continuous*/ true);
      }
    }
  }

  int16_t read() {
    if (!_convReady) {
      return -255;
    } else {
      return constrain(_ads.getLastConversionResults(), 0, ADS_RAW_MAX);
    }
  };

  // uint16_t read() {
  //   if (!_ads.conversionComplete()) {
  //     return -1;
  //   } else {
  //     return constrain(_ads.getLastConversionResults(), 0, ADS_RAW_MAX);
  //   }
  // };



  // Blocking read — waits for conversion (use sparingly)
  uint16_t readBlocking(uint8_t channel) {
    selectChannel(channel);
    while (!_convReady)
      ;
    return read();
  };

  bool isReady() {
    return _convReady;
  }
};

// Declare an ADS Manager instance
extern ADS_Manager ADSManager;
// bool ads.conversionComplete();