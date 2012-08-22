
#include <stdio.h>

class TestBase {
public:

  TestBase(int value = 0) {
    value_ = value;
    num_instances_++;
  }

  virtual ~TestBase() {
    num_instances_--;
  }

  int value() const {
    return value_;
  }

  void set_value(int value) {
    value_ = value;
  }

  virtual void print() const {
    printf("print TestBase\n");
  }

  static int num_instances() {
    return num_instances_;
  }

private:

  int value_;
  static int num_instances_;
};




class TestDerived : public TestBase {
public:

  TestDerived(int value) : TestBase(value) {
  }

  virtual void print() const {
    printf("print TestDerived\n");
  }
};

class TestDerived2 : public TestBase {
public:

  TestDerived2(int value) : TestBase(value) {
  }

  virtual void print() const {
    printf("print TestDerived2\n");
  }
};


