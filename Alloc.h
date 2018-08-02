#include<iostream>
#include<cstring>
#include<list>
using namespace std;

#ifndef __ALLOC__
#define __ALLOC__

//����operator new ��operator new[]
void* operator new(size_t size);
void* operator new[](size_t size);

//����operator delete��operator delete[]
void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif

//�м��̣����ж��Ƿ�й¶�Ĺ������ھ�̬���������������,�������ڽ����������ж�
class Inspect {
private:
	void ReleaseMemory();
public:
	static int count;//��̬��������ֹ��̬������������
	Inspect() {
		++count;
	}
	~Inspect() {
		if (--count == 0) {
			ReleaseMemory(); //����delete���
			getchar();	//����ۿ����н��
		}
	}
};

static Inspect inspect;