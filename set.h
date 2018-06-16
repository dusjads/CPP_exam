#ifndef SET
#define SET

#include <cassert>
#include <iostream>
#include <vector>

template <class T>
struct set
{
    // Bidirectional iterator.
    struct iterator;
    struct node;
    struct opt;

    // Создает пустой set.
    set() {
        head = new node(inf);
        sz = 0;
    }

    // Создает копию указанного set-а.
    set(set const& source){
        head = copy(source.head);
        sz = source.size();
    }

    // Изменяет this так, чтобы он содержал те же элементы, что и rhs.
    // Инвалидирует все итераторы, принадлежащие set'у this, включая end().
    set& operator=(set const& rhs){
        clear();
        delete head;
        head = copy(rhs.head);
        sz = rhs.size();
        return *this;
    }

    // Деструктор. Вызывается при удалении объектов set.
    // Инвалидирует все итераторы ссылающиеся на элементы этого set
    // (включая итераторы ссылающиеся на элемент следующий за последним).
    ~set() {
        // std::cout << "in ~set\n";
        clear();
        delete head;
    }

    // Поиск элемента.
    // Возвращает итератор на найденный элемент, либо end(), если элемент
    // с указанным значением отсутвует.
    iterator find(T x){
        iterator new_it = appr_find(opt(0, x));
        node* it_node = new_it.cur;
        if (it_node->val.value != x)
            return end();
        return new_it;
    }

    // возвращает итератор на первый элемент не менее, чем заданное значение
    iterator lower_bound(T x) {
        // std::cout << "in lower\n";
        iterator new_it = appr_find(opt(0, x));
        while (new_it.cur->parent && new_it.cur->val.value < x){
            new_it.cur = new_it.cur->parent;
        }
        if (new_it.cur->val.value < x)
            return end();
        return new_it;
    }
    // возвращает итератор на первый элемент больше, чем определенное значение
    iterator upper_bound(T x) {
        iterator new_it = appr_find(opt(0, x));
        // std::cout << "in upper " << x << ' ' << new_it.cur->val.value << '\n';
        // if (x == 3)
        //  info(new_it);
        iterator new_it2 = new_it;
        if (new_it.cur->right && new_it.cur->val.value <= x){
            new_it.cur = new_it.cur->right;
        }
        while (new_it.cur->left){
            new_it.cur = new_it.cur->left;
            // std::cout << new_it.cur->val.value << '\n';
        }
        while (new_it2.cur->parent && new_it2.cur->val.value <= x){
            new_it2.cur = new_it2.cur->parent;
        }
        if (*new_it <= x && *new_it2 <= x)
            return end();
        if (*new_it > x && (*new_it <= *new_it2 || *new_it2 <= x))
            return new_it;
        return new_it2;
    }

    // Вставка элемента.
    // 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
    //    на уже присутствующий элемент и false.
    // 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
    //    элемент и true.
    std::pair<iterator, bool> insert(T y){
        if (head->val == inf){
            node* new_node = new node(y);
            head->parent = new_node;
            new_node->right = head;
            head = new_node;
            iterator new_it(this);
            new_it.cur = head;
            sz++;
            return std::make_pair(new_it, true);
        }
        opt x(0, y); 
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
        sz++;
        return std::make_pair(new_it, true);
    }

    // Удаление элемента.
    // Инвалидирует только итераторы удаленной вершины.
    void erase(iterator it){
        // std::cout << "in erase\n";
        // std::cout << it.is_valid << ' ' << (it.cur->val != inf) << '\n';
        assert(it.is_valid && it.cur->val != inf);
        opt val = it.cur->val;
        erase_node(it.cur);
        for (auto &i : v){
            if (i->cur->val == val){
                i->is_valid = false;
            }
        }
        sz--;
        delete it.cur;
    }

    bool empty() const{
        return head->val == inf;
    }

    void clear(){
        // std::cout << "in clear\n";
        while (!empty()){
            auto it = end();
            it--;
            // info(it);
            erase(it);
        }
    }

    // Возващает итератор на элемент с минимальный ключом.
    iterator begin(){
        return appr_find(min_inf);
    }
    // Возващает итератор на элемент следующий за элементом с максимальным ключом.
    iterator end(){
        return appr_find(inf);
    }


    size_t size() const {
        return sz;
    }
    
    struct iterator
    {

        friend set;
        // Элемент на который сейчас ссылается итератор.
        // Разыменование итератора end() неопределено.
        // Разыменование невалидного итератора неопределено.
        T operator*() const{
            assert(is_valid && cur->val != owner->inf);
            return cur->val.value;
        }
        // Переход к элементу со следующим по величине ключом.
        // Инкремент итератора end() неопределен.
        // Инкремент невалидного итератора неопределен.
        iterator& operator++(){
            assert(is_valid && cur->val != owner->inf);
            if (cur->val == owner->inf)
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
            assert(cur->parent);
            cur = cur->parent;
            return *this;
        }

        iterator operator++(int){
            iterator prev = *this;
            ++*this;
            return prev;
        }

        // Переход к элементу с предыдущим по величине ключом.
        // Декремент итератора begin() неопределен.
        // Декремент невалидного итератора неопределен.
        iterator& operator--(){
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

        iterator operator--(int){
            iterator next = *this;
            --*this;
            return next;
        }

        iterator(set* my_set){
            owner = my_set;
            my_set->v.push_back(this);
        }

        ~iterator () {   
            for (size_t it = 0; it < owner->v.size(); it++) {
                if (owner->v[it] == this) {   
                    owner->v.erase(owner->v.begin() + it);
                    it--;
                }
            }
        }

        iterator operator=(iterator const& rhs){
            cur = rhs.cur;
            is_valid = rhs.is_valid;
            owner->v.push_back(this);
            return *this;
        }
        // Сравнение. Итераторы считаются эквивалентными если они ссылаются на один и тот же элемент.
        // Сравнение с невалидным итератором не определено.
        // Сравнение итераторов двух разных контейнеров не определено.

        bool operator==(set<T>::iterator const& right){
            return this->equal(right);
        }

        bool operator!=(set<T>::iterator const& right){
            return !this->equal(right);
        }

    private:
        node* cur = nullptr;
        bool is_valid = true;
        set* owner;
        
        bool equal(set<T>::iterator const right){
            return cur == right.cur;
        }
    };

    struct opt
    {
        int type;
        T value;

        opt(){}

        opt(int tp, T val) {
            type = tp;
            value = val;
        }

        opt(int tp){
            assert(tp == 1 || tp == -1);
            type = tp;
        }

        opt& operator=(opt const& rhs){
            type = rhs.type;
            value = rhs.value;
            return *this;
        }


        bool operator<(opt const& rhs) const{
            return (type < rhs.type || (type == rhs.type && type == 0 && value < rhs.value));
        }

        bool operator>(opt const& rhs) const{
            return rhs < *this;
        }

        bool operator==(opt const& rhs) const{
            return (type == rhs.type && (type != 0 || value == rhs.value));
        }

        bool operator!=(opt const& rhs) const{
            return !(*this == rhs);
        }
    };

    struct node
    {

        opt val;
        node* left;
        node* right;
        node* parent;

        node(T new_val){
            val = opt(0, new_val);
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }
        node(opt opt_val){
            val = opt_val;
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }
    };


private:
    node* head;
    std::vector<iterator*> v;

    iterator appr_find(opt x){
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

    void erase_node(node* old){
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
        opt key = max_left->val;
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

    node* copy(node* src){
        // std::cout << "in copy " << src->val.type << ' ' << src->val.value << '\n';
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

    void info(set<T>::iterator a){
        node* b = a.cur;
        std::cout << "value " << b->val.value << '\n';
        std::cout << "parent ";
        ((b->parent) ? std::cout << (b->parent->val.value) : std::cout << "none") << '\n';
        std::cout << "right ";
        ((b->right) ? std::cout << b->right->val.value :std::cout << "none") << '\n';
        std::cout << "left "; 
        ((b->left) ? std::cout << b->left->val.value : std::cout <<"none") << '\n';

    }

    size_t sz;

    opt inf = opt(1);
    opt min_inf = opt(-1);
};






#endif