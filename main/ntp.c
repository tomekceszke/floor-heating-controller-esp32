#include <esp_sntp.h>
#include <esp_log.h>
#include "config/config.h"

static const char *TAG = "NTP";

char boot_time[64];

void start_ntp_client(void) {
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "0.pl.pool.ntp.org");
    esp_sntp_setservername(2, "1.pl.pool.ntp.org");

    esp_sntp_init();
    time_t now = 0;
    time(&now);

    u_short i = 0;
    while (now < 5000 && i < NTP_MAX_ATTEMPTS) {
        ESP_LOGI(TAG, "Getting time, attempt: %d", ++i);
        vTaskDelay((NTP_RETRY_DELAY_S * 1000) / portTICK_PERIOD_MS);
        time(&now);
    }

    if (now < 5000) {
        ESP_LOGE(TAG, "Couldn't get current time by NTP. Some features won't work!");
        return;
    }

    struct tm timeinfo = {0};

    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    strftime(boot_time, sizeof(boot_time), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current local date/time is: %s", boot_time);
}




