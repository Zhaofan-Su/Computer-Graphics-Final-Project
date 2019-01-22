 #ifndef RENDER_H
 #define RENDER_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include <assert.h>
 #include <windows.h>
 #include <tchar.h>

 //״̬����
 const int RENDER_STATE_WIREFRAME=1;//��Ⱦ�߿�
 const int RENDER_STATE_TEXTURE=2;//��Ⱦ���� 
 const int RENDER_STATE_COLOR=4; //��Ⱦ��ɫ
 const int RENDER_STATE_LIGHT_GLOBAL_AMBIENT=8;//ȫ�ֻ�����
 const int RENDER_STATE_LIGHT_DIFFUSE_REFLECTION=16;//�������


 typedef unsigned int IUINT32;
 const float PI=3.1415926f;

 typedef struct
 { 
 	float m[4][4]; 
 } matrix_t;  
 typedef struct
 {
 	float x;
 	float y;
 	float z;
 	float w;
 } vector_t;
 typedef vector_t point_t;
 
 //=====================================================================
 // �������� 
 //=====================================================================
 //���x�����˱߽磬��ѡ�߽磻���xû�г����߽磬��ѡx�����ж���ɫֵ��ʱ���õ�
 int CMID(int x,int min,int max);
 //z=x+y
 void vector_add(vector_t *z,const vector_t *x,const vector_t *y);
 //z=x-y
 void vector_sub(vector_t *z,const vector_t *x,const vector_t *y);
 //������
 float vector_dotproduct(const vector_t *x,const vector_t *y);
 //������ 
 void vector_crossproduct(vector_t *z,const vector_t *x,const vector_t *y);
 //�����ֵ��t��[0,1] 
 float interp(float x1,float y1,float t);
 //ʸ����ֵ��t��[0,1]
 void vector_interp(vector_t *z,const vector_t *x,const vector_t *y,float t); 
 //ʸ����һ�� 
 void vector_normalize(vector_t *v); 
 
 //=====================================================================
 // �������� 
 //=====================================================================
 //c=a+b
 void matrix_add(matrix_t *c,const matrix_t *a,const matrix_t *b);
 //c=a-b
 void matrix_sub(matrix_t *c,const matrix_t *a,const matrix_t *b);
 //c=a*b
 void matrix_mul(matrix_t *c,const matrix_t *a,const matrix_t *b);
 //c=a*i
 void matrix_scale(matrix_t *c,const matrix_t *a,float i);
 //y=x*m;�����;������
 void matrix_apply(vector_t *y,const vector_t *x,const matrix_t *m); 
 //����Ϊ��λ����
 void matrix_set_identity(matrix_t *m); 
 //����Ϊ����� 
 void matrix_set_zero(matrix_t *m);
 //ƽ�Ʊ任��ƽ�ƾ��� 
 void matrix_set_translate(matrix_t *m,float x,float y,float z); 
 //���ű任�����ž���
 void matrix_set_scale(matrix_t *m,float x,float y,float z); 
 //��ת�任����ת����
 void matrix_set_rotate(matrix_t *m,float x,float y,float z,float theta); 
 //����ͷ����
 void matrix_set_lookat(matrix_t *m,const vector_t *eye,const vector_t *at,const vector_t *up);
 //͸��ͶӰ��������
 void matrix_set_perspective(matrix_t *m,float fovy,float aspect,float zn,float zf);
 
 //=====================================================================
 // ����任 
 //=====================================================================
 typedef struct
 {
 	matrix_t world;//��������ϵ�任 
 	matrix_t view;//���������任 
 	matrix_t projection;//ͶӰ�任 
 	matrix_t transform;//transfrom=world*view*projection 
 	float w;//�� 
 	float h;//�� 
 } transform_t; 
 //������£����㣺transfrom=world*view*projection 
 void transform_update(transform_t *ts);
 //��ʼ����������Ļ��ߺͱ任���� 
 void transform_init(transform_t *ts, int width, int height); 
 //���������������任
 void transform_apply(const transform_t *ts, vector_t *y, const vector_t *x); 
 //����������ͬCVV�ı߽�������׶��Ĳü�
 int transform_check_cvv(const vector_t *v); 
 //��һ�����õ���Ļ���� 
 void transform_homogenize(const transform_t *ts, vector_t *y, const vector_t *x);
 
 //=====================================================================
 // ���μ��㣺���㡢ɨ���ߡ���Ե�����Ρ��������� 
 //=====================================================================
 typedef struct
 {
 	float r;
 	float g;
 	float b;
 } color_t;//RGB��ɫ 
 typedef struct
 {
 	float u;
 	float v;
 } texcoord_t;//��������
 typedef struct
 {
 	point_t pos;
 	texcoord_t tc;
 	color_t color;
 	float rhw;//rhw=1/w����������Ȳ����õģ�����projection�˷���w��z�����Թ�ϵ��������z����Ȳ��ԣ�������w���������棬��Ϊ��������̫���������ȱ����1/w  
 } vertex_t;//���� 
 typedef struct
 {
 	vertex_t v;//����Y=y��left��right�Ľ��� 
 	vertex_t v1;
 	vertex_t v2;
 } edge_t;//�� 
 typedef struct
 {
 	float top;//����ˮƽɨ����ʶ��ɨ��Ĵ�ֱ��Χ 
 	float bottom;
 	edge_t left;//��ߵı� 
 	edge_t right;//�ұߵı� 
 } trapezoid_t;//���� 
 typedef struct
 {
 	vertex_t v;//v�д�ŵ���ɨ���ߵĸ����ͣ�Ҳ��ԭ��left��Y=y�Ľ��㣬��ŵ�ǰɨ���ߵĵ� 
 	vertex_t step;//�洢���� 
 	int x;//x,y�д�ŵ���ɨ�����������ͣ���Ϊ���������͵� 
 	int y;
 	int w;//ɨ���ߵĿ� 
 } scanline_t;//ɨ����

 //��pos�����⣬ȫ����rhw����ɫ�������䣬���㲻����ͬһ��ȣ�����Ҫ��z�����ϵĽ���Ҳ���ǽ�ȥ������Ҫͬ��w
 void vertex_rhw_init(vertex_t *v); 
 //�����ֵ
 void vertex_interp(vertex_t *y,const vertex_t *x1,const vertex_t *x2,float t);
 //�����������õ� 
 void vertex_division(vertex_t *y,const vertex_t *x1,const vertex_t *x2,float w);
 //����ӷ� 
 void vertex_add(vertex_t *y,const vertex_t *x); 
 //����Y������������������������� y �Ķ��� 
 void trapezoid_edge_interp(trapezoid_t *trap,float y); 
 //�����������ߵĶ˵㣬��ʼ������ɨ���ߵ����Ͳ���
 void trapezoid_init_scan_line(const trapezoid_t *trap,scanline_t *scanline,int y); 
 //��������������0~2�����Σ����ҷ��غϷ����ε��������ҵ�ע�ͣ��������ηָ�ɿ���ɨ���ˮƽ�����Σ�
 int trapezoid_init_triangle(trapezoid_t *trap,const vertex_t *p1,const vertex_t *p2,const vertex_t *p3);
 
 //=====================================================================
 // ��Ⱦ�豸
 //=====================================================================
 typedef struct
 {
 	transform_t transform;//����任�� 
 	int width;//���ڿ�� 
 	int height;//���ڸ߶�  
 	IUINT32 **framebuffer;//���ػ��棺framebuffer[y]�����y�У��洢������ɫ����ɫ��32λ�ģ����� 
	float **zbuffer;//��Ȼ���
	IUINT32 **texture;//���� 
	int tex_width;//������ 
	int tex_height;//����߶� 
	float max_u;//��������ȣ�tex_width - 1 
	float max_v;//�������߶ȣ��������꣺v*max_v������v��[0,1] 
	int render_state;//��Ⱦ״̬ 
	IUINT32 background;//������ɫ 
	IUINT32 foreground;//�߿���ɫ 
//	light_t light;//����
 } device_t;

 //�豸��ʼ����fbΪ�ⲿ֡���棬��NULL�������ⲿ����֡���棨ÿ�ж���4�ֽڣ� 
 void device_init(device_t *device,int width,int height,void *fb);
 //ɾ���豸
 void device_destroy(device_t *device);
 //���õ�ǰ���� 
 void device_set_texture(device_t *device,void *bits,long pitch,int w,int h); 
 //���framebuffer��zbuffer
 void device_clear(device_t *device,int mode);
 //���� 
 void device_pixel(device_t *device,int x,int y,IUINT32 color);
 //���߶� 
 void device_draw_line(device_t *device,int x1,int y1,int x2,int y2,IUINT32 c);
 //���������ȡ���� 
 IUINT32 device_texture_read(const device_t *device,float u,float v);
 
 //=====================================================================
 // ��Ⱦʵ�� 
 //=====================================================================
 //����ɨ����(ɨ��������㷨) 
 void device_draw_scanline(device_t *device,scanline_t *scanline);
 //����Ⱦ���� 
 void device_render_trap(device_t *device,trapezoid_t *trap);
 //��ԭʼ������ 
 void device_draw_primitive(device_t *device,const vertex_t *v1,const vertex_t *v2,const vertex_t *v3);

 //=====================================================================
 // ����
 //=====================================================================
 typedef struct  
 {
	 color_t light;//������ɫ
	 point_t pos;//����λ��
 } light_t;
 //ȫ�ֻ�����
 void global_ambient_light(device_t *device,light_t *light);
 //���ù�����ɫ
 void set_light_color(light_t *light,float r,float g,float b);
 //���ù���λ��
 void set_light_pos(light_t *light,int x,int y,int z);
 //����
 void open_light(device_t *device,light_t *light);
 //����ͼƬ
 void loadbmp(const char *szfilename,IUINT32 t[256][256]);

 //=====================================================================
 // Win32 ���ڼ�ͼ�λ���:Ϊdevice�ṩһ��DibSection��FB
 //=====================================================================
 extern int screen_w;
 extern int screen_h;
 extern int screen_exit;
 extern int screen_mx;
 extern int screen_my;
 extern int screen_mb;
 extern int screen_keys[512];//��ǰ���̰���״̬
 extern unsigned char *screen_fb;//frame buffer
 extern long screen_pitch;
 //��Ļ��ʼ��
 int screen_init(int w,int h,const TCHAR *title);	
 //�ر���Ļ
 int screen_close();								
 //������Ϣ
 void screen_dispatch();							
 //��ʾFrameBuffer
 void screen_update();							
 // win32 event handler
 static LRESULT screen_events(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);	
 #ifdef _MSC_VER
	#pragma comment(lib, "gdi32.lib")
	#pragma comment(lib, "user32.lib")
 #endif
 
 #endif