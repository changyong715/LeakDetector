#include<iostream>
#include<cstring>
#include<list>
using namespace std;

#ifndef __ALLOC__
#define __ALLOC__

//重载operator new 和operator new[]
void* operator new(size_t size);
void* operator new[](size_t size);

//重载operator delete和operator delete[]
void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif

//中间商，将判断是否泄露的工作放在静态对象的析构函数中,声明周期结束后会进行判断
class Inspect {
private:
	void ReleaseMemory();
public:
	static int count;//静态变量，防止静态对象析构两次
	Inspect() {
		++count;
	}
	~Inspect() {
		if (--count == 0) {
			ReleaseMemory(); //处理delete结果
			getchar();	//方便观看运行结果
		}
	}
};

static Inspect inspect;