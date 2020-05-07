/*************************************************************************
	> File Name: wave.h
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年05月06日 星期三 14时12分17秒
 ************************************************************************/

#ifndef RESOLVE_WAV_WAVE_H
#define RESOLVE_WAV_WAVE_H
#include <cstdint>
typedef struct WAV_RIFF {
    /* chunk "riff" */
    char ChunkID[4];     /* "RIFF" */
    /* sub-chunk-size */
    uint32_t ChunkSize;  /* 36 + Subchunk2Size */
    /* sub-chunk-data */
    char Format[4];      /* "WAVE" */
} RIFF_t;
typedef struct WAV_FMT {
    /* sub-chunk "fmt" */
    char Subchunk1ID[4];    /* "fmt " */
    /* sub-chunk-size */
    uint32_t Subchunk1Size; /* 16 for PCM */
    /* sub-chunk-data */
    uint16_t AudioFormat;   /* PCM = 1*/
    uint16_t NumChannels;   /* Mono = 1, Stereo = 2, etc. */
    uint32_t SampleRate;    /* 8000, 44100, etc. */
    uint32_t ByteRate;      /* = SampleRate * NumChannels * BitsPerSample/8 */
    uint16_t BlockAlign;    /* = NumChannels * BitsPerSample/8 */
    uint16_t BitsPerSample; /* 8bits, 16bits, etc. */
} FMT_t;
typedef struct WAV_data {
    /* sub-chunk "data" */
    char Subchunk2ID[4];    /* "data" */
    /* sub-chunk-size */
    uint32_t Subchunk2Size; /* data size */
    /* sub-chunk-data */
//    Data_block_t block;
} Data_t;
//typedef struct WAV_data_block {
//} Data_block_t;
typedef struct WAV_fotmat {
    RIFF_t riff;
    FMT_t fmt;
    Data_t data;
} Wav;

#endif //RESOLVE_WAV_WAVE_H