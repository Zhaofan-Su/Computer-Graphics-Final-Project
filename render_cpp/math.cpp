#include "render.h"

//=====================================================================
// �����;������
//=====================================================================

int CMID(int x,int min,int max) 
{ 
	return (x<min)?min:((x>max)?max:x); 
}

void vector_add(vector_t *z,const vector_t *x,const vector_t *y)
{
	z->x=x->x+y->x;
 	z->y=x->y+y->y;
 	z->z=x->z+y->z;
 	z->w=1.0f;
}

void vector_sub(vector_t *z,const vector_t *x,const vector_t *y)
{
	z->x=x->x-y->x;
 	z->y=x->y-y->y;
 	z->z=x->z-y->z;
 	z->w=1.0f;
}

float vector_dotproduct(const vector_t *x,const vector_t *y)
{
	return x->x*y->x+x->y*y->y+x->z*y->z;	//�任������n+1ά�ģ���������һά��Ϊ����Ӧ�任���� 
}

void vector_crossproduct(vector_t *z,const vector_t *x,const vector_t *y)
{
	z->x=x->y*y->z - x->z*y->y;
	z->y=x->z*y->x - x->x*y->z;
	z->z=x->x*y->y - x->y*y->x;
	z->w=1.0f;
}

float interp(float x,float y,float t)
{
	return x+(y-x)*t;
}

void vector_interp(vector_t *z,const vector_t *x,const vector_t *y,float t)
{
	z->x=interp(x->x,y->x,t);
	z->y=interp(x->y,y->y,t);
	z->z=interp(x->z,y->z,t);
	z->w=1.0f;
}

void vector_normalize(vector_t *v)
{
	float length=(float)sqrt(v->x*v->x+v->y*v->y+v->z*v->z);
	if(length!=0)
	{
		v->x=v->x/length;
		v->y=v->y/length;
		v->z=v->z/length;	
	}
}

void matrix_add(matrix_t *c,const matrix_t *a,const matrix_t *b)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			c->m[i][j]=a->m[i][j]+b->m[i][j];
		}
	}
}

void matrix_sub(matrix_t *c,const matrix_t *a,const matrix_t *b)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			c->m[i][j]=a->m[i][j]-b->m[i][j];
		}
	}
}

void matrix_mul(matrix_t *c,const matrix_t *a,const matrix_t *b)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			c->m[i][j]= (a->m[i][0]*b->m[0][j])+
						(a->m[i][1]*b->m[1][j])+
						(a->m[i][2]*b->m[2][j])+
						(a->m[i][3]*b->m[3][j]);
		}
	}
}

void matrix_scale(matrix_t *c,const matrix_t *a,float f)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			c->m[i][j]=a->m[i][j]*f;
		}
	}
}

//��������һ���������ϲ�����г˵���һ��tranfrom���󣬽�����ת����cvv�� 
void matrix_apply(vector_t *y,const vector_t *x,const matrix_t *m)
{
	y->x=x->x*m->m[0][0]+x->y*m->m[1][0]+x->z*m->m[2][0]+x->w*m->m[3][0]; 
	y->y=x->x*m->m[0][1]+x->y*m->m[1][1]+x->z*m->m[2][1]+x->w*m->m[3][1];
	y->z=x->x*m->m[0][2]+x->y*m->m[1][2]+x->z*m->m[2][2]+x->w*m->m[3][2];
	y->w=x->x*m->m[0][3]+x->y*m->m[1][3]+x->z*m->m[2][3]+x->w*m->m[3][3];
} 

void matrix_set_identity(matrix_t *m)
{
	//m����Ϊģ����ͼ���� 
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			m->m[i][j]=0;
		}
	}
	m->m[0][0]=1.0f;
	m->m[1][1]=1.0f;
	m->m[2][2]=1.0f;
	m->m[3][3]=1.0f;
}

void matrix_set_zero(matrix_t *m)
{
	//m����Ϊģ����ͼ���� 
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			m->m[i][j]=0;
		}
	}
}

//����ƽ��
void matrix_set_translate(matrix_t *m,float x,float y,float z)
{
	//m����Ϊģ����ͼ���� 
	matrix_set_identity(m);					//ǰ3��Ϊ��ת�������� 
	m->m[3][0]=x;							//���һ��Ϊƽ������ 
	m->m[3][1]=y;							//ʹ��������꣨3ά����Ϊ4ά���Ժϲ���ת���ž����ƽ�������� 
	m->m[3][2]=z;
}

//�������� 
void matrix_set_scale(matrix_t *m,float x,float y,float z)
{
	//m����Ϊģ����ͼ���� 
	matrix_set_identity(m);
	m->m[0][0]=x;
	m->m[1][1]=y;
	m->m[2][2]=z;
} 

//������ת 
void matrix_set_rotate(matrix_t *m,float x,float y,float z,float theta)
{
	//m����Ϊģ����ͼ���� 
	float qsin=(float)sin(theta*0.5f);
	float qcos=(float)cos(theta*0.5f);
	vector_t vec={x,y,z,1.0f};//(0,0,0)->(x,y,z)Ϊ��ת�� 
	float w=qcos;
	vector_normalize(&vec);
	x=vec.x*qsin;
	y=vec.y*qsin;
	z=vec.z*qsin;
	//��Ϊ��ת���е����ԭ�㣬�������ƽ���ˣ�ֱ����ת������z���غϣ���תtheta�ǣ��ٷ���������������� 
	m->m[0][0]=1-2*y*y-2*z*z;//ֻ��֤�˸õ�  
	m->m[1][0]=2*x*y-2*w*z;
	m->m[2][0]=2*x*z+2*w*y;
	m->m[0][1]=2*x*y+2*w*z;
	m->m[1][1]=1-2*x*x-2*z*z;
	m->m[2][1]=2*y*z-2*w*x;
	m->m[0][2]=2*x*z-2*w*y;
	m->m[1][2]=2*y*z+2*w*x;
	m->m[2][2]=1-2*x*x-2*y*y;
	m->m[0][3]=m->m[1][3]=m->m[2][3]=0.0f;
	m->m[3][0]=m->m[3][1]=m->m[3][2]=0.0f;	
	m->m[3][3]=1.0f;
}

//���ù۲�����ϵ 
void matrix_set_lookat(matrix_t *m,const vector_t *eye,const vector_t *at,const vector_t *up)
{
	//eyeΪ��������������λ�ã�
	//atΪ�����ͷ��׼�����������������λ�ã�
	//upΪ������ϵķ��������������е�λ��  
	//mΪģ����ͼ�������Խ������е���������ת�����������꣬������������ת���������ꡣ
	//�˴��µ���opengl��û�е�����ģ�;������ͼ���� 
	vector_t xaxis;
	vector_t yaxis;
	vector_t zaxis;
	//�����ۣ��۲죩���꣬�ó�������������ᵥλ���� 
	vector_sub(&zaxis,at,eye);//�����������z�ᣨ�������巽��Ϊz�ᣬ��ȣ� 
	vector_normalize(&zaxis);
	vector_crossproduct(&xaxis,up,&zaxis);//ͨ����涨��������ϵ�������ˣ��ó�x������ 
	vector_normalize(&xaxis);
	vector_crossproduct(&yaxis,&zaxis,&xaxis);//x����z��ó����ϵ�y������ 

	m->m[0][0]=xaxis.x;
	m->m[1][0]=xaxis.y;
	m->m[2][0]=xaxis.z;
	m->m[3][0]=-vector_dotproduct(&xaxis, eye);

	m->m[0][1]=yaxis.x;
	m->m[1][1]=yaxis.y;
	m->m[2][1]=yaxis.z;
	m->m[3][1]=-vector_dotproduct(&yaxis, eye);

	m->m[0][2]=zaxis.x;
	m->m[1][2]=zaxis.y;
	m->m[2][2]=zaxis.z;
	m->m[3][2]=-vector_dotproduct(&zaxis, eye);
	
	m->m[0][3]=m->m[1][3]=m->m[2][3]=0.0f;
	m->m[3][3]=1.0f;
}

//����͸��ͶӰ��Ͷ����󣬴˴�CVV��z�����ȡֵΪ[0,1] 
void matrix_set_perspective(matrix_t *m,float fovy,float aspect,float zn,float zf) 
{
	//fovyΪ�ӽǣ�aspectΪ�߿�ȣ�znΪ��ƽ�棬zfΪԶƽ�� 
	//���ｫ��ƽ����ΪͶ��ƽ�� 
	float fax=1.0f/(float)tan(fovy*0.5f);
	matrix_set_zero(m);
	m->m[0][0]=(float)(fax/aspect);
	m->m[1][1]=(float)(fax);
	m->m[2][2]=zf/(zf-zn);
	m->m[3][2]=-zn*zf/(zf-zn);
	m->m[2][3]=1;
}