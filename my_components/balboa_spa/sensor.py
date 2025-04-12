import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    UNIT_CELSIUS,
    ICON_THERMOMETER,
)


from . import CONF_SPA_ID, BalboaSpa

DEPENDENCIES = ["uart", "balboa_spa"]

CODEOWNERS = ["@pvarsanyi"]


# Define all available sensors
CONF_CURRENT_TEMPERATURE = "current_temperature"
CONF_TARGET_TEMP = "target_temperature"
CONF_JET1 = "jet1"
CONF_JET2 = "jet2"
CONF_JET3 = "jet3"
CONF_LIGHT = "light"
CONF_RESTMODE = "restmode"
CONF_HIGHRANGE = "highrange"
CONF_HOUR = "hour"
CONF_MINUTE = "minute"
CONF_HEATER = "heater"
CONF_CIRC = "circ"
CONF_FILT1_HOUR = "filt1hour"
CONF_FILT1_MINUTE = "filt1minute"
CONF_FILT1_DURHOUR = "filt1durhour"
CONF_FILT1_DURMINUTE = "filt1durminute"
CONF_FILT2_ENABLE = "filt2enable"
CONF_FILT2_HOUR = "filt2hour"
CONF_FILT2_MINUTE = "filt2minute"
CONF_FILT2_DURHOUR = "filt2durhour"
CONF_FILT2_DURMINUTE = "filt2durminute"


CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(BalboaSpa),
    cv.Optional(CONF_CURRENT_TEMPERATURE): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1
    ),
    cv.Optional(CONF_TARGET_TEMP): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1
    ),
    cv.Optional(CONF_JET1): sensor.sensor_schema(),
    cv.Optional(CONF_JET2): sensor.sensor_schema(),
    cv.Optional(CONF_JET3): sensor.sensor_schema(),
    cv.Optional(CONF_LIGHT): sensor.sensor_schema(),
    cv.Optional(CONF_RESTMODE): sensor.sensor_schema(),
    cv.Optional(CONF_HIGHRANGE): sensor.sensor_schema(),
    cv.Optional(CONF_HOUR): sensor.sensor_schema(),
    cv.Optional(CONF_MINUTE): sensor.sensor_schema(),
    cv.Optional(CONF_HEATER): sensor.sensor_schema(),
    cv.Optional(CONF_CIRC): sensor.sensor_schema(),
    cv.Optional(CONF_FILT1_HOUR): sensor.sensor_schema(),
    cv.Optional(CONF_FILT1_MINUTE): sensor.sensor_schema(),
    cv.Optional(CONF_FILT1_DURHOUR): sensor.sensor_schema(),
    cv.Optional(CONF_FILT1_DURMINUTE): sensor.sensor_schema(),
    cv.Optional(CONF_FILT2_ENABLE): sensor.sensor_schema(),
    cv.Optional(CONF_FILT2_HOUR): sensor.sensor_schema(),
    cv.Optional(CONF_FILT2_MINUTE): sensor.sensor_schema(),
    cv.Optional(CONF_FILT2_DURHOUR): sensor.sensor_schema(),
    cv.Optional(CONF_FILT2_DURMINUTE): sensor.sensor_schema(),
})


async def to_code(config):
    """Generate code for the spa sensors."""
    hub = await cg.get_variable(config[CONF_SPA_ID])
    
    for key, conf in config.items():
        if key == CONF_SPA_ID:
            continue
        if not isinstance(conf, dict):  # Skip non-dictionary configs
            continue
        var = await sensor.new_sensor(conf)
        if key == CONF_CURRENT_TEMPERATURE:
            cg.add(getattr(hub, "set_current_temperature_sensor")(var))
        else:
            cg.add(getattr(hub, f"set_{key}_sensor")(var))
