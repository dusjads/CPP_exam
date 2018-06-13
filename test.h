#ifndef TEST
#define TEST

#include <iostream>
#include <cassert>
#include <set>
#include <string>

template<class T>
void print(set<T> a){
    for (auto it = a.begin(); it != a.end(); it++)
        std::cout << *it << ' ';
    std::cout << '\n';
}

template<class T>
void print_ok(std::set<T> a){
    for (auto it = a.begin(); it != a.end(); it++)
        std::cout << *it << ' ';
    std::cout << '\n';
}

void test_int(){
    set<int> c;
    c.insert(5);
    c.insert(6);
    c.insert(0);
    set<int> b = c;
    print(c);
    print(b);
    for (int i = 0; i < 20; i+=5)
        c.insert(i);
    c.insert(10);
    // c.info(c.find(inf));
    c.insert(8);
    // c.info(c.find(8));
    for (int i = 0; i < 20; i+=2)
        c.insert(i);
    for (int i = 0; i < 20; i+=3)
        c.insert(i);
    for(auto it = c.begin(); it != c.end(); it++)
        std::cout << *it << ' ';
    std::cout << '\n';
   
   std::cout << *c.find(5) << '\n';
   auto it = c.find(5);
   c.erase(it);

    it = c.begin();
    while (it != c.end()){
        auto tmp = it;
        tmp++;
        if (*it % 2) {
            c.erase(it);
        }
        it = tmp;
    }
    for(auto it = c.begin(); it != c.end(); it++)
        std::cout << *it << ' ';
    std::cout << '\n';
    for(auto it = --c.end(); it != c.begin(); it--)
        std::cout << *it << ' ';
    std::cout << *c.begin() << '\n';
    it = c.end();
    c.insert(19);
    for(auto it = c.begin(); it != c.end(); it++)
        std::cout << *it << ' ';
    std::cout << '\n';
    auto it2 = it;
    it2--;
    std::cout << *it2 << '\n';
    auto it3 = c.end();
    c.insert(20);
    it3--;
    std::cout << *it3 << '\n';
    print(b);
    b = c;
    print(b);
    std::cout << (c.end() == ++(--c.end())) << '\n';

    std::set <int> ok;
    set<int> my;
    int samples[8] = {34,132312,235,687,238,0,-123,-14323};
    for (int i = 0; i < 8; i++){
        my.insert(samples[i]);
        ok.insert(samples[i]);
    }
    print_ok(ok);
    print(my);

    auto ok_it = ok.begin();
    auto my_it = my.begin();
    bool check = true;
    while (my_it != my.end()){
        check &= (*my_it == *ok_it);
        my_it++;
        ok_it++;
    }
    std::cout << (check ? "OK" : "Oh, no") << '\n';
}

void test_string(){
    std::set <std::string> ok;
    set<std::string> my;
    std::string samples[8] = {"I", "Love", "C++", "But", "I'm", "So", "Weak", "So"};
    for (int i = 0; i < 8; i++){
        my.insert(samples[i]);
        ok.insert(samples[i]);
    }
    print_ok(ok);
    print(my);

    auto ok_it = ok.begin();
    auto my_it = my.begin();
    bool check = true;
    while (my_it != my.end()){
        check &= (*my_it == *ok_it);
        my_it++;
        ok_it++;
    }
    std::cout << (check ? "OK" : "Oh, no") << '\n';
    set<std::string> a1;
    std::set<std::string> a1_ok;
    auto it_a1 = a1.end();
    auto it_a1_ok = a1_ok.end();
    a1.insert("a"); 
    a1_ok.insert("a");
    it_a1--;
    it_a1_ok--;
    check = (*it_a1_ok == *it_a1);
    std::cout << (check ? "OK" : "Oh, no") << '\n';

}

template <class T>
void test(){

    bool type_int = std::is_same<T,int>::value;

    if (type_int){
        test_int();
    }
    else
    {
        test_string();
    }
        
}



#endif