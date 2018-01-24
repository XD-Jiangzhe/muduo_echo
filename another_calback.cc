
/*
	此demo主要是拿来练手的，练一练写回调函数,X中包含了Y的弱指针，Y不知道X的存在，在Y删除的时候回调删除在X中的Y的指针
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

class X;
class Y;
typedef function<void()> callback ;
typedef function<void(shared_ptr<Y>)> call_del;


class Y: public enable_shared_from_this<Y>
{

	public:
	
		void setcallback(callback cy){cally = cy;};
		void call(){cally();}

		void addy(callback cb);

		void set_call_del(call_del t){calldel =t; }

		void dele_x()
		{	
			calldel(shared_from_this());
			shared_from_this().reset();
		}
	
	private:

		call_del calldel;;
		callback cally;
};

class X : public enable_shared_from_this<X>
{

	public:

		void setcallback(callback bc);
		
		void find_and_run(Y*);
		void find_and_delete(shared_ptr<Y>);

		vector<weak_ptr<Y>>::size_type getlength(){return pypool.size();}
		void add_y(shared_ptr<Y>);


		vector<weak_ptr<Y>> pypool;
};



void X::add_y(shared_ptr<Y> y)
{
	pypool.push_back(y);
}


void X::find_and_delete(shared_ptr<Y> y)
{
	auto  p = find_if(pypool.begin(), pypool.end(), [&](weak_ptr<Y> t){return &*(t.lock())==&*y;});

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


	x->add_y(y);
	cout<<"before delete "<<x->getlength()<<endl;

	y->set_call_del(bind(&X::find_and_delete, &*x, placeholders::_1));
	y->dele_x();

	cout<<"after delete "<<x->getlength()<<endl;				//0
	cout<<x.use_count()<<endl;
	cout<<y.use_count()<<endl;

	return 0;
}