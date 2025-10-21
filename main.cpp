#include <iostream>
#include "../include/RHMap.h"




int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;
    RHMap<int, std::vector<int>> map;
    map.emplace(1, std::vector<int>{2});
    map.emplace(2, std::vector<int>{3});
    map.emplace(3, std::vector<int>{4});
    map.emplace(6, std::vector<int>{45});
    map.emplace(26, std::vector<int>{68});
    map.emplace(46, std::vector<int>{78});
    map.emplace(246, std::vector<int>{7960});
    map.emplace(24, std::vector<int>{60}); 
    map.emplace(245, std::vector<int>{774});
    map.emplace(2722, std::vector<int>{479}); 
    map.emplace(908, std::vector<int>{4876}); 
    map.emplace(23, std::vector<int>{4679});
    map.emplace(2525, std::vector<int>{490});
    map.emplace(22352, std::vector<int>{324});
    map.emplace(68686, std::vector<int>{264});
    std::cout << map << std::endl;
    return 0;
}