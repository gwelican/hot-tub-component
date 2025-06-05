import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ID,
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
        cv.Optional("min_value", default=70): cv.float_,
        cv.Optional("max_value", default=106): cv.float_,
        cv.Optional("step", default=1): cv.float_,
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
            min_value=conf.get("min_value", 70),
            max_value=conf.get("max_value", 106), 
            step=conf.get("step", 1)
        )
        
        # Set the temperature control
        cg.add(spa.set_temperature_control(var)) 