#include <stdio.h>
#define SIZE 4096

main(argc,argv)
int argc;
char *argv[];
{
	FILE *fp1;

	int i,j,m,p,q;

	char in_buf[SIZE];
	char buf[7][40];

	if (argc!=2) {
		printf("Weather_sort filename\n");
		exit(0);
	}
	

	if ((fp1= fopen(argv[1],"r")) == NULL)
	{
		printf("can't open origalname\n");
		
		exit(0);
	}

	
	i=0;j=0;

	while (fgets(in_buf,SIZE,fp1)!=NULL)
		{
			if (i==17) { 
				printf("\n %s \n\n",in_buf);
				printf("		�� �� �� ��	  �� �ȣ�C��	    �� �� �� ��\n");
				printf("    �� ��  	ҹ ��  �� ��     �� ��  �� ��    ҹ ��       �� ��\n");
				}
			
			
			if ((i>37)&&(i<514))
			{
				j++;
				
				m=strlen(in_buf);
				in_buf[m-1]='\0';
				
				if (j%2==0) 
				{	
					
					for (p=0;p<m;p++)
					{
						if (in_buf[p]!=' ') 
						{
							q=p;
							break;
						}
					}
					
					for (p=q;p<m;p++)
					buf[j/2-1][p-q]=in_buf[p];
					
					
				}
				
				if (i==221) strcpy(buf[0],"���� ");
				else if (i==277) strcpy(buf[0],"̫ԭ ");
				
				if (j==14) 
				{		
					/*printf("%s %s %s %s %s %s\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);*/
					printf("  %8s    %6s %6s %8s%8s  %10s  %10s\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
					j=0;		
					
					printf("\n");
				}
				
				
			}
			
i++;
memset(in_buf,0,SIZE);
	}	
		
	printf("\n\n���������й����������ṩ http://www.nmc.gov.cn\n\n\n");

	fclose(fp1);
	
	exit(0);



}
