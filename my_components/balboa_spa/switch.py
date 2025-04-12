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

DEPENDENCIES = ["balboa_spa"]

# Create the switch class
BalboaSpaSwitch = balboa_spa_ns.class_("BalboaSpaSwitch", switch.Switch, cg.Component)

# Switch types
CONF_LIGHT_SWITCH = "light"
CONF_HEAT_MODE_SWITCH = "heat_mode"
CONF_JET1_SWITCH = "jet1"
CONF_JET2_SWITCH = "jet2"

def switch_schema(icon):
    return cv.Schema({
        cv.GenerateID(): cv.declare_id(BalboaSpaSwitch),
        cv.Optional(CONF_NAME): cv.string,
    }).extend(switch.SWITCH_SCHEMA)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(BalboaSpa),
    cv.Optional(CONF_LIGHT_SWITCH): switch_schema(ICON_LIGHTBULB),
    cv.Optional(CONF_HEAT_MODE_SWITCH): switch_schema(ICON_THERMOMETER),
    cv.Optional(CONF_JET1_SWITCH): switch_schema("mdi:valve-pipe"),
    cv.Optional(CONF_JET2_SWITCH): switch_schema("mdi:valve-pipe"),
})

async def to_code(config):
    spa = await cg.get_variable(config[CONF_SPA_ID])
    
    for key in [CONF_LIGHT_SWITCH, CONF_HEAT_MODE_SWITCH, CONF_JET1_SWITCH, CONF_JET2_SWITCH]:
        if key not in config:
            continue
        conf = config[key]
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await switch.register_switch(var, conf)
        cg.add(getattr(spa, f"set_{key}_switch")(var))