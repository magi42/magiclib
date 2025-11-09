

class MyClass : public Object {
  public:
	MyClass () {cout << "MyClass()\n";}
	~MyClass () {cout << "~MyClass()\n";}
};

void testRef () {
	Ref<MyClass> ref = new MyClass();
	cout << "Refs=" << ref.object().refCount() << "\n";
	Ref<MyClass> ref2 = ref;
	cout << "Refs=" << ref2.object().refCount() << "\n";
	Ref<MyClass> ref3 = ref2;
	cout << "Refs=" << ref2.object().refCount() << "\n";
	Ref<MyClass> ref4 = ref3;
	cout << "Refs=" << ref2.object().refCount() << "\n";
}
