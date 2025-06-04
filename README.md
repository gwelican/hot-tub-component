# Balboa Spa Component for ESPHome

This is a custom ESPHome component for controlling Balboa hot tub systems.

## Recent Updates (v2025.5.2 Compatible)

**Fixed compatibility issues with ESPHome 2025.5.2:**

- Replaced deprecated `button.BUTTON_SCHEMA` with `button.button_schema()`
- Replaced deprecated `number.NUMBER_SCHEMA` with `number.number_schema()`  
- Updated switch configuration to use `switch.switch_schema()`
- Fixed component registration issues that were causing double registration errors
- Removed jet switches (they are implemented as buttons in the C++ code)

## Components

### Switches
- **Light Switch**: Toggle the spa light on/off
- **Heat Mode Switch**: Toggle heat mode on/off

### Buttons
- **Jet1 Button**: Activate jet 1
- **Jet2 Button**: Activate jet 2  
- **Soak Button**: Activate soak mode

### Number
- **Temperature Control**: Set target temperature (26-40°C, 0.5° steps)

### Sensors
- Current temperature
- Target temperature
- Jet states
- Light state
- Rest mode
- High range mode
- Time (hour/minute)
- Heater state
- Filter settings and schedules

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

number:
  - platform: balboa_spa
    spa_id: spa_reader
    temperature:
      name: "Spa Target Temperature"
```

## Dependencies

- ESPHome 2025.5.2 or later
- CircularBuffer library (automatically included)
- UART component 