#include <stdio.h>
#include <console.h>
#include <FreeRTOS.h>
#include <FreeRTOS_CLI.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

static int cmd_help(int argc, char ** argv)
{
    printf("Lists all the registered commands\n");
    printf("\n");

    typedef struct xCOMMAND_INPUT_LIST
    {
        const CLI_Command_Definition_t *pxCommandLineDefinition;
        struct xCOMMAND_INPUT_LIST *pxNext;
    } CLI_Definition_List_Item_t;

    extern CLI_Definition_List_Item_t xRegisteredCommands;
    CLI_Definition_List_Item_t * pxCommand = &xRegisteredCommands;
    while(pxCommand != NULL)
    {
        printf("[%20s]--------------%s\n",
            pxCommand->pxCommandLineDefinition->pcCommand,
            pxCommand->pxCommandLineDefinition->pcHelpString);
        printf("\n");
        pxCommand = pxCommand->pxNext;
    }

    finsh_syscall_show();

    return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_help, help, List all registered commands);

#if 0
int cmd_reg_write(int argc, char ** argv)
{
    uint32_t reg_addr, reg_value ;
    char *err = NULL;

    if(argc < 3)
    {
        printf("Argument Error!\n");
        return -1;
    }

    if ((NULL == argv[1]) || (NULL == argv[2]))
    {
        printf("Argument Error!\n");
        return -1;
    }

    reg_addr = strtoul(argv[1], &err, 0);
    reg_value = strtoul(argv[2], &err, 0);

    if(xport_is_valid_address((void *)reg_addr, NULL))
    {
        *((volatile uint32_t *)(reg_addr)) = reg_value;
    }
    else
    {
        printf("Invalid address!\n");
    }

    return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_reg_write, reg_write, write value to register: reg_write reg_address reg_value);

int cmd_reg_read(int argc, char ** argv)
{
    uint32_t reg_addr  ;
    char *err = NULL;
    uint32_t start_addr, end_addr ;
    uint32_t len;

    if (NULL == argv[1])
    {
        printf("Argument Error!\n");
        return -1;
    }

    if (argv[2])
    {
        start_addr = strtoul(argv[1], &err, 0);

        len = strtoul(argv[2], &err, 0);
        end_addr = start_addr + len;

        if(xport_is_valid_address((void *)start_addr, (void *)end_addr) && end_addr != 0)
        {
            printf("start_addr=0x%08x end_addr=0x%08x\n", start_addr, end_addr);
            for (; start_addr <= end_addr;)
            {
                printf("reg_addr[0x%08x]=0x%08x \n", start_addr, *((volatile uint32_t *)(start_addr)));
                start_addr += 4;
            }
        }
        else
        {
            printf("Invalid address!\n");
        }
    }
    else
    {
        reg_addr = strtoul(argv[1], &err, 0);
        if(xport_is_valid_address((void *)reg_addr, NULL))
        {
            printf("reg_addr[0x%08x]=0x%08x \n", reg_addr, *((volatile uint32_t *)(reg_addr)));
        }
        else
        {
            printf("Invalid address!\n");
        }
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_reg_read, reg_read, write value to register: reg_read reg_start_addr len);
#endif
