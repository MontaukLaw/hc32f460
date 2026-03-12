#include "user_comm.h"

uint8_t stop_u1 = 0;
__IO uint8_t points_data[TOTAL_POINTS] = {0};
__IO uint8_t points_data_after_proc[TOTAL_POINTS] = {0};

float q_out[4] = {1.0f, 0.0f, 0.0f, 0.0f};
uint8_t tx_data[OLD_FRAME_LEN] = {0};

uint8_t mems_data[16] = {0xF1, 0xFF, 0x7F, 0x3F, 0xE0, 0xB6, 0x2F, 0x3A,
                         0xF0, 0xF0, 0x08, 0xBA, 0x88, 0x7C, 0x84, 0xBA};

void adc_data_handler_with_idx(uint8_t point_nmb)
{
    // 简单计算平均值
    uint32_t adc_sum = 0;
    uint32_t i = 0;

    for (i = 0; i < ADC_BUFFER_SIZE; i++)
    {
        adc_sum += m_au16AdcBuf[i];
    }
    float result = adc_sum / ADC_BUFFER_SIZE;
    points_data[point_nmb] = (uint8_t)result; // 将结果存储到points_data中
}

void uart_send(void)
{

#if USE_PRESS
    memcpy(&tx_data[2], (const void *)points_data_after_proc, TOTAL_POINTS / 2);
    memcpy(&tx_data[TOTAL_POINTS / 2 + 8], (const void *)&points_data_after_proc[TOTAL_POINTS / 2], TOTAL_POINTS / 2);
#else
    memcpy(&tx_data[2], (const void *)points_data, TOTAL_POINTS / 2);
    memcpy(&tx_data[TOTAL_POINTS / 2 + 8], (const void *)&points_data[TOTAL_POINTS / 2], TOTAL_POINTS / 2);
#endif

    // 4元数放在最后16个字节
    memcpy(&tx_data[TOTAL_POINTS + 8], (const void *)q_out, 16);

    // 将数据复制到bl的传输数组中
    memcpy(bl_tx_buf, (const void *)tx_data, OLD_FRAME_LEN);

    // send to u2
    uart2_send_data((uint8_t *)tx_data, OLD_FRAME_LEN);
}

const uint8_t adc_idx_v2[] = {
    11, 10, 9, 8,
    3, 2, 1, 0,
    4, 5, 6, 7,
    12, 13, 14, 15};

const uint8_t input_idx_v2[] = {
    7, 6, 5, 4,
    3, 2, 1, 0,
    15, 14, 13, 12,
    11, 10, 9, 8};

void main_task_adc_first(void)
{
    if (stop_u1)
        return;

    uint16_t input_idx = 0;
    uint16_t adc_idx = 0;
    uint16_t point_nmb = 0;

    for (input_idx = 0; input_idx < INPUT_CH_NUMBER; input_idx++)
    {

        // 先打开GPIO输出
        set_output(input_idx_v2[input_idx], 1);

        for (adc_idx = 0; adc_idx < ADC_CHANNEL_NUMBER; adc_idx++)
        {

            set_adc_ch(adc_idx_v2[adc_idx]);

            // delay_us(10);

            // 开启ADC
            // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer, ADC_BUFFER_SIZE); // != HAL_OK;
            // HAL_Delay(1);
            delay_us(10);
            // adc_busy = 1;
            // while (adc_busy)
            //      ;

            // HAL_GPIO_WritePin(HC4067_EN_GPIO_Port, HC4067_EN_Pin, GPIO_PIN_SET);

            point_nmb = input_idx * ADC_CHANNEL_NUMBER + adc_idx;
            adc_data_handler_with_idx(point_nmb);
        }

        // 关闭GPIO输出
        set_output(input_idx_v2[input_idx], 0);
    }

#if USE_PRESS
    // 数据压缩处理
    press256((const uint8_t *)points_data, (uint8_t *)points_data_after_proc, ADC_CHANNEL_NUMBER, INPUT_CH_NUMBER, 5, BY_COL);

#endif

    uart_send();

    // delay_ms(1000);

    // HAL_Delay(100);
}

void fill_tx_data(void)
{
    // 01 06
    uint16_t first_part_len = TOTAL_POINTS / 2;

    tx_data[0] = 0x01;
    tx_data[1] = DEVICE_TYPE;
    //  AA 55 03 99
    tx_data[first_part_len + 2] = 0xaa;
    tx_data[first_part_len + 3] = 0x55;
    tx_data[first_part_len + 4] = 0x03;
    tx_data[first_part_len + 5] = 0x99;

    tx_data[first_part_len + 6] = 0x02;
    tx_data[first_part_len + 7] = DEVICE_TYPE;

    memcpy(&tx_data[TOTAL_POINTS + 8], mems_data, 16);

    // memcpy((const void *)(tx_data + TOTAL_POINTS + 8), (const void *)points_data, 16);

    tx_data[TOTAL_POINTS + 16 + 8] = 0xaa;
    tx_data[TOTAL_POINTS + 16 + 9] = 0x55;
    tx_data[TOTAL_POINTS + 16 + 10] = 0x03;
    tx_data[TOTAL_POINTS + 16 + 11] = 0x99;
}