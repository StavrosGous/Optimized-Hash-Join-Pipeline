#include <iostream>
#include "../include/RHMap.h"




int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;
    RHMap<int, int> map;
    map.insert(1, 2);
    map.insert(2, 3);
    map.insert(3, 4);
    map.insert(6, 45);
    map.insert(26, 68);
    map.insert(46, 78);
    map.insert(246, 7960);
    map.insert(24, 60); 
    map.insert(245, 774);
    map.insert(2722, 479); 
    map.insert(908, 4876); 
    map.insert(23, 4679);
    map.insert(2525, 490);
    map.insert(22352, 324);
    map.insert(68686, 264);
    return 0;
}