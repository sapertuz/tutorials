#include "main.hpp"

int sc_main(int argc, char* argv[]) {
  sc_clock clk("clk", 10, SC_NS);
  Hello hello("hello");
  sc_start();
  return 0;
}