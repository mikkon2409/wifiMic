#include "wav.h"

void init_wav_header(wav_header* dst,
                    uint16_t num_channels,
                    uint32_t sample_rate,
                    uint16_t bits_per_sample) {
    dst->chunkId[0] = 'R';
    dst->chunkId[1] = 'I';
    dst->chunkId[2] = 'F';
    dst->chunkId[3] = 'F';
    // file size - 8
    dst->chunkSize = sizeof(wav_header) - 8;
    
    dst->format[0] = 'W';
    dst->format[1] = 'A';
    dst->format[2] = 'V';
    dst->format[3] = 'E';

    dst->subchank1Id[0] = 'f';
    dst->subchank1Id[1] = 'm';
    dst->subchank1Id[2] = 't';
    dst->subchank1Id[3] = ' ';

    dst->subchank1Size = 16;
    dst->audioFormat = 1;
    dst->numChannels = num_channels;
    dst->sampleRate = sample_rate;
    dst->byteRate = num_channels * sample_rate * (bits_per_sample / 8);
    dst->blockAlign = num_channels * (bits_per_sample / 8);
    dst->bitsPerSample = bits_per_sample;
    dst->subchank2Id[0] = 'd';
    dst->subchank2Id[1] = 'a';
    dst->subchank2Id[2] = 't';
    dst->subchank2Id[3] = 'a';
    dst->subchank2Size = 0;
}

void update_header_according_data_size(wav_header* dst,
                                        uint32_t data_size) {
    dst->subchank2Size = data_size;
    dst->chunkSize = sizeof(wav_header) + data_size - 8;
}

void write_wav_header(FILE* file,
                        wav_header* header) {
    fpos_t pos;
    fgetpos(file, &pos);
    rewind(file);
    fwrite(header, sizeof(wav_header), 1, file);
    fsetpos(file, &pos);
}