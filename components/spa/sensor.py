import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    ICON_THERMOMETER,
    ICON_TIMER,
    ICON_WATER_PUMP,
    ICON_LIGHTBULB,
    ICON_POWER,
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor"]

spa_reader_ns = cg.esphome_ns.namespace("spa_reader")
SpaReader = spa_reader_ns.class_("SpaReader", cg.Component)

# Configuration entries
CONF_TEMP_SENSOR = "temp_sensor"
CONF_TARGET_TEMP_SENSOR = "target_temp_sensor"
CONF_JET1_SENSOR = "jet1_sensor"
CONF_JET2_SENSOR = "jet2_sensor"
CONF_JET3_SENSOR = "jet3_sensor"
CONF_LIGHT_SENSOR = "light_sensor"
CONF_RESTMODE_SENSOR = "restmode_sensor"
CONF_HIGHRANGE_SENSOR = "highrange_sensor"
CONF_HOUR_SENSOR = "hour_sensor"
CONF_MINUTE_SENSOR = "minute_sensor"
CONF_HEATER_SENSOR = "heater_sensor"
CONF_CIRC_SENSOR = "circ_sensor"
CONF_FILT1HOUR_SENSOR = "filt1hour_sensor"
CONF_FILT1MINUTE_SENSOR = "filt1minute_sensor"
CONF_FILT1DURHOUR_SENSOR = "filt1durhour_sensor"
CONF_FILT1DURMINUTE_SENSOR = "filt1durminute_sensor"
CONF_FILT2ENABLE_SENSOR = "filt2enable_sensor"
CONF_FILT2HOUR_SENSOR = "filt2hour_sensor"
CONF_FILT2MINUTE_SENSOR = "filt2minute_sensor"
CONF_FILT2DURHOUR_SENSOR = "filt2durhour_sensor"
CONF_FILT2DURMINUTE_SENSOR = "filt2durminute_sensor"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SpaReader),
        cv.Optional(CONF_TEMP_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_THERMOMETER,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TARGET_TEMP_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_THERMOMETER,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_JET1_SENSOR): sensor.sensor_schema(
            icon=ICON_WATER_PUMP,
        ),
        cv.Optional(CONF_JET2_SENSOR): sensor.sensor_schema(
            icon=ICON_WATER_PUMP,
        ),
        cv.Optional(CONF_JET3_SENSOR): sensor.sensor_schema(
            icon=ICON_WATER_PUMP,
        ),
        cv.Optional(CONF_LIGHT_SENSOR): sensor.sensor_schema(
            icon=ICON_LIGHTBULB,
        ),
        cv.Optional(CONF_RESTMODE_SENSOR): sensor.sensor_schema(
            icon=ICON_POWER,
        ),
        cv.Optional(CONF_HIGHRANGE_SENSOR): sensor.sensor_schema(
            icon=ICON_THERMOMETER,
        ),
        cv.Optional(CONF_HOUR_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_MINUTE_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_HEATER_SENSOR): sensor.sensor_schema(
            icon=ICON_POWER,
        ),
        cv.Optional(CONF_CIRC_SENSOR): sensor.sensor_schema(
            icon=ICON_WATER_PUMP,
        ),
        cv.Optional(CONF_FILT1HOUR_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_FILT1MINUTE_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_FILT1DURHOUR_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_FILT1DURMINUTE_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_FILT2ENABLE_SENSOR): sensor.sensor_schema(
            icon=ICON_POWER,
        ),
        cv.Optional(CONF_FILT2HOUR_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_FILT2MINUTE_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_FILT2DURHOUR_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
        cv.Optional(CONF_FILT2DURMINUTE_SENSOR): sensor.sensor_schema(
            icon=ICON_TIMER,
        ),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_TEMP_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_TEMP_SENSOR])
        cg.add(var.temp_sensor.set_sensor(sens))

    if CONF_TARGET_TEMP_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_TARGET_TEMP_SENSOR])
        cg.add(var.target_temp_sensor.set_sensor(sens))

    if CONF_JET1_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_JET1_SENSOR])
        cg.add(var.jet1_sensor.set_sensor(sens))

    if CONF_JET2_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_JET2_SENSOR])
        cg.add(var.jet2_sensor.set_sensor(sens))

    if CONF_JET3_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_JET3_SENSOR])
        cg.add(var.jet3_sensor.set_sensor(sens))

    if CONF_LIGHT_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_LIGHT_SENSOR])
        cg.add(var.light_sensor.set_sensor(sens))

    if CONF_RESTMODE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_RESTMODE_SENSOR])
        cg.add(var.restmode_sensor.set_sensor(sens))

    if CONF_HIGHRANGE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_HIGHRANGE_SENSOR])
        cg.add(var.highrange_sensor.set_sensor(sens))

    if CONF_HOUR_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_HOUR_SENSOR])
        cg.add(var.hour_sensor.set_sensor(sens))

    if CONF_MINUTE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_MINUTE_SENSOR])
        cg.add(var.minute_sensor.set_sensor(sens))

    if CONF_HEATER_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_HEATER_SENSOR])
        cg.add(var.heater_sensor.set_sensor(sens))

    if CONF_CIRC_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_CIRC_SENSOR])
        cg.add(var.circ_sensor.set_sensor(sens))

    if CONF_FILT1HOUR_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT1HOUR_SENSOR])
        cg.add(var.filt1hour_sensor.set_sensor(sens))

    if CONF_FILT1MINUTE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT1MINUTE_SENSOR])
        cg.add(var.filt1minute_sensor.set_sensor(sens))

    if CONF_FILT1DURHOUR_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT1DURHOUR_SENSOR])
        cg.add(var.filt1durhour_sensor.set_sensor(sens))

    if CONF_FILT1DURMINUTE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT1DURMINUTE_SENSOR])
        cg.add(var.filt1durminute_sensor.set_sensor(sens))

    if CONF_FILT2ENABLE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT2ENABLE_SENSOR])
        cg.add(var.filt2enable_sensor.set_sensor(sens))

    if CONF_FILT2HOUR_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT2HOUR_SENSOR])
        cg.add(var.filt2hour_sensor.set_sensor(sens))

    if CONF_FILT2MINUTE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT2MINUTE_SENSOR])
        cg.add(var.filt2minute_sensor.set_sensor(sens))

    if CONF_FILT2DURHOUR_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT2DURHOUR_SENSOR])
        cg.add(var.filt2durhour_sensor.set_sensor(sens))

    if CONF_FILT2DURMINUTE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_FILT2DURMINUTE_SENSOR])
        cg.add(var.filt2durminute_sensor.set_sensor(sens))

