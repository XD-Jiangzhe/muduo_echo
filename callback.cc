/*
	此demo主要是拿来练手的，练一练写回调函数
	还有切记
	reset的时候一定要用shared_ptr，不能用裸指针，如果用裸指针，引用是不会增加的
*/

#include <memory>
#include <iostream>
#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>

using namespace std;

typedef function<void()> callback ;

void print()
{
	cout<<"print"<<endl;
}
class X;
class Y: public enable_shared_from_this<Y>
{

	public:
	
		void belong(X &);
		void setcallback(callback cy){cally = cy;};
		void call(){cally();}

		void addy(callback cb);
		void delete_from_x(callback c){c();};

	private :

		callback cally;
		shared_ptr<X> x;
};

class X : public enable_shared_from_this<X>
{

	public:

		void setcallback(callback bc);
		void addpy(Y &);
		
		void find_and_run(Y*);
		void find_and_delete(Y*);

		vector<weak_ptr<Y>>::size_type getlength(){return pypool.size();}
	private:


		vector<weak_ptr<Y>> pypool;
};

void Y::belong(X &be)
{
	assert(x==NULL);
	x=be.shared_from_this();				//这里一定要注意，不能使用裸指针，如果使用裸指针是再创建了一个智能指针，而不是在原本的智能指针的计数上+1

}
void Y::addy(callback cb)
{
	this->setcallback(cb);
	x->addpy(*this);
}

void X::addpy(Y &y)
{
	pypool.push_back(y.shared_from_this());

}

void X::find_and_run(Y *y)
{
	auto  p = find_if(pypool.begin(), pypool.end(), [&](weak_ptr<Y> t){return &*(t.lock())==y;});

	assert(p != pypool.end());
	auto temp = p->lock();
	if(temp)
		temp->call();
}

void X::find_and_delete(Y *y)
{
	auto  p = find_if(pypool.begin(), pypool.end(), [&](weak_ptr<Y> t){return &*(t.lock())==y;});

	assert(p != pypool.end());
	auto temp = p->lock();
	if(temp)
	{
		pypool.erase(p);
	}
}

int main()
{
	shared_ptr<Y> y(new Y);
	shared_ptr<X> x(new X);

	y->belong(*x);
	y->addy(print);

	x->find_and_run(&*y);

	y->delete_from_x(bind(&X::find_and_delete,&*x, &*y));
	y.reset();

	cout<<x->getlength()<<endl;				//0
	cout<<x.use_count()<<endl;
	cout<<y.use_count()<<endl;

	return 0;
}