#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include <console.h>
#include <sound/aw_types.h>
#include <aw-alsa-lib/common.h>
#include <platform.h>
#include <components/aw/linux_debug/debug_common.h>

#ifdef configSUPPORT_STATIC_ALLOCATION

#ifdef CONFIG_COMPONENTS_AW_ALSA_RPAF
int32_t snd_dsp_audio_remote_process_init(void);
#endif

#ifdef CONFIG_DRIVERS_SOUND
extern int sunxi_soundcard_init(void);
#endif

#ifdef CONFIG_PM_CLIENT_DSP_WAITI
extern int pm_standby_service_init(void);
#endif

/**
 * @brief This is to provide memory that is used by the Idle task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, then the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() in order to provide memory to
 * the Idle task.
 */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
					StackType_t ** ppxIdleTaskStackBuffer,
					uint32_t * pulIdleTaskStackSize )
{
	/*
	 * If the buffers to be provided to the Idle task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits.
	 */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/*
	 * Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored.
	 */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/*
	 * Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes.
	 */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/**
 * @brief This is to provide the memory that is used by the RTOS daemon/time task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, then application must provide an
 * implementation of vApplicationGetTimerTaskMemory() in order to provide memory
 * to the RTOS daemon/time task.
 */
void vApplicationGetTimerTaskMemory(StaticTask_t ** ppxTimerTaskTCBBuffer,
					StackType_t ** ppxTimerTaskStackBuffer,
					uint32_t * pulTimerTaskStackSize )
{
	/*
	 * If the buffers to be provided to the Timer task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits.
	 */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	/*
	 * Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored.
	 */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/*
	 * Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes.
	 */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

#endif


int main(void)
{

#ifdef CONFIG_LINUX_DEBUG
	pr_info_thread("dsp0 debug init ok\n");
	log_mutex_init();
#endif

#ifdef CONFIG_DRIVERS_SOUND
	/* should be init at last. */
	sunxi_soundcard_init();
#endif

#ifdef CONFIG_COMPONENTS_AW_ALSA_RPAF
	snd_dsp_audio_remote_process_init();
#endif

#ifdef CONFIG_COMPONENTS_FREERTOS_CLI
	if (console_uart != UART_UNVALID) {
		vUARTCommandConsoleStart(0x1000, 1);
	}
#endif

#ifdef CONFIG_PM_CLIENT_DSP_WAITI
	pm_standby_service_init();
#endif

	vTaskStartScheduler();

	/* If we got here then scheduler failed. */
	printf( "vTaskStartScheduler FAILED!\n" );

	return 1;
}
