/* Includes ------------------------------------------------------------------*/
#include "wifi.h"
#include <string.h>
#include "usart.h"
#include "mqtt.h"

static uint8_t wifi_calc_checksum(const uint8_t *data, uint8_t len)
{
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        checksum ^= data[i];
    }
    return checksum;
}

void wifi_send_packet(uint8_t type, const uint8_t *data, uint8_t len)
{
    uint8_t pkt[256];
    if (len > 250) return;

    pkt[0] = PKT_HEADER1;
    pkt[1] = PKT_HEADER2;
    pkt[2] = type;
    pkt[3] = len;
    memcpy(&pkt[4], data, len);
    pkt[4 + len] = wifi_calc_checksum(&pkt[2], len + 2); // Checksum covers Type, Len, and Data

    HAL_UART_Transmit(&huart1, pkt, len + 5, 100);
}

void wifi_process_rx(const uint8_t *data, uint16_t size)
{
    if (size < PKT_MIN_LEN) return;

    for (uint16_t i = 0; i <= size - PKT_MIN_LEN; i++)
    {
        if (data[i] == PKT_HEADER1 && data[i+1] == PKT_HEADER2)
        {
            uint8_t type = data[i+2];
            uint8_t len = data[i+3];

            if (i + 4 + len >= size) continue; // Incomplete packet

            uint8_t checksum = data[i + 4 + len];
            if (checksum == wifi_calc_checksum(&data[i+2], len + 2))
            {
                const uint8_t *payload = &data[i+4];
                switch (type)
                {
                    case PKT_TYPE_STATUS:
                    {
                        status_packet_t *status = (status_packet_t *)payload;
                        net.wifi_status = status->wifi_status;
                        net.mqtt_status = status->mqtt_status;
                        if (PRINTTEST1) printf_U3("[WIFI] Status Sync: WiFi=%d, MQTT=%d\r\n", net.wifi_status, net.mqtt_status);
                        break;
                    }
                    case PKT_TYPE_TIME:
                    {
                        time_sync_packet_t *ts = (time_sync_packet_t *)payload;
                        RTC_DateTypeDef sDate;
                        RTC_TimeTypeDef sTime;
                        sDate.Year = ts->year % 100;
                        sDate.Month = ts->month;
                        sDate.Date = ts->day;
                        sTime.Hours = ts->hour;
                        sTime.Minutes = ts->min;
                        sTime.Seconds = ts->sec;
                        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                        if (PRINTTEST1) printf_U3("[WIFI] Time Sync: %04d-%02d-%02d %02d:%02d:%02d\r\n", ts->year, ts->month, ts->day, ts->hour, ts->min, ts->sec);
                        break;
                    }
                    default:
                        if (PRINTTEST1) printf_U3("[WIFI] Unknown pkt type: 0x%02X\r\n", type);
                        break;
                }
                i += 4 + len; // Skip processed packet
            }
        }
    }
}

