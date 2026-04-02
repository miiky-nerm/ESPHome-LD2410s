#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace ld2410s {

class LD2410S : public Component, public uart::UARTDevice {
 public:
  binary_sensor::BinarySensor *has_target{nullptr};
  binary_sensor::BinarySensor *last_command_success{nullptr};
  sensor::Sensor *target_distance{nullptr};
  text_sensor::TextSensor *output_mode{nullptr};

  uint32_t lastPeriodicMillis{0};

  LD2410S(uart::UARTComponent *parent) : UARTDevice(parent) {}

  void set_has_target(binary_sensor::BinarySensor *s) { has_target = s; }
  void set_last_command_success(binary_sensor::BinarySensor *s) { last_command_success = s; }
  void set_target_distance(sensor::Sensor *s) { target_distance = s; }
  void set_output_mode(text_sensor::TextSensor *s) { output_mode = s; }

  int twoByteToInt(char firstByte, char secondByte) {
    return (int16_t)(secondByte << 8) + firstByte;
  }

  void handlePeriodicData(char *buffer, int len) {
    if (len < 5) return;

    if (buffer[0] != 0xF4 && buffer[0] != 0x6E) return;
    if (buffer[len - 1] != 0x62 && buffer[len - 1] != 0xF5) return;

    char dataType = buffer[0];
    char stateByte = buffer[1];

    if (has_target != nullptr)
      has_target->publish_state(stateByte != 0x00 && stateByte != 0x01);

    uint32_t now = millis();
    if (now - lastPeriodicMillis < 1000) return;
    lastPeriodicMillis = now;

    int newDistance = twoByteToInt(buffer[2], buffer[3]);
    if (target_distance != nullptr)
      target_distance->publish_state(newDistance);

    if (output_mode != nullptr) {
      if (dataType == 0x6E)
        output_mode->publish_state("Simplified mode");
      else if (dataType == 0xF4)
        output_mode->publish_state("Normal mode");
    }
  }

  void handleACKData(char *buffer, int len) {
    if (len < 10) return;

    if (buffer[0] != 0xFD || buffer[1] != 0xFC || buffer[2] != 0xFB || buffer[3] != 0xFA)
      return;

    if (buffer[7] != 0x01) return;

    if (twoByteToInt(buffer[8], buffer[9]) != 0x00) {
      if (last_command_success != nullptr)
        last_command_success->publish_state(false);
      return;
    }

    if (last_command_success != nullptr)
      last_command_success->publish_state(true);
  }

  void readline(int readch, char *buffer, int len) {
    static int pos = 0;

    if (readch >= 0) {
      if (pos < len - 1) {
        buffer[pos++] = readch;
        buffer[pos] = 0;
      } else {
        pos = 0;
      }

      if (pos >= 4) {
        if (buffer[pos - 4] == 0xF8 && buffer[pos - 3] == 0xF7 &&
            buffer[pos - 2] == 0xF6 && buffer[pos - 1] == 0xF5) {
          handlePeriodicData(buffer, pos);
          pos = 0;
        } else if (buffer[pos - 4] == 0x04 && buffer[pos - 3] == 0x03 &&
                   buffer[pos - 2] == 0x02 && buffer[pos - 1] == 0x01) {
          handleACKData(buffer, pos);
          pos = 0;
        } else if (buffer[pos - 1] == 0x62) {
          handlePeriodicData(buffer, pos);
          pos = 0;
        }
      }
    }
  }

  void loop() override {
    const int max_len = 80;
    static char buffer[max_len];

    while (available()) {
      readline(read(), buffer, max_len);
    }
  }
};

}  // namespace ld2410s
}  // namespace esphome
