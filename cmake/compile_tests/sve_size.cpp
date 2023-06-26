#include <arm_sve.h>

#include <iostream>

int main() { std::cout << svlen_u8(svdup_u8(0)) * 8; }
