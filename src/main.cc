#include "driver.hh"

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    driver drv;
    if (drv.scan(argv[1])) return 1;
    return 0;
}
