import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
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
CONF_TEMPERATURE = "temperature"

# Configuration schema for the number component
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(BalboaSpa),
    cv.Optional(CONF_TEMPERATURE): number.number_schema(
        BalboaSpaTemperature,
        min_value=26,
        max_value=40,
        step=0.5,
    ),
})

async def to_code(config):
    """Generate code for the spa temperature control."""
    if CONF_TEMPERATURE in config:
        # Get the spa component
        spa = await cg.get_variable(config[CONF_SPA_ID])
        
        # Create and register the temperature control
        var = cg.new_Pvariable(config[CONF_TEMPERATURE][CONF_ID])
        await cg.register_component(var, config[CONF_TEMPERATURE])
        await number.register_number(var, config[CONF_TEMPERATURE])
        
        # Set the temperature control
        cg.add(spa.set_temperature_control(var)) 