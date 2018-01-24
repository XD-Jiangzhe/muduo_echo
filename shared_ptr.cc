#include <memory>
#include <cassert>
#include <iostream>


using namespace std;

class Y:public enable_shared_from_this<Y>
{
	public :
		shared_ptr<Y> f()
		{
			return shared_from_this();
		}
		Y* f2()
		{
			return this;
		}
};

int main()
{
	shared_ptr<Y> p(new Y);
	shared_ptr<Y> q = p->f();

	Y* r = p->f2();
	assert( p == q);
	assert(p.get() == r);


	cout<<p.use_count()<<endl;
	shared_ptr<Y> s(r);					//此处又构造了一个shared_ptr对象，独立的，只有=号才可以，这里是用裸指针创建的
	cout<<s.use_count()<<endl;			//1 
	assert(p == s);						//虽然assert断言是相同的，但是引用计数不会加1，只有拷贝才会加1

	shared_ptr<Y> m(p->f());					//这里是用的返回的shared_ptr构造的shared_ptr，所以引用会加	1
	cout<<m.use_count()<<endl;					//这里会是3

	return 0;
}