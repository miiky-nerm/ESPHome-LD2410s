import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID
from . import LD2410S

CONF_TARGET_DISTANCE = "target_distance"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.use_id(LD2410S),
    cv.Optional(CONF_TARGET_DISTANCE): sensor.sensor_schema(),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ID])

    if CONF_TARGET_DISTANCE in config:
        sens = await sensor.new_sensor(config[CONF_TARGET_DISTANCE])
        cg.add(parent.set_target_distance(sens))
