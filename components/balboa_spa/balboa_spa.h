#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/number/number.h"
#include "esphome/components/button/button.h"

#include <CircularBuffer.hpp>
namespace esphome
{
  namespace balboa_spa
  {
    // Forward declarations
    class BalboaSpa;

    class BalboaSpaSwitch : public switch_::Switch, public Component
    {
    public:
      void write_state(bool state) override;
      void setup() override {}
      void set_command_code(uint8_t code) { command_code_ = code; }
      void set_parent(BalboaSpa *parent) { parent_ = parent; }

    protected:
      uint8_t command_code_{0x00};
      BalboaSpa *parent_{nullptr};
    };

    class BalboaSpaTemperature : public number::Number, public Component
    {
    public:
      void control(float value) override;
      void set_parent(BalboaSpa *parent) { parent_ = parent; }

    protected:
      BalboaSpa *parent_{nullptr};
    };

    class BalboaSpaButton : public button::Button, public Component
    {
    public:
      void set_command_code(uint8_t code) { command_code_ = code; }
      void set_parent(BalboaSpa *parent) { parent_ = parent; }
      void press_action() override;

    protected:
      uint8_t command_code_{0x00};
      BalboaSpa *parent_{nullptr};
    };

    class BalboaSpa : public Component, public uart::UARTDevice
    {
    public:
      void setup() override;
      void loop() override;
      void ID_ack();
      void ID_request();
      void decodeSettings();
      void decodeState();
      void decodeFilterSettings();
      void decodeFault();
      void rs485_send();
      void print_msg(CircularBuffer<uint8_t, 35> &data);
      void on_target_temperature_update(float value);
      float get_setup_priority() const override { return setup_priority::LATE; }
      CircularBuffer<uint8_t, 35> Q_in;
      CircularBuffer<uint8_t, 35> Q_out;

      // Command setter
      void set_command(uint8_t cmd) { command_to_send_ = cmd; }

      // Switch setters
      void set_light_switch(BalboaSpaSwitch *sw)
      {
        light_switch_ = sw;
        light_switch_->set_command_code(0x11); // Light toggle command
        light_switch_->set_parent(this);
      }

      void set_heat_mode_switch(BalboaSpaSwitch *sw)
      {
        heat_mode_switch_ = sw;
        heat_mode_switch_->set_command_code(0x51); // Heat mode toggle command
        heat_mode_switch_->set_parent(this);
      }

      void set_jet1_button(BalboaSpaButton *btn)
      {
        jet1_button_ = btn;
        jet1_button_->set_command_code(0x04); // Jet1 toggle command
        jet1_button_->set_parent(this);
      }

      void set_jet2_button(BalboaSpaButton *btn)
      {
        jet2_button_ = btn;
        jet2_button_->set_command_code(0x05); // Jet2 toggle command
        jet2_button_->set_parent(this);
      }

      void set_soak_button(BalboaSpaButton *btn)
      {
        soak_button_ = btn;
        soak_button_->set_command_code(0x1D); // Soak command
        soak_button_->set_parent(this);
      }

      void set_current_temperature_sensor(sensor::Sensor *sens) { current_temp_sensor = sens; }
      void set_target_temperature_sensor(sensor::Sensor *sens)
      {
        target_temp_sensor = sens;
        target_temp_sensor->add_on_state_callback([this](float value)
                                                  { this->on_target_temperature_update(value); });
      }
      void set_jet1_sensor(sensor::Sensor *sens) { jet1_sensor = sens; }
      void set_jet2_sensor(sensor::Sensor *sens) { jet2_sensor = sens; }
      void set_light_sensor(sensor::Sensor *sens) { light_sensor = sens; }
      void set_restmode_sensor(sensor::Sensor *sens) { restmode_sensor = sens; }
      void set_highrange_sensor(sensor::Sensor *sens) { highrange_sensor = sens; }
      void set_hour_sensor(sensor::Sensor *sens) { hour_sensor = sens; }
      void set_minute_sensor(sensor::Sensor *sens) { minute_sensor = sens; }
      void set_heater_sensor(sensor::Sensor *sens) { heater_sensor = sens; }
      void set_filt1hour_sensor(sensor::Sensor *sens) { filt1hour_sensor = sens; }
      void set_filt1minute_sensor(sensor::Sensor *sens) { filt1minute_sensor = sens; }
      void set_filt1durhour_sensor(sensor::Sensor *sens) { filt1durhour_sensor = sens; }
      void set_filt1durminute_sensor(sensor::Sensor *sens) { filt1durminute_sensor = sens; }
      void set_filt2enable_sensor(sensor::Sensor *sens) { filt2enable_sensor = sens; }
      void set_filt2hour_sensor(sensor::Sensor *sens) { filt2hour_sensor = sens; }
      void set_filt2minute_sensor(sensor::Sensor *sens) { filt2minute_sensor = sens; }
      void set_filt2durhour_sensor(sensor::Sensor *sens) { filt2durhour_sensor = sens; }
      void set_filt2durminute_sensor(sensor::Sensor *sens) { filt2durminute_sensor = sens; }

      void set_target_temperature(float temp)
      {
        settemp = temp;
        command_to_send_ = 0xff; // Temperature change command
      }

      void set_temperature_control(BalboaSpaTemperature *temp_ctrl)
      {
        temperature_control_ = temp_ctrl;
        temperature_control_->set_parent(this);
      }

    protected:
      // Sensor pointers
      sensor::Sensor *current_temp_sensor{nullptr};
      sensor::Sensor *target_temp_sensor{nullptr};
      sensor::Sensor *jet1_sensor{nullptr};
      sensor::Sensor *jet2_sensor{nullptr};
      sensor::Sensor *light_sensor{nullptr};
      sensor::Sensor *restmode_sensor{nullptr};
      sensor::Sensor *highrange_sensor{nullptr};
      sensor::Sensor *hour_sensor{nullptr};
      sensor::Sensor *minute_sensor{nullptr};
      sensor::Sensor *heater_sensor{nullptr};
      sensor::Sensor *filt1hour_sensor{nullptr};
      sensor::Sensor *filt1minute_sensor{nullptr};
      sensor::Sensor *filt1durhour_sensor{nullptr};
      sensor::Sensor *filt1durminute_sensor{nullptr};
      sensor::Sensor *filt2enable_sensor{nullptr};
      sensor::Sensor *filt2hour_sensor{nullptr};
      sensor::Sensor *filt2minute_sensor{nullptr};
      sensor::Sensor *filt2durhour_sensor{nullptr};
      sensor::Sensor *filt2durminute_sensor{nullptr};

      // Switch members
      BalboaSpaSwitch *light_switch_{nullptr};
      BalboaSpaSwitch *heat_mode_switch_{nullptr};
      BalboaSpaButton *jet1_button_{nullptr};
      BalboaSpaButton *jet2_button_{nullptr};
      BalboaSpaButton *soak_button_{nullptr};

      // Command variable
      uint8_t command_to_send_{0x00};
      float settemp{0.0};
      BalboaSpaTemperature *temperature_control_{nullptr};
    };
  }
}