import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)
from . import spa_ns, SpaReader, CONF_SPA_ID

SENSORS = {
    "temp_sensor": ("Current Temp", UNIT_CELSIUS, 1),
    "target_temp_sensor": ("Target Temp", UNIT_CELSIUS, 1),
    "jet1_sensor": ("Jet1", None, 0),
    "jet2_sensor": ("Jet2", None, 0),
    "jet3_sensor": ("Jet3", None, 0),
    "light_sensor": ("Light", None, 0),
    "restmode_sensor": ("Rest Mode", None, 0),
    "highrange_sensor": ("High Range", None, 0),
    "hour_sensor": ("Hour", None, 0),
    "minute_sensor": ("Minute", None, 0),
    "heater_sensor": ("Heater", None, 0),
    "circ_sensor": ("Circulation Pump", None, 0),
    "filt1hour_sensor": ("Filter 1 Hour", None, 0),
    "filt1minute_sensor": ("Filter 1 Minute", None, 0),
    "filt1durhour_sensor": ("Filter 1 Duration Hour", None, 0),
    "filt1durminute_sensor": ("Filter 1 Duration Minute", None, 0),
    "filt2enable_sensor": ("Filter 2 Enable", None, 0),
    "filt2hour_sensor": ("Filter 2 Hour", None, 0),
    "filt2minute_sensor": ("Filter 2 Minute", None, 0),
    "filt2durhour_sensor": ("Filter 2 Duration Hour", None, 0),
    "filt2durminute_sensor": ("Filter 2 Duration Minute", None, 0),
}

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(SpaReader),
    cv.Optional(sensor_id): sensor.sensor_schema(
        unit_of_measurement=unit,
        accuracy_decimals=accuracy,
        device_class=DEVICE_CLASS_TEMPERATURE if "temp" in sensor_id else None,
    ) for sensor_id, (name, unit, accuracy) in SENSORS.items()
})

async def to_code(config):
    spa = await cg.get_variable(config[CONF_SPA_ID])
    
    for sensor_id, (name, unit, accuracy) in SENSORS.items():
        if sensor_id in config:
            sens = await sensor.new_sensor(config[sensor_id])
            cg.add(spa.set_sensor(sensor_id, sens))
