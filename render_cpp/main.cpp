#include "render.h"

//=====================================================================
// ���Գ���
//=====================================================================
vertex_t mesh[8]={//����λ�ã��������꣬ɫ��RGB��rhw��������Ȳ��ԣ��˴�Ϊ��ʼ���� 
	{ {  1, -1,  1, 1 }, { 0, 0 }, { 1.0f, 0.2f, 0.2f }, 1 },
	{ { -1, -1,  1, 1 }, { 0, 1 }, { 0.2f, 1.0f, 0.2f }, 1 },
	{ { -1,  1,  1, 1 }, { 1, 1 }, { 0.2f, 0.2f, 1.0f }, 1 },
	{ {  1,  1,  1, 1 }, { 1, 0 }, { 1.0f, 0.2f, 1.0f }, 1 },
	{ {  1, -1, -1, 1 }, { 0, 0 }, { 1.0f, 1.0f, 0.2f }, 1 },
	{ { -1, -1, -1, 1 }, { 0, 1 }, { 0.2f, 1.0f, 1.0f }, 1 },
	{ { -1,  1, -1, 1 }, { 1, 1 }, { 1.0f, 0.3f, 0.3f }, 1 },
	{ {  1,  1, -1, 1 }, { 1, 0 }, { 0.2f, 1.0f, 0.3f }, 1 },
};

void draw_plane(device_t *device,int a,int b,int c,int d)//��һ��ƽ�� 
{
	vertex_t p1=mesh[a];
	vertex_t p2=mesh[b]; 
	vertex_t p3=mesh[c];
	vertex_t p4=mesh[d];
	p1.tc.u=0;//�ĸ��˵��������� 
	p1.tc.v=0;
	p2.tc.u=0;
	p2.tc.v=1;
	p3.tc.u=1;
	p3.tc.v=1;
	p4.tc.u=1; 
	p4.tc.v=0;
	device_draw_primitive(device,&p1,&p2,&p3);
	device_draw_primitive(device,&p3,&p4,&p1);
}

void draw_box(device_t *device,float theta)//��һ��������
{
	matrix_t m;
	matrix_set_rotate(&m,-1,-0.5,1,theta);//������ת
	device->transform.world=m;
	transform_update(&device->transform);
	draw_plane(device,0,1,2,3);
	draw_plane(device,4,5,6,7);
	draw_plane(device,0,4,5,1);
	draw_plane(device,1,5,6,2);
	draw_plane(device,2,6,7,3);
	draw_plane(device,3,7,4,0);
}

void camera_at_zero(device_t *device,float x,float y,float z)//�����۲�����ϵ
{
	point_t eye={x,y,z,1};//�����λ��
	point_t  at={0,0,0,1};//����������λ��
	point_t  up={0,0,1,1};//��������ϵķ���
	matrix_set_lookat(&device->transform.view,&eye,&at,&up);
	transform_update(&device->transform);
}

void init_texture(device_t *device)//��bmp�ļ��м�������
{
	static IUINT32 texture[256][256];
	loadbmp("images\\texture.bmp",texture);//ֻ֧�ּ���256*256��bmp�ļ�,texture������D:\\2.bmp
	device_set_texture(device,texture,256*4,256,256);
}
 
int main()
{
	device_t device;
	int states[]={RENDER_STATE_TEXTURE,RENDER_STATE_COLOR,RENDER_STATE_WIREFRAME};
	int indicator=0;
	int kbhit=0;
	float alpha=1;
	float pos=3.5;
	light_t light;

	TCHAR *title =_T("software render");
	set_light_color(&light,0.4f,0.44f,0.4f);
	if(screen_init(800,600,title))
	{
		return -1;
	}
	
	device_init(&device,800,600,screen_fb);//��ʼ����Ⱦ�豸 
	camera_at_zero(&device,3,0,0);//���ù۲�����ϵ 
	
	init_texture(&device);//��ʼ������ 
	device.render_state=RENDER_STATE_TEXTURE|RENDER_STATE_LIGHT_GLOBAL_AMBIENT|RENDER_STATE_LIGHT_DIFFUSE_REFLECTION;//����״̬���� 
	
	while(screen_exit==0&&screen_keys[VK_ESCAPE]==0) 
	{
		screen_dispatch();
		device_clear(&device,1);
		camera_at_zero(&device,pos,0,0);
		
		if(screen_keys[VK_UP])
		{
			pos-=0.01f;
		}
		if(screen_keys[VK_DOWN])
		{
			pos+=0.01f;
		}
		if(screen_keys[VK_LEFT])
		{
			alpha+=0.01f;
		}
		if(screen_keys[VK_RIGHT])
		{
			alpha-=0.01f;
		}
		
		if(screen_keys[VK_SPACE])
		{
			if(kbhit==0) 
			{
				kbhit=1;
				if(++indicator>=3)
				{
					indicator=0;
				}
				device.render_state=states[indicator];
			}
		}	
		else
		{
			kbhit = 0;
		}
		
		open_light(&device,&light);
		draw_box(&device,alpha);
		screen_update();
		Sleep(1);
	}
	return 0;
}