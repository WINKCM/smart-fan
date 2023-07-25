#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#include "esp_log.h"
#include "esp_err.h"

#include "bdc_pcnt.h"

const static char *TAG = "bdc pcnt";

#define BDC_PCNT_HIGH_LIMIT 100
#define BDC_PCNT_LOW_LIMIT -1

#define BDC_PCNT_GLITCH_FILTER_NS 1000

// TODO :设定为正确引脚
#define BDC_PCNT_GPIO_A1 14
#define BDC_PCNT_GPIO_A2 27

void bdc_pcnt_unit_config(pcnt_unit_handle_t *pcnt_unit, int pcnt_high_limit, int pcnt_low_limit)
{
    pcnt_unit_config_t pcnt_unit_config = {
        .high_limit = BDC_PCNT_HIGH_LIMIT,
        .low_limit = BDC_PCNT_LOW_LIMIT,
    };

    pcnt_new_unit(&pcnt_unit_config, pcnt_unit);
}

void bdc_pcnt_glitch_filter_config(pcnt_unit_handle_t pcnt_unit, int glitch_ns)
{
    if (glitch_ns <= 0)
        return;

    pcnt_glitch_filter_config_t pcnt_glitch_filter_config = {
        .max_glitch_ns = glitch_ns,
    };

    pcnt_unit_set_glitch_filter(pcnt_unit, &pcnt_glitch_filter_config);
}

void bdc_pcnt_channel_config(pcnt_unit_handle_t pcnt_unit, int edge_gpio_num, int level_gpio_num)
{
    if (edge_gpio_num < 0 || level_gpio_num < 0)
        return;

    pcnt_chan_config_t pcnt_a_channel_config = {
        .edge_gpio_num = edge_gpio_num,
        .level_gpio_num = level_gpio_num,
    };
    pcnt_channel_handle_t pcnt_a_channel_handle = NULL;
    pcnt_new_channel(pcnt_unit, &pcnt_a_channel_config, &pcnt_a_channel_handle);

    pcnt_chan_config_t pcnt_b_channel_config = {
        .edge_gpio_num = level_gpio_num,
        .level_gpio_num = edge_gpio_num,
    };
    pcnt_channel_handle_t pcnt_b_channel_handle = NULL;
    pcnt_new_channel(pcnt_unit, &pcnt_b_channel_config, &pcnt_b_channel_handle);

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_a_channel_handle, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_a_channel_handle, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_b_channel_handle, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_b_channel_handle, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
}

void bdc_pcnt_config(pcnt_unit_handle_t *pcnt_unit, int edge_gpio_num, int level_gpio_num)
{
    bdc_pcnt_unit_config(pcnt_unit, BDC_PCNT_HIGH_LIMIT, BDC_PCNT_LOW_LIMIT);
    bdc_pcnt_glitch_filter_config(*pcnt_unit, BDC_PCNT_GLITCH_FILTER_NS);
    bdc_pcnt_channel_config(*pcnt_unit, edge_gpio_num, level_gpio_num);
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(*pcnt_unit, BDC_PCNT_HIGH_LIMIT));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(*pcnt_unit, BDC_PCNT_LOW_LIMIT));
    ESP_ERROR_CHECK(pcnt_unit_enable(*pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(*pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(*pcnt_unit));
}

bdc_pcnt_t *bdc_pcnt_create()
{
    bdc_pcnt_t *bdc_pcnt = malloc(sizeof(bdc_pcnt_t));
    bdc_pcnt->pcnt_unit_a = NULL;

    bdc_pcnt_config(&bdc_pcnt->pcnt_unit_a, BDC_PCNT_GPIO_A1, BDC_PCNT_GPIO_A2);

    return bdc_pcnt;
}

int bdc_pcnt_a_get_count(bdc_pcnt_t *bdc_pcnt)
{
    int pcnt_count = 0;
    pcnt_unit_get_count(bdc_pcnt->pcnt_unit_a, &pcnt_count);
    // pcnt_unit_clear_count(bdc_pcnt->pcnt_unit_a);

    return pcnt_count;
}

void bdc_pcnt_delete(bdc_pcnt_t *bdc_pcnt)
{
    if (bdc_pcnt == NULL)
        return;

    pcnt_unit_disable(bdc_pcnt->pcnt_unit_a);

    pcnt_del_unit(bdc_pcnt->pcnt_unit_a);

    free(bdc_pcnt);
}
