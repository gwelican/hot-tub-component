import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID
from . import CONF_SPA_ID, BalboaSpa, balboa_spa_ns

# Component dependencies
DEPENDENCIES = ["balboa_spa"]

# Create the button class
BalboaSpaButton = balboa_spa_ns.class_("BalboaSpaButton", button.Button, cg.Component)

# Button configuration keys
CONF_JET1_BUTTON = "jet1"
CONF_JET2_BUTTON = "jet2"
CONF_SOAK_BUTTON = "soak"

# Configuration schema for the button component
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(BalboaSpa),
    cv.Optional(CONF_JET1_BUTTON): button.BUTTON_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(BalboaSpaButton),
    }).extend(cv.COMPONENT_SCHEMA),
    cv.Optional(CONF_JET2_BUTTON): button.BUTTON_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(BalboaSpaButton),
    }).extend(cv.COMPONENT_SCHEMA),
    cv.Optional(CONF_SOAK_BUTTON): button.BUTTON_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(BalboaSpaButton),
    }).extend(cv.COMPONENT_SCHEMA),
})

async def to_code(config):
    """Generate code for the spa buttons."""
    # Get the spa component
    spa = await cg.get_variable(config[CONF_SPA_ID])
    
    # Process each button configuration
    for key in [CONF_JET1_BUTTON, CONF_JET2_BUTTON, CONF_SOAK_BUTTON]:
        if key not in config:
            continue
            
        # Get the button configuration
        conf = config[key]
        
        # Create and register the button
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await button.register_button(var, conf)
        
        # Set the appropriate setter method
        cg.add(getattr(spa, f"set_{key}_button")(var)) 