import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    ICON_LIGHTBULB,
    ICON_THERMOMETER,
    CONF_ID,
    CONF_NAME,
)
from . import CONF_SPA_ID, BalboaSpa, balboa_spa_ns

# Component dependencies
DEPENDENCIES = ["balboa_spa"]

# Create the switch class
BalboaSpaSwitch = balboa_spa_ns.class_("BalboaSpaSwitch", switch.Switch, cg.Component)

# Switch configuration keys
CONF_LIGHT_SWITCH = "light"
CONF_HEAT_MODE_SWITCH = "heat_mode"

# Configuration schema for the switch component
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(BalboaSpa),
    cv.Optional(CONF_LIGHT_SWITCH): switch.switch_schema(BalboaSpaSwitch, icon=ICON_LIGHTBULB),
    cv.Optional(CONF_HEAT_MODE_SWITCH): switch.switch_schema(BalboaSpaSwitch, icon=ICON_THERMOMETER),
})

async def to_code(config):
    """Generate code for the spa switches."""
    # Get the spa component
    spa = await cg.get_variable(config[CONF_SPA_ID])
    
    # Process each switch configuration
    for key in [CONF_LIGHT_SWITCH, CONF_HEAT_MODE_SWITCH]:
        if key not in config:
            continue
            
        # Get the switch configuration
        conf = config[key]
        
        # Create and register the switch
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await switch.register_switch(var, conf)
        
        # Set the appropriate setter method
        cg.add(getattr(spa, f"set_{key}_switch")(var))