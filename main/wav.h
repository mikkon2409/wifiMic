#pragma once
#include "stdio.h"
#include "esp_types.h"
typedef struct
{
    char chunkId[4];
    uint32_t chunkSize;
    char format[4];
    char subchank1Id[4];
    uint32_t subchank1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchank2Id[4];
    uint32_t subchank2Size;
} wav_header;


void init_wav_header(wav_header* dst,
                    uint16_t num_channels,
                    uint32_t sample_rate,
                    uint16_t bits_per_sample);

void update_header_according_data_size(wav_header* dst,
                                        uint32_t data_size);

void write_wav_header(FILE* file,
                        wav_header* header);
