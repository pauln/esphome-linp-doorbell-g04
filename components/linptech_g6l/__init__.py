import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import (
    CONF_ID,
    CONF_ADDRESS,
    CONF_COMMAND,
    CONF_MODE,
)
from esphome.core import CORE
from esphome.components.remote_base import (
    declare_protocol,
    register_action,
    register_binary_sensor,
    register_dumper,
    register_trigger,
    remote_base_ns,
)

AUTO_LOAD = ["remote_base"]

CONF_TUNE = "tune"
CONF_VOLUME = "volume"

# Register Linptech G6L Protocol
LinptechG6lData, LinptechG6lBinarySensor, LinptechG6lTrigger, LinptechG6lAction, LinptechG6lDumper = declare_protocol(
    "LinptechG6l"
)
LINPTECH_G6L_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ADDRESS, default=1): cv.hex_int,
    }
)


@register_binary_sensor("linptech_g6l", LinptechG6lBinarySensor, LINPTECH_G6L_SCHEMA)
def linptech_g6l_binary_sensor(var, config):
    cg.add(
        var.set_data(
            cg.StructInitializer(
                LinptechG6lData,
                ("address", config[CONF_ADDRESS]),
            )
        )
    )


@register_trigger("linptech_g6l", LinptechG6lTrigger, LinptechG6lData)
def linptech_g6l_trigger(var, config):
    pass


@register_dumper("linptech_g6l", LinptechG6lDumper)
def linptech_g6l_dumper(var, config):
    pass


@register_action("linptech_g6l", LinptechG6lAction, LINPTECH_G6L_SCHEMA)
async def linptech_g6l_action(var, config, args):
    template_ = await cg.templatable(config[CONF_ADDRESS], args, cg.uint8)
    cg.add(var.set_address(template_))


# Register SZY8039B protocol
Szy8039bData, Szy8039bBinarySensor, Szy8039bTrigger, Szy8039bAction, Szy8039bDumper = declare_protocol(
    "Szy8039b"
)
Szy8039bCommand = remote_base_ns.enum("Szy8039bCommand")
SZY8039B_COMMANDS = {
    "PLAY": Szy8039bCommand.SZY8039B_COMMAND_PLAY,
    "STOP": Szy8039bCommand.SZY8039B_COMMAND_STOP,
}
SZY8039B_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_TUNE, default=1): cv.int_range(min=1, max=40),
        cv.Optional(CONF_VOLUME, default=1): cv.int_range(min=1, max=8),
        cv.Optional(CONF_MODE, default=1): cv.int_range(min=1, max=4),
        cv.Optional(CONF_COMMAND, default="PLAY"): cv.enum(
            SZY8039B_COMMANDS, upper=True, space=""
        ),
    }
)


@register_binary_sensor("szy8039b", Szy8039bBinarySensor, SZY8039B_SCHEMA)
def szy8039b_binary_sensor(var, config):
    cg.add(
        var.set_data(
            cg.StructInitializer(
                Szy8039bData,
                ("tune", config[CONF_TUNE]),
            )
        )
    )


@register_trigger("szy8039b", Szy8039bTrigger, Szy8039bData)
def szy8039b_trigger(var, config):
    pass


@register_dumper("szy8039b", Szy8039bDumper)
def szy8039b_dumper(var, config):
    pass


@register_action("szy8039b", Szy8039bAction, SZY8039B_SCHEMA)
async def szy8039b_action(var, config, args):
    template_ = await cg.templatable(config[CONF_TUNE], args, cg.uint8)
    cg.add(var.set_tune(template_))
    template_ = await cg.templatable(config[CONF_VOLUME], args, cg.uint8)
    cg.add(var.set_volume(template_))
    template_ = await cg.templatable(config[CONF_MODE], args, cg.uint8)
    cg.add(var.set_mode(template_))
    template_ = await cg.templatable(config[CONF_COMMAND], args, cg.uint8)
    cg.add(var.set_command(template_))



linptech_g6l_ns = cg.esphome_ns.namespace("linptech_g6l")
LINPTECH_G6L = linptech_g6l_ns.class_(
    "LinptechG6lComponent", cg.Component
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LINPTECH_G6L),
        }
    )
    .extend(cv.COMPONENT_SCHEMA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
