
#include <stdio.h>

class TestBase {
public:

  TestBase(int v) {
    v_ = v;
    num_instances++;
  }

  virtual ~TestBase() {
    num_instances--;
  }

  int v() {
    return v_;
  }

  virtual void print() {
    printf("print TestBase\n");
  }

  static int get_num_instances() {
    return num_instances;
  }

private:

  int v_;
  static int num_instances;
};




class TestDerived : public TestBase {
public:

  TestDerived(int v) : TestBase(v) {
    v2_ = v * v;
  }

  virtual void print() {
    printf("print TestDerived\n");
  }

private:

  int v2_;
};

class TestDerived2 : public TestBase {
public:

  TestDerived2(int v) : TestBase(v) {
    v2_ = v * v * v;
  }

  virtual void print() {
    printf("print TestDerived2\n");
  }

private:

  int v2_;
};


