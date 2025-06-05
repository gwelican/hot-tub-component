# Balboa Spa Component for ESPHome

This is a custom ESPHome component for controlling Balboa hot tub systems.

## Recent Updates (v2025.5.2 Compatible)

**Fixed compatibility issues with ESPHome 2025.5.2:**

- Replaced deprecated `button.BUTTON_SCHEMA` with `button.button_schema()`
- Replaced deprecated `number.NUMBER_SCHEMA` with `number.number_schema()`  
- Updated switch configuration to use `switch.switch_schema()`
- Fixed component registration issues that were causing double registration errors
- Removed jet switches (they are implemented as buttons in the C++ code)
- **Added dual temperature sensors**: Separate read-only and write-only temperature controls

## Components

### Switches
- **Light Switch**: Toggle the spa light on/off
- **Heat Mode Switch**: Toggle heat mode on/off

### Buttons
- **Jet1 Button**: Activate jet 1
- **Jet2 Button**: Activate jet 2  
- **Soak Button**: Activate soak mode

### Number
- **Manual Target Temperature**: Override the target temperature remotely (70-106°F, 1° steps)

### Sensors
- **Current Temperature**: Actual water temperature
- **Target Temperature**: (Legacy) Target temperature reading
- **Active Target Temperature**: Read-only sensor showing what the controller is actually set to
- Jet states
- Light state
- Rest mode
- High range mode
- Time (hour/minute)
- Heater state
- Filter settings and schedules

## Temperature Control System

The component uses a dual-sensor approach:

### Active Target Temperature (Read-Only Sensor)
- Shows **exactly** what the hot tub controller is set to
- Always updates with controller readings (no filtering)
- Use this to monitor changes made via the physical panel

### Manual Target Temperature (Number Control)  
- Write-only control for remote temperature changes
- Use this to set the temperature remotely from Home Assistant
- Range: 70-106°F with 1° steps

This design lets you see what the controller is set to while providing separate remote control.

## Usage

Add this to your ESPHome configuration:

```yaml
external_components:
  - source: github://gwelican/hot-tub-component
    
uart:
  id: spa_uart
  tx_pin: GPIO1
  rx_pin: GPIO3
  baud_rate: 115200

balboa_spa:
  id: spa_reader
  uart_id: spa_uart

# Configure individual components as needed
switch:
  - platform: balboa_spa
    spa_id: spa_reader
    light:
      name: "Spa Light"
    heat_mode:
      name: "Spa Heat Mode"

button:
  - platform: balboa_spa
    spa_id: spa_reader
    jet1:
      name: "Spa Jet 1"
    jet2:
      name: "Spa Jet 2"
    soak:
      name: "Spa Soak"

sensor:
  - platform: balboa_spa
    spa_id: spa_reader
    current_temperature:
      name: "Spa Current Temperature"
    active_target_temperature:
      name: "Spa Active Target Temperature"

number:
  - platform: balboa_spa
    spa_id: spa_reader
    manual_target_temperature:
      name: "Spa Manual Target Temperature"
```

## Dependencies

- ESPHome 2025.5.2 or later
- CircularBuffer library (automatically included)
- UART component 