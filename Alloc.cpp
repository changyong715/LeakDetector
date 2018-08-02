#include"Alloc.h"

//初始化静态变量
int Inspect::count = 0;

//定义异常类，解决delete的非法操作
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
	int _errId;//错误码
	const char* _errMsg;//错误消息
};

//选用双向链表管理分配的内存
typedef struct List {
	List* _next;
	List* _prev;
	int _size;  //申请的内存空间大小
	bool _IsNotArr;  //判断是operator new还是operator new[]，是数组置为false
	
	//初始化，采用全缺省,避免传参出错
	List(List* next=NULL,List* _prev=NULL,int size=0,bool isnotarr=true)
		:_next(next)
		, _prev(_prev)
		,_size(size)
		,_IsNotArr(isnotarr) //默认是operator new
	{}

}List;

//定义头节点
static List phead = {&phead,&phead,0};

//未释放的空间大小
static int SurplusMemory = 0;

//静态对象的析构调用ReleaseMemory函数，
//ReleaseMemory函数调用Delete(),判断是否内存泄漏
void Inspect::ReleaseMemory()
{
	//是否内存泄漏，判断SurplusMemory是否为0即可
	if (0 == SurplusMemory)
	{
		printf("Memory not reveal!\n");
		return;
	}
	else
	{
		int count = 0;//统计泄漏次数，即链表中有几个节点就代表有几次泄漏
		//遍历链表判断
		List* cur = phead._next;
		while (cur != NULL && cur!=&phead)
		{
			//判断是new泄漏还是new []泄漏，通过_IsNotArr判断
			if (true == cur->_IsNotArr)
				printf("new空间未释放，");
			else
				printf("new[]空间未释放，");
			printf("对应指针->0x%x，泄漏的大小是%dbyte\n", cur, cur->_size);
			cur = cur->_next;
			++count;
		}
		printf("总共泄露了%d处，总计大小是%dbyte\n", count, SurplusMemory);
	}
}

//分配内存，用链表管理，头插
void* AllocMemory(size_t size,bool isnotarr=true)
{
	//管理分配的链表结构
	size_t newsize = size + sizeof(List);//插入节点的大小，包括头结点大小

	//转为List*，方便操作管理
	List* node = (List*)malloc(newsize);

	//插入节点
	node->_prev = &phead;//链接头结点
	node->_next = phead._next;

	node->_size = size;//分配的内存大小
	node->_IsNotArr = isnotarr;//判断是否为operator new[]

	phead._next->_prev = node;
	phead._next = node;

	//更新已经分配的内存大小
	SurplusMemory += size;

	//返回new的空间
	return (char*)node - sizeof(List);  //要减去头结点大小，才是new实际申请的空间大小
}

bool Find(void* ptr)
{
	List* cur = phead._next;
	if (NULL == cur)
		return false;
	//得到正确的节点位置，即头结点大小+ptr大小
	List* ret = (List*)((char*)ptr+sizeof(List));
	while (cur != NULL && cur != &phead)
	{
		if (cur == ret)
			return true;
		cur = cur->_next;
	}
	return false;
}

//delete，即删除双向链表的一个一个节点，所以需要找到对应的节点
void Delete(void* ptr, bool isnotarr=true)
{
	//堆空间自底向上增长，减去头结点结尾对应位置
	List* del = (List*)((char*)ptr+sizeof(List));

	if (Find(ptr) == false)//避免释放空指针，即多次释放问题
		throw Exception(1,"多次释放错误");

	//注意new delete和new [] delete[] 匹配使用
	if (del->_IsNotArr != isnotarr)
	{
		/*printf("new/delete new[]/delete[]未匹配使用\n");
		return;*/
		throw Exception(2,"未匹配使用");
	}

	//删除节点，转为双向链表删除节点问题
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

	//释放对应空间大小，并且减去相应的SurplusMemory
	SurplusMemory -= del->_size;
end:
	free(del);
	del = NULL;
}

//重载operator new 
void* operator new(size_t size)
{
	//调用链表管理
	return AllocMemory(size);
}

//重载operator new[]
void* operator new[](size_t size)
{
	return AllocMemory(size, false);
}

//重载delete
void operator delete(void* ptr)
{
	//调用链表删除
	try {
		Delete(ptr);
	}
	catch (const Exception& w)
	{
		printf("错误码:%d  %s\n", w.ID(),w.What());
	}
}

//重载delete []
void operator delete[](void* ptr)
{
	try {
		Delete(ptr, false);
	}
	catch (const Exception& w)
	{
		printf("错误码:%d  %s\n",w.ID(), w.What());
	}
}