/*************************************************************************
	> File Name: wave.cpp
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年05月06日 星期三 14时18分07秒
 ************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "wave.h"
 
int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    Wav wav;
    RIFF_t riff;
    FMT_t fmt;
    Data_t data;
	
	//char path[1024] = argv;

    //fp = fopen("/home/renruidong/work/test/rock_github/nod_test/wav_unpack/output.wav", "rb");
	if(argc<2)
	{	
	printf("没有wav文件！！\n");
	}

	fp = fopen(argv[1], "rb");
    if (!fp) {
        printf("can't open audio file\n");
        exit(1);
    }
    fread(&wav, 1, sizeof(wav), fp);
    riff = wav.riff;
    fmt = wav.fmt;
    data = wav.data;
    /**
     *  RIFF
     */
    printf("ChunkID \t\t%c%c%c%c\n", riff.ChunkID[0], riff.ChunkID[1], riff.ChunkID[2], riff.ChunkID[3]);
    printf("ChunkSize \t\t%d\n", riff.ChunkSize);
    printf("Format \t\t\t%c%c%c%c\n", riff.Format[0], riff.Format[1], riff.Format[2], riff.Format[3]);
    printf("\n");
    /**
     *  fmt
     */
    printf("Subchunk1ID \t%c%c%c%c\n", fmt.Subchunk1ID[0], fmt.Subchunk1ID[1], fmt.Subchunk1ID[2], fmt.Subchunk1ID[3]);
    printf("Subchunk1Size \t%d\n", fmt.Subchunk1Size);
    printf("AudioFormat \t%d\n", fmt.AudioFormat);
    printf("NumChannels \t%d\n", fmt.NumChannels);
    printf("SampleRate \t\t%d\n", fmt.SampleRate);
    printf("ByteRate \t\t%d\n", fmt.ByteRate);
    printf("BlockAlign \t\t%d\n", fmt.BlockAlign);
    printf("BitsPerSample \t%d\n", fmt.BitsPerSample);
    printf("\n");
    /**
     *  data
     */
    printf("blockID \t\t%c%c%c%c\n", data.Subchunk2ID[0], data.Subchunk2ID[1], data.Subchunk2ID[2], data.Subchunk2ID[3]);
    printf("blockSize \t\t%d\n", data.Subchunk2Size);
    printf("\n");
//    duration = Subchunk2Size / ByteRate
    printf("duration \t\t%d\n", data.Subchunk2Size / fmt.ByteRate);
}

