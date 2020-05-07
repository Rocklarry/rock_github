
#include <stdio.h>
#include <string.h>


#include "D7_FW.h"


#define DONT_SKIP_BYTES 0
#define DATA_CHUNK	32768

int D7_load_file(const char *file, unsigned long fileLen, int skip_bytes)
{
	uint8_t *buffer;
	uint32_t i;

	uint8_t *checksum;
	int rc;
    char clr_crc_d7[]= {0xDB, 0xD7, 0x00, 0x0F};
    int fw_checksum = 0;			// holds calculated checksum

	uint16_t d7db;
	uint8_t opcode;
	uint8_t N;
	uint32_t blk_length;
	uint32_t opcode_blk_len;
	uint32_t N2;
	uint32_t cur_pos = 0;


	/** Allocate memory */
	buffer = (unsigned char *)file;

	if(rc < 0) {
			printf("[D7_load_file]: fail to write clr_crc command");
			return -1;
	}

	if(skip_bytes > 0) {
		checksum = &buffer[fileLen - skip_bytes];
	} else {
		int tmp_i32 = 0;
		printf( "[%s]: calculating checksum from file  fileLen=%ld \n", __func__,fileLen );
		for(i = 0; i < fileLen; i+= sizeof(int)) {
			memcpy(&tmp_i32, buffer+i, sizeof(int));
			fw_checksum += tmp_i32;
		}
		checksum = (uint8_t*)&fw_checksum;
	}

	while(cur_pos < fileLen) {
		N2 = 0;
		printf("[%s]: cur_pos = %d.\n", __func__, cur_pos);
		opcode = *((uint8_t*)&buffer[cur_pos + 3]);
		printf("[%s]: opcode = %d.\n", __func__, opcode);
		N = (*((uint8_t*)&buffer[cur_pos + 2])) & 0xF;
		printf("[%s]: N = %d.\n", __func__, N);
		
		d7db = *((uint16_t*)&buffer[cur_pos + 0]);
		printf("[%s]: d7db = 0x%02X.\n", __func__, d7db);

		blk_length = *((uint32_t*)&buffer[cur_pos + 4]);
		printf("[%s]: blk_length = %d.\n", __func__, blk_length);
		printf("*******************************\n");

		if(d7db == 0xD7DB) {
			if (opcode == 0) {
				opcode_blk_len = 8;
				N2 = blk_length;
			} else if(opcode == 1 || opcode == 4 || opcode == 5) {
				opcode_blk_len = 8;
			} else if(opcode == 2) {
				opcode_blk_len = 12;
				N2 = blk_length;
			} else if(opcode == 3) {
				opcode_blk_len = 12;
			} else if(opcode == 6 || opcode == 8) {
				opcode_blk_len = 4;
				N2 = 4*N;
			} else {
				opcode_blk_len = 4; // 7, 9, A-F
			}
		} else {
			printf("error loading file\n");
			return -1;
		}

		printf("[%s]: opcode_blk_len = %d.\n", __func__, opcode_blk_len);
		printf("[%s]: N2 = %d.\n", __func__, N2);
		printf("*******************************\n");

		if(rc < 0 ) {
			printf ("error loading opcode_blk_len\n");
			return -1;
		}
		cur_pos += opcode_blk_len;

		if(N2 > 0) {
			for(i = 0 ; i < N2; i += DATA_CHUNK) {
				int chunk;
				chunk = (DATA_CHUNK < (N2 - i) ? DATA_CHUNK : N2 - i);
				printf("  write  ====");
				if(rc < 0)
				{
					printf("error loading file\n");
					return -1;
				}
			}
		}
		cur_pos += N2;
	} 

	printf("[%s]: cur_pos = %d.\n", __func__, cur_pos);

	printf("[%s]: Finish.\n", __func__);
	return 0;
}

static int D7_load_fw(const char * file, unsigned long fileLen,  int skip_bytes)
{
	int fail_cnt = 0;

	while(fail_cnt < 5) {
		printf("[%s]: Start uploading binary to D7.\n", __func__);
		if(D7_load_file(file, fileLen,  skip_bytes) < 0) {
			fail_cnt++;
			if(fail_cnt == 5) {
				printf ("[%s]: Failed uploading FirmWare to D7.\n",__func__);
			}
		}
		else
			break;
	}
	return 0;;
}

int main(void)
{
	printf("[%s]: D7_FW ************************ FirmWare to D7.\n", __func__);
	if (D7_load_fw(D7_FW, sizeof(D7_FW),  DONT_SKIP_BYTES) < 0)
		printf("[%s]: FAiled uploading FirmWare to D7.\n", __func__);

	printf("[%s]: Addon_ASRP ************************ FirmWare to D7.\n", __func__);
	//if (D7_load_fw(Addon_ASRP, sizeof(Addon_ASRP),  DONT_SKIP_BYTES) < 0)
		printf("[%s]: FAiled uploading FirmWare to D7.\n", __func__);

	printf("[%s]: Addon_Pomelo ************************ FirmWare to D7.\n", __func__);
	//if (D7_load_fw(Addon_Pomelo, sizeof(Addon_Pomelo),  DONT_SKIP_BYTES) < 0)
		printf("[%s]: FAiled uploading FirmWare to D7.\n", __func__);


	uint32_t blk_length = 0x30;
	uint8_t  test1=0x01;
	uint8_t  test2=0x00;
	uint8_t  test3=0x00;
	printf("= %d.\n",blk_length);//80

	blk_length=test1<<8|blk_length;
	printf("= %d.\n",blk_length);//80

	blk_length=test2<<16|blk_length;
	printf("= %d.\n",blk_length);//80

	blk_length=test3<<24|blk_length;
	printf("= %d.\n",blk_length);//80


	uint16_t d7db = 0x50;
	d7db=0x00<<8|d7db;
	printf("= %d.\n",d7db);//80


	return 0;
}
	