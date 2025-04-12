import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ID,
    CONF_MIN_VALUE,
    CONF_MAX_VALUE,
    CONF_STEP,
    UNIT_CELSIUS,
)
from . import CONF_SPA_ID, BalboaSpa, balboa_spa_ns

DEPENDENCIES = ["balboa_spa"]

BalboaSpaTemperature = balboa_spa_ns.class_("BalboaSpaTemperature", number.Number, cg.Component)

CONF_TEMPERATURE = "temperature"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(BalboaSpa),
    cv.Optional(CONF_TEMPERATURE): number.NUMBER_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(BalboaSpaTemperature),
        cv.Optional(CONF_MIN_VALUE, default=26): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=40): cv.float_,
        cv.Optional(CONF_STEP, default=0.5): cv.float_,
    }).extend(cv.COMPONENT_SCHEMA),
})

async def to_code(config):
    if CONF_TEMPERATURE in config:
        spa = await cg.get_variable(config[CONF_SPA_ID])
        var = await number.new_number(
            config[CONF_TEMPERATURE],
            min_value=config[CONF_TEMPERATURE][CONF_MIN_VALUE],
            max_value=config[CONF_TEMPERATURE][CONF_MAX_VALUE],
            step=config[CONF_TEMPERATURE][CONF_STEP],
        )
        await cg.register_component(var, config[CONF_TEMPERATURE])
        cg.add(spa.set_temperature_control(var)) 