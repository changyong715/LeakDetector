#include"Alloc.h"
#include<string>

//1.new未释放
void Test1()
{
	int* a = new int(1);
}
//2.new[] 未释放
void Test2()
{
	int* a = new int[10];
}
//3.匹配使用问题
void Test3()
{
	int* a = new int[10];
	delete a;
}
//4.多次释放
void Test4()
{
	int* a = new int[10];
	delete[] a;
	delete[] a;
}
//5.释放指向同一空间的指针
void Test5()
{
	int* a = new int(1);
	int* b = a;
	delete a;
	delete b;
}
//6.释放的不是指针
void Test6() //直接报错，肉眼可看，不用测
{
	string a = "111";
	//delete a;
}

int main()
{
	Test1();
	//Test2();
	//Test3();
	//Test4();
	//Test5();
	//Test6();
	return 0;
}