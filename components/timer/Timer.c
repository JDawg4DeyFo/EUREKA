#include "../../include/Timer.h"
#include "esp_log.h"

static const char *TAG = "FreeRunningGPTimer";

gptimer_handle_t GPT_Handle;
static bool Initialized;

static gptimer_config_t GPT_cfg = {
	.clk_src = GPTIMER_CLK_SRC_DEFAULT,
	.direction = GPTIMER_COUNT_UP,
	.resolution_hz = CONFIG_GPT_RESOLUTION,
};

void FreeRunningTimer_Init()
{
	// Check if timer was already initialized
	if (!Initialized) {
		ESP_LOGI(TAG, "Initialziing timer...");
		ESP_ERROR_CHECK(gptimer_new_timer(&GPT_cfg, &GPT_Handle));
		ESP_ERROR_CHECK(gptimer_enable(GPT_Handle));
		ESP_ERROR_CHECK(gptimer_start(GPT_Handle));
		Initialized = true;
	}
	else {
		ESP_LOGW(TAG, "Timer has already been initialized!");
	}
}