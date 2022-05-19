#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include <xtensa/xtbsp.h>
#include "xtensa_timer.h"
#include "spinlock.h"

static unsigned int div_of_us_cycle = 400000000 / 1000000;

void msleep(unsigned int ms)
{
	int tick = pdMS_TO_TICKS(ms) ?: 1;

	vTaskDelay(tick);

	return;
}

void udelay(unsigned int us)
{
	unsigned expiry = xthal_get_ccount() + div_of_us_cycle * us;
	while( (long)(expiry - xthal_get_ccount()) > 0 );

	return;
}
