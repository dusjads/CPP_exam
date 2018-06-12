#include "set.h"
#include <iostream>
#include <cassert>

const int inf = 1e9;

template<class T>
set<T>::node::node(T new_val){
    val = new_val;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

template<class T>
set<T>::iterator::iterator(set* my_set){
    owner = my_set;
    my_set->v.push_back(this);
}

template<class T>
typename set<T>::iterator set<T>::iterator::operator=(iterator rhs){
    cur = rhs.cur;
    is_valid = rhs.is_valid;
    owner->v.push_back(this);
    return *this;
}


template<class T>
set<T>::iterator::~iterator ()
{   
    for (size_t it = 0; it < owner->v.size(); it++)
    {
        if (owner->v[it] == this)
        {   
            owner->v.erase(owner->v.begin() + it);
            it--;
        }
    }
}

// void set<T>::iterator::swap(iterator &rhs)
// {
//     std::swap(cur, rhs.cur);
//     std::swap(is_valid, rhs.is_valid);
//     std::swap(owner, rhs.owner);
// }

// Элемент на который сейчас ссылается итератор.
// Разыменование итератора end() неопределено.
// Разыменование невалидного итератора неопределено.
template<class T>
T const& set<T>::iterator::operator*() const{
    assert(is_valid && cur->val != inf);
    return cur->val;
}

// Переход к элементу со следующим по величине ключом.
// Инкремент итератора end() неопределен.
// Инкремент невалидного итератора неопределен.
template<class T>
typename set<T>::iterator& set<T>::iterator::operator++(){
    assert(is_valid && cur->val != inf);
    if (cur->val == inf)
        return *this;
    if (cur->right){
        cur = cur->right;
        while (cur->left){
            cur = cur->left;
        }
        return *this;
    }
    while (cur->parent && cur == cur->parent->right){
        cur = cur->parent;
    }
    // if (!cur->parent){
    //  is_end = true;
    //  return *this;
    // }
    assert(cur->parent);
    cur = cur->parent;
    return *this;
}

template<class T>
typename set<T>::iterator set<T>::iterator::operator++(int){
    iterator prev = *this;
    ++*this;
    return prev;
}

// Переход к элементу с предыдущим по величине ключом.
// Декремент итератора begin() неопределен.
// Декремент невалидного итератора неопределен.
template<class T>
typename set<T>::iterator& set<T>::iterator::operator--(){
    //std::cout << "in --\n";
    assert(is_valid);
    if (cur->left){
        cur = cur->left;
        while (cur->right){
            cur = cur->right;
        }
        return *this;
    }
    while (cur->parent && cur == cur->parent->left){
        cur = cur->parent;
    }
    assert(cur->parent);
    cur = cur->parent;
    return *this;
}

template<class T>
typename set<T>::iterator set<T>::iterator::operator--(int){
    iterator next = *this;
    --*this;
    return next;
}

template<class T>
set<T>::set() {
    head = new node(inf);
}

template<class T>
typename set<T>::node* set<T>::copy(node* src){
    //std::cout << "in copy " << src->val << '\n';
    node* new_node = new node(src->val);
    if (src->right){
        new_node->right = copy(src->right);
        if (new_node->right)
            new_node->right->parent = new_node;
    }
    if (src->left){
        new_node->left = copy(src->left);
        if (new_node->left)
            new_node->left->parent = new_node;
    }
    //std::cout << new_node->val << '\n';
    return new_node;
}

// Создает копию указанного set-а.
template<class T>
set<T>::set(set const& source){
    head = copy(source.head);
}

template<class T>
bool set<T>::empty(){
    // std::cout << "in empty ";
    // std::cout << head->val << '\n';
    return head->val == inf;
}

template<class T>
void set<T>::clear(){
    // std::cout << "in clear\n";
    while (!empty()){
        auto it = end();
        it--;
        // info(it);
        erase(it);
    }
}

// Изменяет this так, чтобы он содержал те же элементы, что и rhs.
// Инвалидирует все итераторы, принадлежащие set'у this, включая end().
template<class T>
set<T>& set<T>::operator=(set const& rhs){
    clear();
    delete head;
    head = copy(rhs.head);
    return *this;
}

// Деструктор. Вызывается при удалении объектов set.
// Инвалидирует все итераторы ссылающиеся на элементы этого set
// (включая итераторы ссылающиеся на элемент следующий за последним).
template<class T>
set<T>::~set(){
    // std::cout << "in ~set\n";
    clear();
    delete head;
}
// Вершина, под которой надо вставить
// end для пустого
template<class T>
typename set<T>::iterator set<T>::appr_find(T x){
    iterator new_it = iterator(this);
    new_it.cur = head;
    if (head->val == inf)
        return new_it;

    node* it_node = new_it.cur;
    while (((it_node->left && it_node->val > x) || (it_node->right && it_node->val < x))){
        while (it_node->left && it_node->val > x){
            it_node = it_node->left;
        }
        while (it_node->right && it_node->val < x){
            it_node = it_node->right;
        }
    }
    new_it.cur = it_node;
    // std::cout << "it_n " << it_node->val << '\n';
    return new_it;
}


// Поиск элемента.
// Возвращает итератор на найденный элемент, либо end(), если элемент
// с указанным значением отсутвует.
template<class T>
typename set<T>::iterator set<T>::find(T x){
    iterator new_it = appr_find(x);
    node* it_node = new_it.cur;
    if (x == inf){
        return new_it;
    }
    if (it_node->val != x)
        return end();
    return new_it;
}

// Вставка элемента.
// 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
//    на уже присутствующий элемент и false.
// 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
//    элемент и true.
template<class T>
std::pair<typename set<T>::iterator, bool> set<T>::insert(T x){
    if (head->val == inf){
        node* new_node = new node(x);
        head->parent = new_node;
        new_node->right = head;
        head = new_node;
        iterator new_it(this);
        new_it.cur = head;
        return std::make_pair(new_it, true);
    }
    iterator new_it = appr_find(x);
    node* it_node = new_it.cur;
    // std::cout << "it_node " << it_node->val << '\n';
    if (it_node->val == x){
        return std::make_pair(new_it, false);
    }
    node* new_node = new node(x);
    if (it_node->val == inf){
        new_node->parent = it_node->parent;
        it_node->parent->right = new_node;
        new_node->right = it_node;
        it_node->parent = new_node;
    }
    else if (it_node->val > x){
        it_node->left = new_node;
        new_node->parent = it_node;
    }
    else{
        it_node->right = new_node;
        new_node->parent = it_node;
    }
    new_it.cur = new_node;
    return std::make_pair(new_it, true);
}


template<class T>
void set<T>::erase_node(node* old){
    bool is_left = false;
    // std::cout << "in erase_node " << old->val << '\n';
    if (!old->left && !old->right){
        if (old->parent->left == old){
            old->parent->left = nullptr;
            return;
        }
        old->parent->right = nullptr;
        return;
    }
    if (!old->parent){
        if (!old->left){
            head = old->right;
            head->parent = nullptr;
            return;
        }
        if (!old->right){
            head = old->left;
            head->parent = nullptr;
            return;
        }
    }
    else {
        is_left = (old == old->parent->left);
        if (!old->left){
            if (is_left){
                old->right->parent = old->parent;
                old->parent->left = old->right;
            }
            else{   
                old->right->parent = old->parent;
                old->parent->right = old->right;
            }
            return;
        }
        if (!old->right){
            if (is_left){
                old->left->parent = old->parent;
                old->parent->left = old->left;
            }
            else{   
                old->left->parent = old->parent;
                old->parent->right = old->left;
            }
            return;
        }
    }
    node* max_left = old->left;
    while (max_left->right){
        max_left = max_left->right;
    }
    T key = max_left->val;
    erase_node(max_left);
    delete max_left;
    // std::cout << "out of en\n";
    node* new_node = new node(key);
    new_node->right = old->right;
    new_node->left = old->left;
    new_node->parent = old->parent;
    if (old->left)
        old->left->parent = new_node;   
    if (old->right)
        old->right->parent = new_node;
    if (old->parent){
        if (is_left)
            old->parent->left = new_node;
        else
            old->parent->right = new_node;
    }
    else {
        head = new_node;
    }
    // std::cout << "out of en2\n";
}

// Удаление элемента.
// Инвалидирует только итераторы удаленной вершины.
template<class T>
void set<T>::erase(iterator it){
    // std::cout << "in erase\n";
    // std::cout << it.is_valid << ' ' << (it.cur->val != inf) << '\n';
    assert(it.is_valid && it.cur->val != inf);
    T val = it.cur->val;
    erase_node(it.cur);
    for (auto &i : v){
        if (i->cur->val == val){
            i->is_valid = false;
        }
    }
    delete it.cur;
}

// Возващает итератор на элемент с минимальный ключом.
template<class T>
typename set<T>::iterator set<T>::begin(){
    return appr_find(-inf);
}
// Возващает итератор на элемент следующий за элементом с максимальным ключом.
template<class T>
typename set<T>::iterator set<T>::end(){
    return find(inf);
}

template<class T>
bool set<T>::iterator::equal(set<T>::iterator const right){
    return cur == right.cur;
}

template<class T>
bool set<T>::iterator::operator==(set<T>::iterator const& right){
    return this->equal(right);
}

template<class T>
bool set<T>::iterator::operator!=(set<T>::iterator const& right){
    return !this->equal(right);
}

// template<class T>
// bool operator==(set<T>::iterator left, set<T>::iterator right){
//     return left.equal(right);
// }

// template<class T>
// bool operator!=(set<T>::iterator left, set<T>::iterator right){
//     return !left.equal(right);
// }

template<class T>
void set<T>::info(set<T>::iterator a){
    node* b = a.cur;
    std::cout << "value " << b->val << '\n';
    std::cout << "parent ";
    ((b->parent) ? std::cout << (b->parent->val) : std::cout << "none") << '\n';
    std::cout << "right ";
    ((b->right) ? std::cout << b->right->val :std::cout << "none") << '\n';
    std::cout << "left "; 
    ((b->left) ? std::cout << b->left->val : std::cout <<"none") << '\n';

}

template<class T>
void print(set<T> a){
    for (auto it = a.begin(); it != a.end(); it++)
        std::cout << *it << ' ';
    std::cout << '\n';
}

int main()
{
    set<int> c;
    c.insert(5);
    c.insert(6);
    c.insert(0);
    set<int> b = c;
 //    for (auto it = b.begin(); it != b.end(); it++)
    //  std::cout << *it << ' ';
    // std::cout << '\n';
    // for (auto it = c.begin(); it != c.end(); it++)
    //  std::cout << *it << ' ';
    // std::cout << '\n';
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
    return 0;       
}