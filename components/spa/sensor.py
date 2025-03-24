import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)
from . import spa_ns, SpaReader

CONF_SPA_ID = "spa_id"

TYPES = [
    "temp_sensor",
    "target_temp_sensor",
    "jet1_sensor",
    "jet2_sensor",
    "jet3_sensor",
    "light_sensor",
    "restmode_sensor",
    "highrange_sensor",
    "hour_sensor",
    "minute_sensor",
    "heater_sensor",
    "circ_sensor",
    "filt1hour_sensor",
    "filt1minute_sensor",
    "filt1durhour_sensor",
    "filt1durminute_sensor",
    "filt2enable_sensor",
    "filt2hour_sensor",
    "filt2minute_sensor",
    "filt2durhour_sensor",
    "filt2durminute_sensor",
]

CONFIG_SCHEMA = sensor.sensor_schema().extend({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(SpaReader),
})

async def to_code(config):
    var = await sensor.new_sensor(config)
    spa = await cg.get_variable(config[CONF_SPA_ID])
    cg.add(spa.register_sensor(var))
