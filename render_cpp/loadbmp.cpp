#include "render.h"

void loadbmp(const char *szfilename,IUINT32 t[256][256]) //texture[256][256]�������ɫֵ��0xffffff��ʽ
{  
    //���ļ�   
	int iwidth;
	int iheight;
    FILE *pfile=fopen(szfilename,"rb");  
	unsigned char B;
	unsigned char G;
	unsigned char R;
    if(pfile==0)  
    {  
        exit(0);  
    }
    fseek(pfile,0x0012,SEEK_SET);
    fread(&iwidth,sizeof(int),1,pfile);   
    fseek(pfile,0x0016,SEEK_SET);
	fread(&iheight,sizeof(int),1,pfile);   
    
	int filepos=54;
	for(int i=0;i<iwidth;i++)
	{
		for(int j=0;j<iheight;j++)
		{
			fseek(pfile,filepos,SEEK_SET);
			fread(&B,1,1,pfile);
			fseek(pfile,filepos+1,SEEK_SET);
			fread(&G,1,1,pfile);
			fseek(pfile,filepos+2,SEEK_SET);
			fread(&R,1,1,pfile);
			filepos+=3;

			int r=(int)R;
			int g=(int)G;
			int b=(int)B;
			t[i][j]=(r<<16)|(g<<8)|b;
			
		}
	} 
    //�ر��ļ�  
    fclose(pfile);
}  