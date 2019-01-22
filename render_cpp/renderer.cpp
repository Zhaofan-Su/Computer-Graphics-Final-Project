#include "render.h"

//=====================================================================
// ��Ⱦʵ��
//=====================================================================

//����ɨ����(ɨ��������㷨) 
void device_draw_scanline(device_t *device,scanline_t *scanline)
{
	IUINT32 *framebuffer=device->framebuffer[scanline->y];
	float *zbuffer=device->zbuffer[scanline->y];
	
	int width=device->width;
	int w=scanline->w;
	int x=scanline->x;
	int render_state=device->render_state;
	for(;w>0;x++,w--)
	{
		if(x>=0&&x<width)
		{
			float rhw=scanline->v.rhw;//v.rhw=1/v.w 
			if(rhw>=zbuffer[x])//zbuffer�д洢��Ҳ��rhw��1/w��;w��z����projection���������Թ�ϵ 
			{
				float w=1.0f/rhw;
				zbuffer[x]=rhw;//���z��С�����ӽ��۲��ߣ�������Ȼ������Ӧλ��	
				if(render_state&RENDER_STATE_COLOR)//��λ�ң�ɨ���߻���ʹ����ɫ 
				{
					float r=scanline->v.color.r*w;
					float g=scanline->v.color.g*w;
					float b=scanline->v.color.b*w;
					int R=(int)(r*255.0f);//���������r��[0,1]
					int G=(int)(g*255.0f); 
					int B=(int)(b*255.0f);
					R = CMID(R,0,255);//��ֹ����
					G = CMID(G,0,255);
					B = CMID(B,0,255);
					framebuffer[x]=(R<<16)|(G<<8)|(B);//��ɫ�Ĵ洢�ǣ�0xffffff��ǰff��R����ffΪG����ffΪB
				}
				if(render_state&RENDER_STATE_TEXTURE)//ɨ���߻���ʹ������
				{
					float u=scanline->v.tc.u*w;//��vertex_rhw_init��ȫ���˸�w������Ҫ�����
					float v=scanline->v.tc.v*w;//IUINT32 *framebuffer=device->framebuffer[scanline->y];Ҳ��һ��һ��ȡ������
					IUINT32 cc=device_texture_read(device,u,v); 
					framebuffer[x]=cc;//����Ҳ����ʵ����ɫ���棨֡���棩�е�
				} /*
				if(render_state&RENDER_STATE_LIGHT_DIFFUSE_REFLECTION)
				{
					int R=(int)(device->light.light.r*255.0f);//���������r��[0,1]
					int G=(int)(device->light.light.g*255.0f); 
					int B=(int)(device->light.light.b*255.0f);
					R = CMID(R,0,255);//��ֹ����
					G = CMID(G,0,255);
					B = CMID(B,0,255);
					framebuffer[x]+=(R<<16)|(G<<8)|(B);
				}*/
			}
		}
		vertex_add(&scanline->v,&scanline->step);//�ƶ�ɨ��������Ҫ���Ƶĵ�
		if(x>=width)
		{
			break;
		} 
	}
}

//����Ⱦ���� 
void device_render_trap(device_t *device,trapezoid_t *trap)
{
	scanline_t scanline;
	int top=(int)(trap->top+0.5f);//��Ҫ���Ƶ�ˮƽ�����εĶ���
	int bottom=(int)(trap->bottom+0.5f);
	for(int y=top;y<bottom;y++)
	{
		if(y>=0&&y<device->height)//ɨ��������Ұ��Χ֮��
		{
			trapezoid_edge_interp(trap,(float)y+0.5f);//����Y������������������������� y ����������
			trapezoid_init_scan_line(trap,&scanline,y);//���������˵��ʼ������ɨ���ߵ����Ͳ���
			device_draw_scanline(device,&scanline);//��ɨ���� 
		} 
		if(y>=device->height)//������Ұ��Ĳ���������� 
		{
			break;
		}
	} 
}

//��ԭʼ������ 
void device_draw_primitive(device_t *device,const vertex_t *v1,const vertex_t *v2,const vertex_t *v3)
{
	point_t p1;
	point_t p2;
	point_t p3;
	point_t c1;
	point_t c2;
	point_t c3;
	int render_state=device->render_state;
	
	transform_apply(&device->transform,&c1,&v1->pos);//��v1->pos transform�任�����c1��
	transform_apply(&device->transform,&c2,&v2->pos);//c2�ͱ�֤��cvv���� 
	transform_apply(&device->transform,&c3,&v3->pos); 
	
	//�˴�ֻ�Ǵֲڵ��жϣ�����һ����û��cvv�оͲ������������
	if(transform_check_cvv(&c1)!=0||transform_check_cvv(&c2)!=0||transform_check_cvv(&c3)!=0)
	{
		return;
	} 
	//��һ��
	transform_homogenize(&device->transform,&p1,&c1);
	transform_homogenize(&device->transform,&p2,&c2);
	transform_homogenize(&device->transform,&p3,&c3);
	
	if(render_state&(RENDER_STATE_TEXTURE|RENDER_STATE_COLOR ))//�������ɫ��״̬��������������������� 
	{
		vertex_t t1=*v1;
		vertex_t t2=*v2;
		vertex_t t3=*v3;
		
		trapezoid_t traps[2];
		int n;
		
		t1.pos=p1;
		t2.pos=p2;
		t3.pos=p3;
		t1.pos.w=c1.w;
		t2.pos.w=c2.w;
		t3.pos.w=c3.w;//w��������任���w��������������Ϣ	
		
		vertex_rhw_init(&t1);//����λ�������⣬����ͬ����w����Ϊ�ڼ�����߽���Ĺ����У����w��Ϊ1���˴���������Ϣ�ȳ�w 
		vertex_rhw_init(&t2);
		vertex_rhw_init(&t3);
		//�������ηָ�ɿ���ɨ���ˮƽ������ 
		n=trapezoid_init_triangle(traps,&t1,&t2,&t3); 
		if(n>=1)
		{
			device_render_trap(device,&traps[0]);
		}	
		if(n>=2)
		{
			device_render_trap(device,&traps[1]);//���� 
		}
	} 
	if(render_state&RENDER_STATE_WIREFRAME)//�����߿��״̬�� 
	{
		device_draw_line(device,(int)p1.x,(int)p1.y,(int)p2.x,(int)p2.y,device->foreground);
		device_draw_line(device,(int)p1.x,(int)p1.y,(int)p3.x,(int)p3.y,device->foreground);
		device_draw_line(device,(int)p3.x,(int)p3.y,(int)p2.x,(int)p2.y,device->foreground);	
	}
}