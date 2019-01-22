#include "render.h"

//=====================================================================
// ��Ⱦ�豸
//=====================================================================

//�豸��ʼ����fbΪ�ⲿ֡���棬��NULL�������ⲿ����֡���棨ÿ�ж���4�ֽڣ�
void device_init(device_t *device,int width,int height,void *fb)
{
	//����˵void*��Ϊû��ָ���������ͣ������޷�����ָ������С��
	//�˴�sizeof(void*)�Ĵ�С���������Ŀ��ƽ̨�йأ����Ŀ��ƽ̨��32λ�ģ���ôsizeof(void*)�Ĵ�СΪ4�ֽ�
	//ָ��ı��ʾ����ڴ��ַ�����ָ���С���ڴ�ռ䣬����ַλ���йء� 
	char *framebuf;
	char *zbuf;
	 
	int n=sizeof(void*)*(height*2+1024)+width*height*8;//width*height*8����Ļ���ظ�����ÿ������8λ��256ɫ�� 
	char *ptr=(char*)malloc(n+64);
	assert(ptr);//assert��������������ʽΪFalse����ô����stderr��ӡһ��������Ϣ��Ȼ��ͨ��abort����ֹ����
	
	device->framebuffer=(IUINT32**)ptr; //ptr=0;
	device->zbuffer=(float**)(ptr+sizeof(void*)*height);//ptr=0;
	ptr+=sizeof(void*)*height*2;
	device->texture=(IUINT32**)ptr;
	ptr+=sizeof(void*)*1024;
	framebuf=(char*)ptr;
	zbuf=(char*)ptr+width*height*4;
	ptr+=width*height*8;
	if(fb!=NULL)//������ⲿ֡���棬�������ⲿ֡����
	{
		framebuf=(char*)fb;
	} 
	for(int j=0;j<height;j++)
 	{
		device->framebuffer[j]=(IUINT32*)(framebuf+width*4*j);//��֡�����ַ����framebuffer
		device->zbuffer[j]=(float*)(zbuf+width*4*j);//����Ȼ����ַ����zbuffer
	}
	device->texture[0]=(IUINT32*)ptr;
	device->texture[1]=(IUINT32*)(ptr+16);
	memset(device->texture[0],0,64);
	device->tex_width=2;
	device->tex_height=2;
	device->max_u=1.0f;
	device->max_v=1.0f;
	device->width=width;
	device->height=height;
	device->background=0xc0c0c0;
	device->foreground=0;
	transform_init(&device->transform,width,height);
	device->render_state=RENDER_STATE_WIREFRAME; 
}

//���framebuffer��zbuffer
void device_clear(device_t *device,int mode)
{
	int height=device->height;
	int y;
	for(y=0;y<device->height;y++)
 	{
 		//ʹ�������ɫʹ�õ��ǲ�ͬ�Ҷȣ�����ɫ���ǻ�ɫ����ɫ��ǿ�Ȳ�ͬ���ѣ� 
		IUINT32 cc=(height-1-y)*230/(height-1);//cc���ʹ�õ���ɫ��
		//rgb��256ɫ��8λ��ʾһ��ɫ��r/g/b��������16λ������8λ������0λ��������ɫ�ϲ��ó���ȷ�������ɫ
		//cc���ڴ洢��ɫ����32λ��ǰ8λ�գ���24λ�ֱ�洢R��G��B��ɫ���˴����һ��8λ���ֺ�������16��8λ��RGB��ֵ��ͬ��ccΪһ�ֻҶ���ɫ��ȥ��ͬ��cc��Ҳ���ǻҶȴ�С��ͬ 
		cc=(cc<<16)|(cc<<8)|cc;
		if(mode==0)
		{
			cc=device->background;
		} 
		IUINT32 *dst=device->framebuffer[y];
		for(int x=device->width;x>0;dst++,x--) 
		{
			dst[0]=cc;
		}
	}
	for(y=0;y<device->height;y++)
 	{
		float *dst=device->zbuffer[y];
		for(int x=device->width;x>0;dst++,x--)
		{
			dst[0]=0.0f;
		} 
	}
}

//���õ�ǰ����
void device_set_texture(device_t *device,void *bits,long pitch,int w,int h) 
{
	//bitsΪԭ������ʼ��ַ
	//pitchΪһ����������β֮�����
	//pitch����Ϊlong�ͣ�32λ��Ϊ4�ֽڣ�64λ��Ϊ8�ֽڣ����ڴ��ַλ����ͬ
	//��Ҫ����ɿ��Ϊw��h������ 
	char *ptr=(char*)bits;
	assert(w<=1024&&h<=1024);
	for(int j=0;j<h;ptr+=pitch,j++)//���¼���ÿ���������۶������������С������
	{
		device->texture[j]=(IUINT32*)ptr;//����Ĵ洢��ַ 
	}
	device->tex_width=w;
	device->tex_height=h;
	device->max_u=(float)(w-1);
	device->max_v=(float)(h-1);
}

//ɾ���豸 
void device_destroy(device_t *device) 
{
	if(device->framebuffer)
	{
		free(device->framebuffer);
	} 
	device->framebuffer=NULL;
	device->zbuffer=NULL;
	device->texture=NULL;
}

//���� 
void device_pixel(device_t *device,int x,int y,IUINT32 color) 
{
	if(((IUINT32)x)<=(IUINT32)device->width&&((IUINT32)y)<=(IUINT32)device->height) 
	{
		device->framebuffer[y][x]=color;
	}
}

//���߶� 
void device_draw_line(device_t *device,int x1,int y1,int x2,int y2,IUINT32 c) 
{
	int x;
	int y;
	int rem=0;
	if(x1==x2&&y1==y2)//ֻ��һ���� 
 	{
		device_pixel(device,x1,y1,c);
	}	
	else if(x1==x2)//��һ��ֱ���� 
	{
		int inc=(y1<=y2)?1:-1;
		for(y=y1;y!=y2;y+= inc)
		{
			device_pixel(device,x1,y,c);
		} 
		device_pixel(device,x2,y2,c);
	}	
	else if(y1==y2) 
	{
		int inc=(x1<=x2)?1:-1;
		for (x=x1;x!=x2;x+=inc) 
		{
			device_pixel(device,x,y1,c);
		}
		device_pixel(device,x2,y2,c);
	}	
	else//û��һ��ֱ���ϣ��е㻭���㷨 
	{
		int dx=(x1<x2)?x2-x1:x1-x2;
		int dy=(y1<y2)?y2-y1:y1-y2;
		if(dx>=dy)//dx>dy������Ϊx 
		{
			if(x2<x1)
			{
				x=x1;
	  	 		x1=x2;
	  	 		x2=x;
	  	 		y=y1;
   			 	y1=y2;
			 	y2=y;
			}
			for(x=x1,y=y1;x<=x2;x++)
		 	{
				device_pixel(device,x,y,c);
				rem+=dy;
				if(rem>=dx)//б�ʳ���1��y��Ҫ+1����-1����y1��y2�Ĵ�С������ 
				{
					rem-=dx;
					y+=(y2>=y1)?1:-1;
					device_pixel(device,x,y,c);
				}
			}
			device_pixel(device,x2,y2,c);
		}	
		else//dy>dx������Ϊy 
		{
			if(y2<y1) 
			{
				x=x1;
				x1=x2;
				x2=x;
				y=y1;
				y1=y2;
				y2=y;
			} 
			for(x=x1,y=y1;y<=y2;y++)
		 	{
				device_pixel(device,x,y,c);
				rem+=dx;
				if(rem>=dy) 
				{
					rem-=dy;
					x+=(x2>=x1)?1:-1;
					device_pixel(device,x,y,c);
				}
			}
			device_pixel(device,x2,y2,c);
		}
	}
}

//���������ȡ���� 
IUINT32 device_texture_read(const device_t *device,float u,float v) 
{
	int x;
	int y;
	u=u*device->max_u;//max_u��ָ���������ȣ�u�����������꣬����0~1����ֵ 
	v=v*device->max_v;
	x=(int)(u+0.5f);//�������� 
	y=(int)(v+0.5f);
	x=CMID(x,0,device->tex_width-1);//���x�����˱߽磬��ѡ�߽磻���xû�г����߽磬��ѡx 
	y=CMID(y,0,device->tex_height-1);
	return device->texture[y][x];//y�ڼ��У�x�ڼ��� 
}