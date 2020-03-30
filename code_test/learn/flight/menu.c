#include "head.h"

void main_menu(void)
{
		system("clear");
		printf(RED"***************************************\n"NONE);
		printf( RED"**"NONE BLUE"           航班查询系统            "NONE RED"**\n");
		printf("**                                   **\n");
		printf("**	1.录入信息\t2.查询信息   **\n");
		printf("**	3.预览信息\t4.退出       **\n");
		printf("**                                   **\n");
		printf("***************************************\n"NONE);
}
void menu1(void)
{
				system("clear");
				printf(GREEN"--->录入信息\n"NONE);
				printf(BLUE"**********************************************\n");
				printf("**	1.录入信息\t2.查询信息          **\n");
				printf("**	3.预览信息\t4.返回上一级菜单    **\n");
				printf("**********************************************\n"NONE);
}
void menu2(void)
{
				system("clear");
				printf(GREEN"--->查询信息\n"NONE);
				printf(BLUE"*********************************************************\n");
				printf("**	1.按起飞时间查询\t2.按起飞地点查询       **\n");
				printf("**	3.按航班号查询  \t4.返回上一级菜单       **\n");
				printf("*********************************************************\n"NONE);
}
void menu3(void)
{
				system("clear");
				printf(GREEN"--->预览信息\n"NONE);
				printf(BLUE"************************************************\n");
				printf("**	1.录入信息\t2.查询信息            **\n");
				printf("**	3.预览信息\t4.返回上一级菜单      **\n");
				printf("************************************************\n"NONE);
}
