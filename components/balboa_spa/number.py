import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ID,
    CONF_MIN_VALUE,
    CONF_MAX_VALUE,
    CONF_STEP,
)
from . import CONF_SPA_ID, BalboaSpa, balboa_spa_ns

# Component dependencies
DEPENDENCIES = ["balboa_spa"]

# Create the temperature control class
BalboaSpaTemperature = balboa_spa_ns.class_("BalboaSpaTemperature", number.Number, cg.Component)

# Temperature configuration key
CONF_MANUAL_TARGET_TEMPERATURE = "manual_target_temperature"

# Configuration schema for the number component
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(BalboaSpa),
    cv.Optional(CONF_MANUAL_TARGET_TEMPERATURE): number.number_schema(
        BalboaSpaTemperature
    ).extend({
        cv.Optional(CONF_MIN_VALUE, default=70): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=106): cv.float_,
        cv.Optional(CONF_STEP, default=1): cv.float_,
    }),
})

async def to_code(config):
    """Generate code for the spa manual temperature control."""
    if CONF_MANUAL_TARGET_TEMPERATURE in config:
        # Get the spa component
        spa = await cg.get_variable(config[CONF_SPA_ID])
        
        # Get the configuration for this number
        conf = config[CONF_MANUAL_TARGET_TEMPERATURE]
        
        # Create and register the temperature control
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await number.register_number(
            var, 
            conf,
            min_value=conf[CONF_MIN_VALUE],
            max_value=conf[CONF_MAX_VALUE], 
            step=conf[CONF_STEP]
        )
        
        # Set the temperature control
        cg.add(spa.set_temperature_control(var)) 