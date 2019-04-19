#include <iostream> 
#include <iterator> 
#include <map> 
  
// using namespace std; 
  
int main() 
{ 
  
    // empty map container 
    std::map<std::string,int> map; 
    std::map<std::string,int>::iterator it;
    // insert elements in random order 
    map.insert(std::pair<std::string, int>("1", 40)); 
    map.insert(std::pair<std::string, int>("2", 30)); 
    map.insert(std::pair<std::string, int>("3", 60)); 
    map.insert(std::pair<std::string, int>("4", 20)); 
    map.insert(std::pair<std::string, int>("5", 50)); 
    map.insert(std::pair<std::string, int>("6", 50)); 
    map.insert(std::pair<std::string, int>("7", 10)); 

    std::map<std::string, int>::iterator itr; 
    for (itr = map.begin(); itr != map.end(); ++itr) { 
        if((itr->second) == 50)
        {
            itr = map.erase(itr);
        }
    } 

    std::cout << "\nThe map map is : \n"; 
    std::cout << "\tKEY\tELEMENT\n"; 

    std::map<std::string, int>::iterator itr1; 
    for (itr1 = map.begin(); itr1 != map.end(); ++itr1) { 
        std::cout << '\t' << itr1->first 
             << '\t' << itr1->second << '\n'; 
    } 
    std::cout << std::endl; 
}