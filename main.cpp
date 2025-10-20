#include <iostream>
#include "../include/RHMap.h"




int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;
    RHMap<int, int> map;
    map.emplace(1, 2);
    map.emplace(2, 3);
    map.emplace(3, 4);
    map.emplace(6, 45);
    map.emplace(26, 68);
    map.emplace(46, 78);
    map.emplace(246, 7960);
    map.emplace(24, 60); 
    map.emplace(245, 774);
    map.emplace(2722, 479); 
    map.emplace(908, 4876); 
    map.emplace(23, 4679);
    map.emplace(2525, 490);
    map.emplace(22352, 324);
    map.emplace(68686, 264);
    return 0;
}