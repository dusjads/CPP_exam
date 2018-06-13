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
    set();

    // Создает копию указанного set-а.
    set(set const&);

    // Изменяет this так, чтобы он содержал те же элементы, что и rhs.
    // Инвалидирует все итераторы, принадлежащие set'у this, включая end().
    set& operator=(set const& rhs);

    // Деструктор. Вызывается при удалении объектов set.
    // Инвалидирует все итераторы ссылающиеся на элементы этого set
    // (включая итераторы ссылающиеся на элемент следующий за последним).
    ~set();

    // Поиск элемента.
    // Возвращает итератор на найденный элемент, либо end(), если элемент
    // с указанным значением отсутвует.
    iterator find(T);

    // Вставка элемента.
    // 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
    //    на уже присутствующий элемент и false.
    // 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
    //    элемент и true.
    std::pair<iterator, bool> insert(T);

    // Удаление элемента.
    // Инвалидирует только итераторы удаленной вершины.
    void erase(iterator);

    // Возващает итератор на элемент с минимальный ключом.
    iterator begin();
    // Возващает итератор на элемент следующий за элементом с максимальным ключом.
    iterator end();

    iterator appr_find(opt);
    void erase_node(node*);

    node* copy(node* src);
    bool empty();
    void clear();
    void info(iterator);
    
    struct iterator
    {

        friend set;
        // Элемент на который сейчас ссылается итератор.
        // Разыменование итератора end() неопределено.
        // Разыменование невалидного итератора неопределено.
        T const& operator*() const;

        // Переход к элементу со следующим по величине ключом.
        // Инкремент итератора end() неопределен.
        // Инкремент невалидного итератора неопределен.
        iterator& operator++();
        iterator operator++(int);

        // Переход к элементу с предыдущим по величине ключом.
        // Декремент итератора begin() неопределен.
        // Декремент невалидного итератора неопределен.
        iterator& operator--();
        iterator operator--(int);
        bool equal(iterator const);

        iterator(set*);
        ~iterator();
        iterator operator=(iterator const&);

        // Сравнение. Итераторы считаются эквивалентными если они ссылаются на один и тот же элемент.
        // Сравнение с невалидным итератором не определено.
        // Сравнение итераторов двух разных контейнеров не определено.

        bool operator==(iterator const&);

        bool operator!=(iterator const&);

    private:
        node* cur = nullptr;
        bool is_valid = true;
        set* owner;
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
        node(T);
        node(opt);

        
    };

    opt inf = opt(1);
    opt min_inf = opt(-1);

private:
    node* head;
    std::vector<iterator*> v;
};






#endif