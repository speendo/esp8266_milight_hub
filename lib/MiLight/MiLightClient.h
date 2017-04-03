#include <Arduino.h>
#include <MiLightRadio.h>
#include <MiLightRadioPL1167_LT8900.h>
#include <PL1167_nRF24.h>
#include <RF24.h>
#include <MiLightButtons.h>
#include <RadioStack.h>
#include <Settings.h>

#ifndef _MILIGHTCLIENT_H
#define _MILIGHTCLIENT_H

// #define DEBUG_PRINTF

#define MILIGHT_DEFAULT_RESEND_COUNT 10

class MiLightClient {
  public:
  MiLightClient(byte byCSPin, byte byResetPin, byte byPktFlag, eRadioInterfaceType InterfaceType)
  :
  resendCount(MILIGHT_DEFAULT_RESEND_COUNT),
  currentRadio(NULL),
  numRadios(MiLightRadioConfig::NUM_CONFIGS)

  {
    radios = new RadioStack*[numRadios];

    for (size_t i = 0; i < numRadios; i++)
    {
      if(InterfaceType == LT1167_PL8900)
      {
        radios[i] = new RadioStack(byCSPin, byResetPin, byPktFlag, *MiLightRadioConfig::ALL_CONFIGS[i]);
      }
      else if(InterfaceType == nRF24)
      {
        pRF = new RF24(byCSPin, byPktFlag);
        radios[i] = new RadioStack(*pRF, *MiLightRadioConfig::ALL_CONFIGS[i]);
      }
    }

    currentRadio = radios[0];

    currentRadio->getRadioInterface()->configure();
  }


    ~MiLightClient() {
      delete pRF;
      delete[] radios;
    }

    void begin() {
      for (size_t i = 0; i < numRadios; i++) {
        radios[i]->getRadioInterface()->begin();
      }
    }


    void prepare(MiLightRadioConfig& config, const uint16_t deviceId = -1, const uint8_t groupId = -1);

    void setResendCount(const unsigned int resendCount);
    bool available();
    void read(uint8_t packet[]);
    void write(uint8_t packet[]);

    // Common methods
    void updateStatus(MiLightStatus status);
    void updateStatus(MiLightStatus status, uint8_t groupId);
    void pair();
    void unpair();
    void command(uint8_t command, uint8_t arg);
    void updateMode(uint8_t mode);
    void nextMode();
    void previousMode();
    void modeSpeedDown();
    void modeSpeedUp();

    // RGBW methods
    void updateHue(const uint16_t hue);
    void updateBrightness(const uint8_t brightness);
    void updateColorWhite();
    void updateColorRaw(const uint8_t color);

    // CCT methods
    void updateTemperature(const uint8_t colorTemperature);
    void decreaseTemperature();
    void increaseTemperature();
    void increaseBrightness();
    void decreaseBrightness();

    void updateSaturation(const uint8_t saturation);

    void formatPacket(uint8_t* packet, char* buffer);


  protected:

    RF24 *pRF;
    RadioStack** radios;
    RadioStack* currentRadio;
    PacketFormatter* formatter;
    const size_t numRadios;

    unsigned int resendCount;

    MiLightRadioInterface* switchRadio(const MiLightRadioType type);

    void flushPacket();
};

#endif
