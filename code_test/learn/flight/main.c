#include "head.h"

int main(void)
{
	int n,menu;
	while(1)
	{
start:
		main_menu();
		printf(BLUE"pls chioce:"NONE);
		scanf("%d",&n);
		switch(n)
		{
start1:
			case 1:
				menu1();
				printf(BLUE"pls chioce:"NONE);
				scanf("%d",&menu);
				switch(menu)
				{
					case 1:
						goto start1;
					case 2:
						goto start2;
					case 3:
						goto start3;
					case 4:
						goto start;
				}
				break;
start2:   		case 2:
				menu2();
				printf(BLUE"pls chioce:"NONE);
				scanf("%d",&menu);
					switch(menu)
				{
					case 1:
						goto start1;
					case 2:
						goto start2;
					case 3:
						goto start3;
					case 4:
						goto start;
				}
				break;
start3:			case 3:
				menu3();
				printf(BLUE"pls chioce:"NONE);
				scanf("%d",&menu);
				switch(menu)
				{
					case 1:
						goto start1;
					case 2:
						goto start2;
					case 3:
						goto start3;
					case 4:
						goto start;
				}
				break;
start4:			case 4:
				printf(GREEN LIGHT"Byebye!\n"NONE);
				exit(0);
		}
	}
	return 0;
}
