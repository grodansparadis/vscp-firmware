/* Stub ESP-IDF esp_log.h for desktop unit-test builds. */
#pragma once
#define ESP_LOGI(tag, fmt, ...) ((void)(tag))
#define ESP_LOGE(tag, fmt, ...) ((void)(tag))
#define ESP_LOGW(tag, fmt, ...) ((void)(tag))
#define ESP_LOGD(tag, fmt, ...) ((void)(tag))
#define ESP_LOGV(tag, fmt, ...) ((void)(tag))
#define ESP_LOG_INFO  3
#define ESP_LOG_WARN  2
#define ESP_LOG_ERROR 1
