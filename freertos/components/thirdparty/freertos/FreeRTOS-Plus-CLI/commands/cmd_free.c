#include <FreeRTOS.h>
#include <task.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <console.h>

#include "../FreeRTOS_CLI.h"

int cmd_free(int argc, char ** argv)
{
    uint32_t totalsize = 0;
    uint32_t freesize = 0;
    uint32_t minfreesize = 0;
    uint32_t i = 0;
    uint32_t num = 0;

    if (argc > 2)
        printf("Arguments Error!\nUsage: free <rounds>\n");
    else if (argc == 2)
        num = atoi(argv[1]);
    else
        num = 1;

    for (i = 0; i < num; i ++) {
        char pcHeader[2048] = "Task          State  Priority  Stack      #\r\n************************************************\r\n";
        totalsize = configTOTAL_HEAP_SIZE;
        freesize = xPortGetFreeHeapSize();
        minfreesize = xPortGetMinimumEverFreeHeapSize();

        printf("==> Round [%d] <==\n", i+1);
        printf( "Total Heap Size : %8u Bytes    (%5u KB)\n"
            "           Free : %8u Bytes    (%5u KB)\n"
            "       Min Free : %8u Bytes    (%5u KB)\n",
            totalsize, totalsize >> 10,
            freesize, freesize >> 10,
            minfreesize, minfreesize >> 10);

        printf("\n      List Task MIN Free Stack(unit: word)      \n");
        vTaskList(pcHeader + strlen(pcHeader));
        printf("%s\n\n", pcHeader);

        if (i + 1 != num)
            vTaskDelay(configTICK_RATE_HZ);
    }

    return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_free, free, Free Memory in Heap);
