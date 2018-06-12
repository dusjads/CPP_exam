#ifndef SET
#define SET

#include <vector>

template <class T>
struct set
{
    // Вы можете определить этот тайпдеф по вашему усмотрению.
    //typedef int T;

    // Bidirectional iterator.
    struct iterator;
    struct node;

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

    iterator appr_find(T);
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
        iterator operator=(iterator);

        // bool operator==(iterator const&) const;
        // bool operator!=(iterator const&) const;

        bool operator==(iterator const&);

        bool operator!=(iterator const&);// noexcept

    private:
        node* cur = nullptr;
        bool is_valid = true;
        set* owner;
    };

    struct node
    {

        T val;
        node* left;
        node* right;
        node* parent;
        node(T new_val);
    };

private:
    node* head;
    std::vector<iterator*> v;
};





// Сравнение. Итераторы считаются эквивалентными если они ссылаются на один и тот же элемент.
// Сравнение с невалидным итератором не определено.
// Сравнение итераторов двух разных контейнеров не определено.
// template <class T>
// bool operator==(set<T>::iterator, set<T>::iterator);
// template <class T>
// bool operator!=(set<T>::iterator, set<T>::iterator);

#endif