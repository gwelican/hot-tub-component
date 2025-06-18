#include <CircularBuffer.hpp>

#include <Arduino.h>
#include <string>
#include "balboa_spa.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h" // Include for millis()
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include <WString.h>

namespace esphome
{
    namespace balboa_spa
    {

        static const char *const TAG = "balboa_spa";
        static float latest_controller_target_temp = NAN, last_controller_target_temp = NAN;
        static float latest_manual_target_temp = NAN, last_manual_target_temp = NAN;
        static float latest_current_temp = NAN, last_current_temp = NAN;
        static float latest_hour = NAN, last_hour = NAN;
        static float latest_minute = NAN, last_minute = NAN;
        static float latest_restmode = NAN, last_restmode = NAN;
        static float latest_heater = NAN, last_heater = NAN;
        static float latest_highrange = NAN, last_highrange = NAN;
        static float latest_jet1 = NAN, last_jet1 = NAN;
        static float latest_jet2 = NAN, last_jet2 = NAN;
        static float latest_light = NAN, last_light = NAN;
        static float latest_light_switch = NAN, last_light_switch = NAN;
        // Filter sensors
        static float latest_filt1hour = NAN, last_filt1hour = NAN;
        static float latest_filt1minute = NAN, last_filt1minute = NAN;
        static float latest_filt1durhour = NAN, last_filt1durhour = NAN;
        static float latest_filt1durminute = NAN, last_filt1durminute = NAN;
        static float latest_filt2enable = NAN, last_filt2enable = NAN;
        static float latest_filt2hour = NAN, last_filt2hour = NAN;
        static float latest_filt2minute = NAN, last_filt2minute = NAN;
        static float latest_filt2durhour = NAN, last_filt2durhour = NAN;
        static float latest_filt2durminute = NAN, last_filt2durminute = NAN;
        uint8_t last_state_crc = 0x00;
        uint8_t command_to_send_ = 0x00;
        
        uint8_t settemp = 0x00;
        uint8_t sethour = 0x00;
        uint8_t setminute = 0x00;
        uint8_t id = 0x00;
        unsigned long lastrx = 0;

        uint8_t setfilt1Hour = 0x00;
        uint8_t setfilt1Minute = 0x00;
        uint8_t setfilt1DurationHour = 0x00;
        uint8_t setfilt1DurationMinute = 0x00;
        uint8_t setfilt2HourEnable = 0x00;
        uint8_t setfilt2Minute = 0x00;
        uint8_t setfilt2DurationHour = 0x00;
        uint8_t setfilt2DurationMinute = 0x00;

        char have_config = 0;            // stages: 0-> want it; 1-> requested it; 2-> got it; 3-> further processed it
        char have_faultlog = 0;          // stages: 0-> want it; 1-> requested it; 2-> got it; 3-> further processed it
        char have_filtersettings = 0;    // stages: 0-> want it; 1-> requested it; 2-> got it; 3-> further processed it
        char faultlog_minutes = 0;       // temp logic so we only get the fault log once per 5 minutes
        char filtersettings_minutes = 0; // temp logic so we only get the filter settings once per 5 minutes

        float last_proper_temp = 0.0;
        
        void BalboaSpa::setup()
        {
            Q_out.clear();
            Q_in.clear();
            ESP_LOGCONFIG(TAG, "Setting up Balboa Spa Component...");
        }
        struct {
            uint8_t jet1 : 2;
            uint8_t jet2 : 2;
            uint8_t light : 1;
            uint8_t restmode : 1;
            uint8_t highrange : 1;
            uint8_t heater : 1;
            uint8_t hour : 5;
            uint8_t minutes : 6;
        } SpaState;
        struct {
            uint8_t pump1 : 2;
            uint8_t pump2 : 2;
            uint8_t light1 : 1;
            uint8_t temp_scale : 1; // 0 -> Farenheit, 1-> Celcius
        } SpaConfig;
        struct {
            uint8_t totEntry : 5;
            uint8_t currEntry : 5;
            uint8_t faultCode : 6;
            std::string faultMessage;
            uint8_t daysAgo : 8;
            uint8_t hour : 5;
            uint8_t minutes : 6;
        } SpaFaultLog;
        struct {
            uint8_t filt1Hour : 5;
            uint8_t filt1Minute : 6;
            uint8_t filt1DurationHour : 5;
            uint8_t filt1DurationMinute : 6;
            uint8_t filt2Enable : 1;
            uint8_t filt2Hour : 5;
            uint8_t filt2Minute : 6;
            uint8_t filt2DurationHour : 5;
            uint8_t filt2DurationMinute : 6;
        } SpaFilterSettings;

        sensor::Sensor *current_temp_sensor = new sensor::Sensor();
        sensor::Sensor *jet1_sensor = new sensor::Sensor();
        sensor::Sensor *jet2_sensor = new sensor::Sensor();
        sensor::Sensor *light_sensor = new sensor::Sensor();
        sensor::Sensor *restmode_sensor = new sensor::Sensor();
        sensor::Sensor *highrange_sensor = new sensor::Sensor();
        sensor::Sensor *hour_sensor = new sensor::Sensor();
        sensor::Sensor *minute_sensor = new sensor::Sensor();
        sensor::Sensor *heater_sensor = new sensor::Sensor();
        sensor::Sensor *filt1hour_sensor = new sensor::Sensor();
        sensor::Sensor *filt1minute_sensor = new sensor::Sensor();
        sensor::Sensor *filt1durhour_sensor = new sensor::Sensor();
        sensor::Sensor *filt1durminute_sensor = new sensor::Sensor();
        sensor::Sensor *filt2enable_sensor = new sensor::Sensor();
        sensor::Sensor *filt2hour_sensor = new sensor::Sensor();
        sensor::Sensor *filt2minute_sensor = new sensor::Sensor();
        sensor::Sensor *filt2durhour_sensor = new sensor::Sensor();
        sensor::Sensor *filt2durminute_sensor = new sensor::Sensor();

        uint8_t crc8(CircularBuffer<uint8_t, 35> &data)
        {
            unsigned long crc;
            int i, bit;
            uint8_t length = data.size();

            crc = 0x02;
            for (int i = 0; i < length; i++)
            {
                crc ^= data[i];
                for (bit = 0; bit < 8; bit++)
                {
                    if ((crc & 0x80) != 0)
                    {
                        crc <<= 1;
                        crc ^= 0x7;
                    }
                    else
                    {
                        crc <<= 1;
                    }
                }
            }

            return crc ^ 0x02;
        }

        void BalboaSpa::print_msg(CircularBuffer<uint8_t, 35> &data)
        {
            String s;
            for (int i = 0; i < data.size(); i++)
            {
                int x = Q_in[i];
                if (x < 0x0A)
                    s += "0";
                s += String(x, 16);
                s += " ";
            }
            yield();
        }

        void BalboaSpa::decodeSettings()
        {
            SpaConfig.pump1 = Q_in[5] & 0x03;
            SpaConfig.pump2 = (Q_in[5] & 0x0C) >> 2;
            SpaConfig.light1 = (Q_in[7] & 0x03);
            SpaConfig.temp_scale = Q_in[3] & 0x00; // Read temperature scale - 0 -> Farenheit, 1-> Celcius
            have_config = 2;
        }
        void BalboaSpa::decodeState()
        {
            // target temperature
            float target_temperature = Q_in[25];
            float current_temperature = 0.0;
            if (Q_in[7] != 0xFF)
            {
                current_temperature = Q_in[7];
                if (last_proper_temp > 0)
                {
                    if ((current_temperature > last_proper_temp * 1.2) || (current_temperature < last_proper_temp * 0.8))
                        current_temperature = last_proper_temp; // remove spurious readings greater or less than 20% away from previous read
                }
                last_proper_temp = current_temperature;
            }
            // clock
            SpaState.hour = Q_in[8];
            SpaState.minutes = Q_in[9];
            sethour = SpaState.hour;
            setminute = SpaState.minutes;
            // restmode
            uint8_t rest_mode = Q_in[10];
            uint8_t heater_state = bitRead(Q_in[15], 5);
            uint8_t jet1_state = Q_in[16] & 0x03;
            uint8_t jet2_state = (Q_in[16] & 0x0C) >> 2;
            uint8_t light_state = Q_in[19] & 0x03;
            // Store latest values
            latest_controller_target_temp = target_temperature;
            if (temperature_control_) latest_manual_target_temp = target_temperature;
            latest_current_temp = current_temperature;
            latest_hour = SpaState.hour;
            latest_minute = SpaState.minutes;
            latest_restmode = (rest_mode == 0x01 ? 1 : 0);
            latest_heater = heater_state;
            latest_highrange = bitRead(Q_in[15], 2);
            latest_jet1 = jet1_state;
            latest_jet2 = jet2_state;
            latest_light = light_state;
            latest_light_switch = (light_state == 1 ? 1 : 0);
            last_state_crc = Q_in[Q_in[1]];
        }
        void BalboaSpa::decodeFilterSettings()
        {
            SpaFilterSettings.filt1Hour = Q_in[5];
            SpaFilterSettings.filt1Minute = Q_in[6];
            SpaFilterSettings.filt1DurationHour = Q_in[7];
            SpaFilterSettings.filt1DurationMinute = Q_in[8];
            SpaFilterSettings.filt2Enable = bitRead(Q_in[9], 7); // check
            SpaFilterSettings.filt2Hour = Q_in[9];               // ^ (SpaFilterSettings.filt2Enable << 7); // check (idk why XOR it?)
            bitWrite(SpaFilterSettings.filt2Hour, 7, 0);         // Discard Enable bit from time value
            SpaFilterSettings.filt2Minute = Q_in[10];
            SpaFilterSettings.filt2DurationHour = Q_in[11];
            SpaFilterSettings.filt2DurationMinute = Q_in[12];
            // Set data for setting settings
            setfilt1Hour = SpaFilterSettings.filt1Hour;
            setfilt1Minute = SpaFilterSettings.filt1Minute;
            setfilt1DurationHour = SpaFilterSettings.filt1DurationHour;
            setfilt1DurationMinute = SpaFilterSettings.filt1DurationMinute;
            setfilt2HourEnable = SpaFilterSettings.filt2Hour ^ (SpaFilterSettings.filt2Enable << 7);
            setfilt2Minute = SpaFilterSettings.filt2Minute;
            setfilt2DurationHour = SpaFilterSettings.filt2DurationHour;
            setfilt2DurationMinute = SpaFilterSettings.filt2DurationMinute;
            // Filter 1 time conversion for log
            //  if (SpaFilterSettings.filt1Hour < 10) s = "0"; else s = "";
            //  s = String(SpaFilterSettings.filt1Hour) + ":";
            //  if (SpaFilterSettings.filt1Minute < 10) s += "0";
            //  s += String(SpaFilterSettings.filt1Minute);

            // if (SpaFilterSettings.filt1DurationHour < 10) d = "0"; else d = "";
            // d = String(SpaFilterSettings.filt1DurationHour) + ":";
            // if (SpaFilterSettings.filt1DurationMinute < 10) d += "0";
            // d += String(SpaFilterSettings.filt1DurationMinute);
            // String payld;
            // payld = "{\"start\":\""+s+"\",\"duration\":\""+d+"\"}";
            // ESP_LOGD("Spa/filter1/state", "Value: ", payld.c_str());

            // Filter 2 time conversion for log
            //  if (SpaFilterSettings.filt2Hour < 10) s = "0"; else s = "";
            //  s += String(SpaFilterSettings.filt2Hour) + ":";
            //  if (SpaFilterSettings.filt2Minute < 10) s += "0";
            //  s += String(SpaFilterSettings.filt2Minute);

            // if (SpaFilterSettings.filt2DurationHour < 10) d = "0"; else d = "";
            // d += String(SpaFilterSettings.filt2DurationHour) + ":";
            // if (SpaFilterSettings.filt2DurationMinute < 10) d += "0";
            // d += String(SpaFilterSettings.filt2DurationMinute);
            // if ((int)(SpaFilterSettings.filt2Enable) == 1) ESP_LOGD("Spa/filter2_enabled/state", "ON"); else ESP_LOGD("Spa/filter2_enabled/state", "OFF");

            // payld = "{\"start\":\""+s+"\",\"duration\":\""+d+"\"}";
            // ESP_LOGD("Spa/filter2/state", "Value: ", payld.c_str());
            // Store latest values
            latest_filt1hour = SpaFilterSettings.filt1Hour;
            latest_filt1minute = SpaFilterSettings.filt1Minute;
            latest_filt1durhour = SpaFilterSettings.filt1DurationHour;
            latest_filt1durminute = SpaFilterSettings.filt1DurationMinute;
            latest_filt2enable = SpaFilterSettings.filt2Enable;
            latest_filt2hour = SpaFilterSettings.filt2Hour;
            latest_filt2minute = SpaFilterSettings.filt2Minute;
            latest_filt2durhour = SpaFilterSettings.filt2DurationHour;
            latest_filt2durminute = SpaFilterSettings.filt2DurationMinute;
            have_filtersettings = 2;
            // ESP_LOGD("Spa/debug/have_filtersettings", "have the filter settings, #2");
        }
        void BalboaSpa::decodeFault()
        {
            SpaFaultLog.totEntry = Q_in[5];
            SpaFaultLog.currEntry = Q_in[6];
            SpaFaultLog.faultCode = Q_in[7];
            switch (SpaFaultLog.faultCode)
            { // this is a inelegant way to do it, a lookup table would be better
            case 15:
                SpaFaultLog.faultMessage = "Sensors are out of sync";
                break;
            case 16:
                SpaFaultLog.faultMessage = "The water flow is low";
                break;
            case 17:
                SpaFaultLog.faultMessage = "The water flow has failed";
                break;
            case 18:
                SpaFaultLog.faultMessage = "The settings have been reset";
                break;
            case 19:
                SpaFaultLog.faultMessage = "Priming Mode";
                break;
            case 20:
                SpaFaultLog.faultMessage = "The clock has failed";
                break;
            case 21:
                SpaFaultLog.faultMessage = "The settings have been reset";
                break;
            case 22:
                SpaFaultLog.faultMessage = "Program memory failure";
                break;
            case 26:
                SpaFaultLog.faultMessage = "Sensors are out of sync -- Call for service";
                break;
            case 27:
                SpaFaultLog.faultMessage = "The heater is dry";
                break;
            case 28:
                SpaFaultLog.faultMessage = "The heater may be dry";
                break;
            case 29:
                SpaFaultLog.faultMessage = "The water is too hot";
                break;
            case 30:
                SpaFaultLog.faultMessage = "The heater is too hot";
                break;
            case 31:
                SpaFaultLog.faultMessage = "Sensor A Fault";
                break;
            case 32:
                SpaFaultLog.faultMessage = "Sensor B Fault";
                break;
            case 34:
                SpaFaultLog.faultMessage = "A pump may be stuck on";
                break;
            case 35:
                SpaFaultLog.faultMessage = "Hot fault";
                break;
            case 36:
                SpaFaultLog.faultMessage = "The GFCI test failed";
                break;
            case 37:
                SpaFaultLog.faultMessage = "Standby Mode (Hold Mode)";
                break;
            default:
                SpaFaultLog.faultMessage = "Unknown error";
                break;
            }
            SpaFaultLog.daysAgo = Q_in[8];
            SpaFaultLog.hour = Q_in[9];
            SpaFaultLog.minutes = Q_in[10];
            // ESP_LOGD("Spa/fault/Entries", "Value: ", String(SpaFaultLog.totEntry).c_str());
            // ESP_LOGD("Spa/fault/Entry", "Value: ", String(SpaFaultLog.currEntry).c_str());
            // ESP_LOGD("Spa/fault/Code", "Value: ", String(SpaFaultLog.faultCode).c_str());
            // ESP_LOGD("Spa/fault/Message", "Value: ", SpaFaultLog.faultMessage.c_str());
            // ESP_LOGD("Spa/fault/DaysAgo", "Value: ", String(SpaFaultLog.daysAgo).c_str());
            // ESP_LOGD("Spa/fault/Hours", "Value: ", String(SpaFaultLog.hour).c_str());
            // ESP_LOGD("Spa/fault/Minutes", "Value: ", String(SpaFaultLog.minutes).c_str());
            have_faultlog = 2;
            // ESP_LOGD("Spa/debug/have_faultlog", "have the faultlog, #2");
        }

        // void BalboaSpa::on_set_temp(float temp)
        // {
        //     if (temp >= 26 || temp <= 40)
        //     {
        //         settemp = temp;
        //         command_to_send_ = 0xff;
        //     }
        // }
        // void on_set_clock(int hour, int minute)
        // {
        //     if ((hour >= 0 || hour <= 23) && (minute >= 0 || minute <= 59))
        //     {
        //         sethour = hour;
        //         setminute = minute;
        //         command_to_send_ = 0x21;
        //     }
        //     else
        //     {
        //         // ESP_LOGD("Spa/SetValues/Clock", "Invalid variable values!");
        //     }
        // }
        // void on_set_filtration(int f1hour, int f1min, int f1durhour, int f1durmin, int f2enab, int f2hour, int f2min, int f2durhour, int f2durmin)
        // {
        //     if ((f1hour >= 0 || f1hour <= 23) && (f1min >= 0 || f1min <= 59) && (f1durhour >= 0 || f1durhour <= 23) && (f1durmin >= 0 || f1durmin <= 59) && (f2enab == 0 || f2enab == 1) && (f2hour >= 0 || f2hour <= 23) && (f2min >= 0 || f2min <= 23) && (f2durhour >= 0 || f2durhour <= 23) && (f2durmin >= 0 || f2durmin <= 23))
        //     {
        //         setfilt1Hour = f1hour;
        //         setfilt1Minute = f1min;
        //         setfilt1DurationHour = f1durhour;
        //         setfilt1DurationMinute = f1durmin;
        //         setfilt2HourEnable = f2hour ^ (f2enab << 7);
        //         setfilt2Minute = f2min;
        //         setfilt2DurationHour = f2durhour;
        //         setfilt2DurationMinute = f2durmin;
        //         have_filtersettings = 0; // Re-request settings after change
        //         command_to_send_ = 0x23;
        //     }
        //     else
        //     {
        //         ESP_LOGD("Spa/SetValues/Filtration", "Invalid variable values!");
        //     }
        // }
        
        void BalboaSpa::rs485_send()
        {
            // Add telegram length
            Q_out.unshift(Q_out.size() + 2);

            // Add CRC
            Q_out.push(crc8(Q_out));

            // Wrap telegram in SOF/EOF
            Q_out.unshift(0x7E);
            Q_out.push(0x7E);

            for (int i = 0; i < Q_out.size(); i++)
            {
                this->write(Q_out[i]);
            }

            this->flush();

            Q_out.clear();
        }

        void BalboaSpa::ID_ack()
        {
            Q_out.push(id);
            Q_out.push(0xBF);
            Q_out.push(0x03);

            rs485_send();
        }
        void BalboaSpa::ID_request()
        {
            Q_out.push(0xFE);
            Q_out.push(0xBF);
            Q_out.push(0x01);
            Q_out.push(0x02);
            Q_out.push(0xF1);
            Q_out.push(0x73);

            rs485_send();
            ESP_LOGD("Spa/node/id", "Requested ID");
        }
        void BalboaSpa::loop()
        {
            const int MAX_BYTES_PER_LOOP = 8;
            int available = this->available();
            int to_process = (available < MAX_BYTES_PER_LOOP) ? available : MAX_BYTES_PER_LOOP;
            if (to_process == 0) return;
            for (int i = 0; i < to_process; i++)
            {
                int x = this->read();
                Q_in.push(x);
                // Drop until SOF is seen
                if (Q_in.size() > 0 && Q_in.first() != 0x7E)
                {
                    Q_in.clear();
                    continue;
                }
                // Double SOF-marker, drop last one
                if (Q_in.size() > 1 && Q_in[1] == 0x7E)
                {
                    Q_in.pop();
                // Complete package
                }
                if (x == 0x7E && Q_in.size() > 2)
                {
                    // ESP_LOGD("Spa/debug", "Got a package");
                    // Unregistered or yet in progress
                    if (id == 0)
                    {
                        ESP_LOGD("Spa/node/id", "Unregistered");
                        if (Q_in.size() > 2 && Q_in[2] == 0xFE)
                        {
                            print_msg(Q_in);
                            ESP_LOGD("Spa/node/id", "Unregistered");
                        }
                        // print_msg(Q_in);
                        //  FE BF 02:got new client ID
                        if (Q_in.size() > 4 && Q_in[2] == 0xFE && Q_in[4] == 0x02)
                        {
                            id = Q_in[5];
                            if (id > 0x2F)
                                id = 0x2F;
                            ESP_LOGD("Spa/node/id", "Got ID, acknowledging");
                            this->ID_ack();
                            ESP_LOGD(String(id).c_str(), "Spa/node/id");
                        }
                        // FE BF 00:Any new clients?
                        if (Q_in.size() > 4 && Q_in[2] == 0xFE && Q_in[4] == 0x00)
                        {
                            ESP_LOGD("Spa/node/id", "Requesting ID");
                            this->ID_request();
                        }
                    }
                        // id BF 06:Ready to Send
                    else if (Q_in.size() > 4 && Q_in[2] == id && Q_in[4] == 0x06)
                    {
                        if (command_to_send_ == 0x21)
                        {
                            Q_out.push(id);
                            Q_out.push(0xBF);
                            Q_out.push(0x21);
                            Q_out.push(sethour);
                            Q_out.push(setminute);
                        }
                        else if (command_to_send_ == 0xff)
                        {
                            // 0xff marks dirty temperature for now
                            Q_out.push(id);
                            Q_out.push(0xBF);
                            Q_out.push(0x20);
                            Q_out.push(settemp);
                        }
                        else if (command_to_send_ == 0x23)
                        {
                            Q_out.push(id);
                            Q_out.push(0xBF);
                            Q_out.push(0x23);
                            Q_out.push(setfilt1Hour);
                            Q_out.push(setfilt1Minute);
                            Q_out.push(setfilt1DurationHour);
                            Q_out.push(setfilt1DurationMinute);
                            Q_out.push(setfilt2HourEnable);
                            Q_out.push(setfilt2Minute);
                            Q_out.push(setfilt2DurationHour);
                            Q_out.push(setfilt2DurationMinute);
                        }
                        else if (command_to_send_ == 0x00)
                        {
                            if (have_config == 0)
                            { // Get configuration of the hot tub
                                Q_out.push(id);
                                Q_out.push(0xBF);
                                Q_out.push(0x22);
                                Q_out.push(0x00);
                                Q_out.push(0x00);
                                Q_out.push(0x01);
                                ESP_LOGD("Spa/config/status", "Getting config");
                                // have_config = 1;
                            }
                            else if ((have_faultlog == 0) && (have_config == 2))
                            { // Get the fault log
                                Q_out.push(id);
                                Q_out.push(0xBF);
                                Q_out.push(0x22);
                                Q_out.push(0x20);
                                Q_out.push(0xFF);
                                Q_out.push(0x00);
                                // have_faultlog = 1;
                                // ESP_LOGD("Spa/debug/have_faultlog", "requesting fault log, #1");
                            }
                            else if ((have_filtersettings == 0) && (have_faultlog == 2))
                            { // Get the filter cycles log once we have the faultlog
                                Q_out.push(id);
                                Q_out.push(0xBF);
                                Q_out.push(0x22);
                                Q_out.push(0x01);
                                Q_out.push(0x00);
                                Q_out.push(0x00);
                                // ESP_LOGD("Spa/debug/have_filtersettings", "requesting filter settings, #1");
                                // have_filtersettings = 1;
                            }
                            else
                            {
                                // A Nothing to Send message is sent by a client immediately after a Clear to Send message if the client has no messages to send.
                                Q_out.push(id);
                                Q_out.push(0xBF);
                                Q_out.push(0x07);
                            }
                        }
                        else
                        {
                            // Send toggle commands
                            Q_out.push(id);
                            Q_out.push(0xBF);
                            Q_out.push(0x11);
                            Q_out.push(command_to_send_);
                            Q_out.push(0x00);
                        }
                        rs485_send();
                        command_to_send_ = 0x00;
                    }
                    else if (Q_in.size() > 4 && Q_in[2] == id && Q_in[4] == 0x2E)
                    {
                        if (last_state_crc != Q_in[Q_in[1]])
                        {
                            decodeSettings();
                        }
                    }
                    else if (Q_in.size() > 4 && Q_in[2] == id && Q_in[4] == 0x28)
                    {
                        if (last_state_crc != Q_in[Q_in[1]])
                        {
                            decodeFault();
                        }
                    }
                    else if (Q_in.size() > 4 && Q_in[2] == 0xFF && Q_in[4] == 0x13)
                    {
                        if (last_state_crc != Q_in[Q_in[1]])
                        {
                            decodeState();
                        }
                    }
                    else if (Q_in.size() > 4 && Q_in[2] == id && Q_in[4] == 0x23)
                    {
                        if (last_state_crc != Q_in[Q_in[1]])
                        {
                            decodeFilterSettings();
                        }
                    }
                    yield();
                    Q_in.clear();
                }
            }
            // Publish sensors if value changed
            if (!isnan(latest_controller_target_temp) && latest_controller_target_temp != last_controller_target_temp && controller_target_temp_sensor) {
                controller_target_temp_sensor->publish_state(latest_controller_target_temp);
                last_controller_target_temp = latest_controller_target_temp;
            }
            if (!isnan(latest_manual_target_temp) && latest_manual_target_temp != last_manual_target_temp && temperature_control_) {
                temperature_control_->publish_state(latest_manual_target_temp);
                last_manual_target_temp = latest_manual_target_temp;
            }
            if (!isnan(latest_current_temp) && latest_current_temp != last_current_temp && current_temp_sensor) {
                current_temp_sensor->publish_state(latest_current_temp);
                last_current_temp = latest_current_temp;
            }
            if (!isnan(latest_hour) && latest_hour != last_hour && hour_sensor) {
                hour_sensor->publish_state(latest_hour);
                last_hour = latest_hour;
            }
            if (!isnan(latest_minute) && latest_minute != last_minute && minute_sensor) {
                minute_sensor->publish_state(latest_minute);
                last_minute = latest_minute;
            }
            if (!isnan(latest_restmode) && latest_restmode != last_restmode && restmode_sensor) {
                restmode_sensor->publish_state(latest_restmode);
                last_restmode = latest_restmode;
            }
            if (!isnan(latest_heater) && latest_heater != last_heater && heater_sensor) {
                heater_sensor->publish_state(latest_heater);
                last_heater = latest_heater;
            }
            if (!isnan(latest_highrange) && latest_highrange != last_highrange && highrange_sensor) {
                highrange_sensor->publish_state(latest_highrange);
                last_highrange = latest_highrange;
            }
            if (!isnan(latest_jet1) && latest_jet1 != last_jet1 && jet1_sensor) {
                jet1_sensor->publish_state(latest_jet1);
                last_jet1 = latest_jet1;
            }
            if (!isnan(latest_jet2) && latest_jet2 != last_jet2 && jet2_sensor) {
                jet2_sensor->publish_state(latest_jet2);
                last_jet2 = latest_jet2;
            }
            if (!isnan(latest_light) && latest_light != last_light && light_sensor) {
                light_sensor->publish_state(latest_light);
                last_light = latest_light;
            }
            if (!isnan(latest_light_switch) && latest_light_switch != last_light_switch && light_switch_) {
                light_switch_->publish_state(latest_light_switch == 1);
                last_light_switch = latest_light_switch;
            }
            // Filter sensors
            if (!isnan(latest_filt1hour) && latest_filt1hour != last_filt1hour && filt1hour_sensor) {
                filt1hour_sensor->publish_state(latest_filt1hour);
                last_filt1hour = latest_filt1hour;
            }
            if (!isnan(latest_filt1minute) && latest_filt1minute != last_filt1minute && filt1minute_sensor) {
                filt1minute_sensor->publish_state(latest_filt1minute);
                last_filt1minute = latest_filt1minute;
            }
            if (!isnan(latest_filt1durhour) && latest_filt1durhour != last_filt1durhour && filt1durhour_sensor) {
                filt1durhour_sensor->publish_state(latest_filt1durhour);
                last_filt1durhour = latest_filt1durhour;
            }
            if (!isnan(latest_filt1durminute) && latest_filt1durminute != last_filt1durminute && filt1durminute_sensor) {
                filt1durminute_sensor->publish_state(latest_filt1durminute);
                last_filt1durminute = latest_filt1durminute;
            }
            if (!isnan(latest_filt2enable) && latest_filt2enable != last_filt2enable && filt2enable_sensor) {
                filt2enable_sensor->publish_state(latest_filt2enable);
                last_filt2enable = latest_filt2enable;
            }
            if (!isnan(latest_filt2hour) && latest_filt2hour != last_filt2hour && filt2hour_sensor) {
                filt2hour_sensor->publish_state(latest_filt2hour);
                last_filt2hour = latest_filt2hour;
            }
            if (!isnan(latest_filt2minute) && latest_filt2minute != last_filt2minute && filt2minute_sensor) {
                filt2minute_sensor->publish_state(latest_filt2minute);
                last_filt2minute = latest_filt2minute;
            }
            if (!isnan(latest_filt2durhour) && latest_filt2durhour != last_filt2durhour && filt2durhour_sensor) {
                filt2durhour_sensor->publish_state(latest_filt2durhour);
                last_filt2durhour = latest_filt2durhour;
            }
            if (!isnan(latest_filt2durminute) && latest_filt2durminute != last_filt2durminute && filt2durminute_sensor) {
                filt2durminute_sensor->publish_state(latest_filt2durminute);
                last_filt2durminute = latest_filt2durminute;
            }
        }

        void BalboaSpaSwitch::write_state(bool state) {
            publish_state(state);
            if (parent_) {
                parent_->set_command(command_code_);
            }
        }

        void BalboaSpaTemperature::control(float value) {
            if (parent_ && value >= 80 && value <= 104) {
                // Only send command if the value has actually changed
                if (settemp != static_cast<uint8_t>(value)) {
                    parent_->set_target_temperature(value);
                    ESP_LOGD("balboa_spa", "Manual target temperature changed to %.1f°F", value);
                }
            }
            publish_state(value);
        }

        void BalboaSpaButton::press_action() {
            if (parent_) {
                parent_->set_command(command_code_);
            }
        }

        void BalboaSpa::on_target_temperature_update(float value) {
            settemp = value;
            command_to_send_ = 0xff;
        }

    } // namespace balboa_spa
} // namespace esphome
