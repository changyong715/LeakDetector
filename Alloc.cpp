#include"Alloc.h"

//��ʼ����̬����
int Inspect::count = 0;

//�����쳣�࣬���delete�ķǷ�����
class Exception {
public:
	Exception(int errid,const char* errmsg)
		:_errId(errid)
		,_errMsg(errmsg)
	{}
	const char* What() const
	{
		return _errMsg;
	}
	const int ID() const
	{
		return _errId;
	}
private:
	int _errId;//������
	const char* _errMsg;//������Ϣ
};

//ѡ��˫��������������ڴ�
typedef struct List {
	List* _next;
	List* _prev;
	int _size;  //������ڴ�ռ��С
	bool _IsNotArr;  //�ж���operator new����operator new[]����������Ϊfalse
	
	//��ʼ��������ȫȱʡ,���⴫�γ���
	List(List* next=NULL,List* _prev=NULL,int size=0,bool isnotarr=true)
		:_next(next)
		, _prev(_prev)
		,_size(size)
		,_IsNotArr(isnotarr) //Ĭ����operator new
	{}

}List;

//����ͷ�ڵ�
static List phead = {&phead,&phead,0};

//δ�ͷŵĿռ��С
static int SurplusMemory = 0;

//��̬�������������ReleaseMemory������
//ReleaseMemory��������Delete(),�ж��Ƿ��ڴ�й©
void Inspect::ReleaseMemory()
{
	//�Ƿ��ڴ�й©���ж�SurplusMemory�Ƿ�Ϊ0����
	if (0 == SurplusMemory)
	{
		printf("Memory not reveal!\n");
		return;
	}
	else
	{
		int count = 0;//ͳ��й©���������������м����ڵ�ʹ����м���й©
		//���������ж�
		List* cur = phead._next;
		while (cur != NULL && cur!=&phead)
		{
			//�ж���newй©����new []й©��ͨ��_IsNotArr�ж�
			if (true == cur->_IsNotArr)
				printf("new�ռ�δ�ͷţ�");
			else
				printf("new[]�ռ�δ�ͷţ�");
			printf("��Ӧָ��->0x%x��й©�Ĵ�С��%dbyte\n", cur, cur->_size);
			cur = cur->_next;
			++count;
		}
		printf("�ܹ�й¶��%d�����ܼƴ�С��%dbyte\n", count, SurplusMemory);
	}
}

//�����ڴ棬���������ͷ��
void* AllocMemory(size_t size,bool isnotarr=true)
{
	//������������ṹ
	size_t newsize = size + sizeof(List);//����ڵ�Ĵ�С������ͷ����С

	//תΪList*�������������
	List* node = (List*)malloc(newsize);

	//����ڵ�
	node->_prev = &phead;//����ͷ���
	node->_next = phead._next;

	node->_size = size;//������ڴ��С
	node->_IsNotArr = isnotarr;//�ж��Ƿ�Ϊoperator new[]

	phead._next->_prev = node;
	phead._next = node;

	//�����Ѿ�������ڴ��С
	SurplusMemory += size;

	//����new�Ŀռ�
	return (char*)node - sizeof(List);  //Ҫ��ȥͷ����С������newʵ������Ŀռ��С
}

bool Find(void* ptr)
{
	List* cur = phead._next;
	if (NULL == cur)
		return false;
	//�õ���ȷ�Ľڵ�λ�ã���ͷ����С+ptr��С
	List* ret = (List*)((char*)ptr+sizeof(List));
	while (cur != NULL && cur != &phead)
	{
		if (cur == ret)
			return true;
		cur = cur->_next;
	}
	return false;
}

//delete����ɾ��˫�������һ��һ���ڵ㣬������Ҫ�ҵ���Ӧ�Ľڵ�
void Delete(void* ptr, bool isnotarr=true)
{
	//�ѿռ��Ե�������������ȥͷ����β��Ӧλ��
	List* del = (List*)((char*)ptr+sizeof(List));

	if (Find(ptr) == false)//�����ͷſ�ָ�룬������ͷ�����
		throw Exception(1,"����ͷŴ���");

	//ע��new delete��new [] delete[] ƥ��ʹ��
	if (del->_IsNotArr != isnotarr)
	{
		/*printf("new/delete new[]/delete[]δƥ��ʹ��\n");
		return;*/
		throw Exception(2,"δƥ��ʹ��");
	}

	//ɾ���ڵ㣬תΪ˫������ɾ���ڵ�����
	List* next = del->_next;
	List* prev = del->_prev;
	if (NULL == next)
	{
		prev->_next = NULL;
		prev->_prev = NULL;
		goto end;
	}
	next->_prev = prev;
	prev->_next = next;

	//�ͷŶ�Ӧ�ռ��С�����Ҽ�ȥ��Ӧ��SurplusMemory
	SurplusMemory -= del->_size;
end:
	free(del);
	del = NULL;
}

//����operator new 
void* operator new(size_t size)
{
	//�����������
	return AllocMemory(size);
}

//����operator new[]
void* operator new[](size_t size)
{
	return AllocMemory(size, false);
}

//����delete
void operator delete(void* ptr)
{
	//��������ɾ��
	try {
		Delete(ptr);
	}
	catch (const Exception& w)
	{
		printf("������:%d  %s\n", w.ID(),w.What());
	}
}

//����delete []
void operator delete[](void* ptr)
{
	try {
		Delete(ptr, false);
	}
	catch (const Exception& w)
	{
		printf("������:%d  %s\n",w.ID(), w.What());
	}
}