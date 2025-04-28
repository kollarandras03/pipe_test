#include <iostream> 

const int NUM_SMALL = 100000;
const int NUM_BIG = 1000000;

// Multiplies 2 potentially big numbers safely
long long prodLargeNums(const int num1, const int num2){
    long long product = (long long)num1 * num2;
    return product;
}

int main(){
    std::cout << "The product of the numbers is: ";
    std::cout << prodLargeNums(NUM_SMALL, NUM_BIG) << std::endl;
    return 0;
}
