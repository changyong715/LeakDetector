#include"Alloc.h"
#include<string>

//1.newδ�ͷ�
void Test1()
{
	int* a = new int(1);
}
//2.new[] δ�ͷ�
void Test2()
{
	int* a = new int[10];
}
//3.ƥ��ʹ������
void Test3()
{
	int* a = new int[10];
	delete a;
}
//4.����ͷ�
void Test4()
{
	int* a = new int[10];
	delete[] a;
	delete[] a;
}
//5.�ͷ�ָ��ͬһ�ռ��ָ��
void Test5()
{
	int* a = new int(1);
	int* b = a;
	delete a;
	delete b;
}
//6.�ͷŵĲ���ָ��
void Test6() //ֱ�ӱ������ۿɿ������ò�
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