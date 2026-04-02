from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor, binary_sensor, text_sensor
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

ld2410s_ns = cg.esphome_ns.namespace("ld2410s")
LD2410S = ld2410s_ns.class_("LD2410S", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LD2410S),
}).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
