obj-y += aw_cmd.o
obj-y += command_api.o domisc.o
obj-$(CONFIG_FREERTOS_CLI_CMD_TEMPLATE) += cmd_template.o
obj-$(CONFIG_FREERTOS_CLI_CMD_MEM_RW) += cmd_md.o cmd_mw.o
obj-$(CONFIG_FREERTOS_CLI_CMD_FREE) += cmd_free.o

CFLAGS_domisc.o += -Icomponents/thirdparty/freertos/FreeRTOS-Plus-CLI/
