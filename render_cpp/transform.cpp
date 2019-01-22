#include"render.h"

//=====================================================================
// ����任
//=====================================================================

void transform_update(transform_t *ts)
{
	matrix_t m;
	matrix_mul(&m,&ts->world,&ts->view);
	matrix_mul(&ts->transform,&m,&ts->projection);
} 

//��ʼ�� 
void transform_init(transform_t *ts,int width,int height)
{
	float aspect=(float)width/(float)height;
	matrix_set_identity(&ts->world);
	matrix_set_identity(&ts->view);
	matrix_set_perspective(&ts->projection,PI*0.5f,aspect,1.0,500.0f);
	ts->w=(float)width;
	ts->h=(float)height;
	transform_update(ts);	
}

//��������x������任��CVV�У�����ü��������ж��Ƿ���CVV�У� 
void transform_apply(const transform_t *ts,vector_t *y,const vector_t *x)
{
	matrix_apply(y,x,&ts->transform);//�任����������y�� 
}

//����������ͬCVV�ı߽�������׶��Ĳü�
int transform_check_cvv(const vector_t *v) 
{
	float w=v->w;//�淶����wӦ����1 
	int check=0;
	if (v->z<0.0f) 
		check|=1;//��check��1����λ����� 
	if (v->z>w) 
		check|=2;
	if (v->x<-w) 
		check|=4;
	if (v->x>w) 
		check|=8;
	if (v->y<-w) 
		check|=16;
	if (v->y>w) 
		check|=32;
	return check;//����һ��check�Ϳ����жϳ�6λ������� 
}

//��һ���õ���Ļ����
void transform_homogenize(const transform_t *ts,vector_t *y,const vector_t *x) 
{
	float rhw=1.0f/x->w;
	y->x=(x->x*rhw+1.0f)*ts->w*0.5f;
	y->y=(1.0f-x->y*rhw)*ts->h*0.5f;
	y->z=x->z*rhw;
	y->w=1.0f;
} 