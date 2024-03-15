#ifndef __MAIN__
#define __MAIN__
#include <systemc.h>

SC_MODULE(Hello) {
  SC_CTOR(Hello) {
    SC_THREAD(run);
  }

  void run() {
    std::cout << "Hello, SystemC!" << std::endl;
    sc_stop();
  }
};

#endif