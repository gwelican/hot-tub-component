import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_TYPE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)
from . import spa_ns, SpaReader, CONF_SPA_ID

SENSOR_TYPES = {
    "temp_sensor": (UNIT_CELSIUS, 1, DEVICE_CLASS_TEMPERATURE),
    "target_temp_sensor": (UNIT_CELSIUS, 1, DEVICE_CLASS_TEMPERATURE),
    "jet1_sensor": (None, 0, None),
    "jet2_sensor": (None, 0, None),
    "jet3_sensor": (None, 0, None),
    "light_sensor": (None, 0, None),
    "restmode_sensor": (None, 0, None),
    "highrange_sensor": (None, 0, None),
    "hour_sensor": (None, 0, None),
    "minute_sensor": (None, 0, None),
    "heater_sensor": (None, 0, None),
    "circ_sensor": (None, 0, None),
    "filt1hour_sensor": (None, 0, None),
    "filt1minute_sensor": (None, 0, None),
    "filt1durhour_sensor": (None, 0, None),
    "filt1durminute_sensor": (None, 0, None),
    "filt2enable_sensor": (None, 0, None),
    "filt2hour_sensor": (None, 0, None),
    "filt2minute_sensor": (None, 0, None),
    "filt2durhour_sensor": (None, 0, None),
    "filt2durminute_sensor": (None, 0, None),
}

CONFIG_SCHEMA = sensor.sensor_schema().extend(
    {
        cv.GenerateID(CONF_SPA_ID): cv.use_id(SpaReader),
        cv.Required(CONF_TYPE): cv.enum(SENSOR_TYPES, upper=False),
    }
)


async def to_code(config):
    spa = await cg.get_variable(config[CONF_SPA_ID])
    var = await sensor.new_sensor(config)
    cg.add(spa.set_sensor(config[CONF_TYPE], var))
