#ifndef SET_H
#define SET_H

#include <cassert>
#include <iostream>
#include <list>
#include <memory>

using std::list;
using std::shared_ptr;
using std::pair;
using std::make_pair;

template <class T>
struct set
{

    // Bidirectional iterator.
    struct iterator;
    struct node;
    
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;
    using const_iterator = iterator;

    // Создает пустой set.
    set() {
        head = nullptr;
    }

    // Создает копию указанного set-а.
    set(set const& source) : set(){
        if (!source.head) return;
        head = std::shared_ptr<node>(new node(source.head->val));
        head->ptr = content.insert(content.begin(), head);
        copy(head, source.head, source);
    }

    template <typename T1>
    friend void swap(set<T1> &l, set<T1> &r);

    // Изменяет this так, чтобы он содержал те же элементы, что и rhs.
    // Инвалидирует все итераторы, принадлежащие set'у this, включая end().
    set& operator=(set<T> const& rhs){
        set<T> tmp(rhs);
        swap(tmp, *this);
        return *this;
    }

    // Деструктор. Вызывается при удалении объектов set.
    // Инвалидирует все итераторы ссылающиеся на элементы этого set
    // (включая итераторы ссылающиеся на элемент следующий за последним).
    ~set() = default;

    // Поиск элемента.
    // Возвращает итератор на найденный элемент, либо end(), если элемент
    // с указанным значением отсутвует.
    iterator find(T const& x){
        if (!head) return end();
        shared_ptr<node> new_it = appr_find(head, x);
        if (new_it->val != x) return end();
        return iterator(new_it->ptr, this);
    }

    // возвращает итератор на первый элемент не менее, чем заданное значение
    iterator lower_bound(T const& x) {
        if (!head) return end();
        shared_ptr<node> found_node = appr_find(head, x);
        if (found_node->val < x){
            auto next_ptr = found_node->ptr;
            return iterator(++next_ptr, this);
        }
        return iterator(found_node->ptr, this);
    }
    // возвращает итератор на первый элемент больше, чем определенное значение
    iterator upper_bound(T const& x) {
        if (!head) return end();
        shared_ptr<node> found_node = appr_find(head, x);
        if (x < found_node->val) return iterator(found_node->ptr, this);
        auto next_ptr = found_node->ptr;
        return iterator(++next_ptr, this);
    }

    // Вставка элемента.
    // 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
    //    на уже присутствующий элемент и false.
    // 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
    //    элемент и true.
    pair<iterator, bool> insert(T const& x){
        if (head){
            auto found_node = appr_find(head, x);
            if (found_node->val > x){
                found_node->left = shared_ptr<node>(new node(x));
                found_node->left->ptr = content.insert(found_node->ptr, found_node->left);
                return make_pair(iterator(found_node->left->ptr, this), true); 
            }
            if (found_node->val < x){
                found_node->right = shared_ptr<node>(new node(x));
                auto next_ptr = found_node->ptr;
                found_node->right->ptr = content.insert(++next_ptr, found_node->right);
                return make_pair(iterator(found_node->right->ptr, this), true);
            }
            return make_pair(iterator(found_node->ptr, this), false);
        }
        head = shared_ptr<node>(new node(x));
        head->ptr = content.insert(content.begin(), head);
        return make_pair(iterator(head->ptr, this), true);
    }

    // Удаление элемента.
    // Инвалидирует только итераторы удаленной вершины.
    // возвращает итератор на следующий постле удаленного.
    iterator erase(iterator it){
        if (!head) return end();
        shared_ptr<node> vertex = (*it.it);
        T x = vertex->val;
        shared_ptr<node> parent_vertex = parent(head, x);
        bool left = (parent_vertex) && (parent_vertex->left) && (parent_vertex->left->val == x);
        auto next = it;
        next++; 
        if (!vertex->right && !vertex->left){
            content.erase(vertex->ptr);
            if (!parent_vertex){
                head = nullptr;
                return end();
            }
            if (left)
                parent_vertex->left = nullptr;
            else
                parent_vertex->right = nullptr;
            return next;
        }
        if (vertex->left && vertex->right){
            auto prev = it;
            prev--;
            shared_ptr<node> node_prev = *(prev.it);
            shared_ptr<node> parent_prev = parent(head, node_prev->val);
            if (parent_prev->val != vertex->val) {
                parent_prev->right = nullptr;
                node_prev->left = vertex->left;
            }
            node_prev->right = vertex->right;
            if (parent_vertex) {
                if (left) parent_vertex->left = node_prev;
                else parent_vertex->right = node_prev;
            }
            else head = node_prev;
            content.erase(vertex->ptr);
            return next;
        }
        if (vertex->right){
            if (!parent_vertex) head = vertex->right;
            else if (left) parent_vertex->left = vertex->right;
            else parent_vertex->right = vertex->right;
        }
        else{
            if (!parent_vertex) head = vertex->left;
            else if (left) parent_vertex->left = vertex->left;
            else parent_vertex->right = vertex->left;
        }
        content.erase(vertex->ptr);
        return next;
    }

    bool empty() const{
        return content.size() == 0;
    }

    void clear(){
        content.clear();
        head = nullptr;
    }

    // Возващает итератор на элемент с минимальный ключом.
    iterator begin() const{
        return iterator(content.begin(), this);
    }
    // Возващает итератор на элемент следующий за элементом с максимальным ключом.
    iterator end() const{
        return iterator(content.end(), this);
    }

    // Возващает развернутый итератор на элемент с максимальным ключом.
    reverse_iterator rbegin() const{
        return std::make_reverse_iterator(end());  
    }
    // Возващает развернутый итератор на элемент следующий до элемента с минимальным ключом.
    reverse_iterator rend() const{
        return std::make_reverse_iterator(begin());  
    }

    struct iterator {
        typedef ptrdiff_t difference_type;
        typedef iterator self_type;
        typedef T const value_type;
        typedef T const& reference;
        typedef T const* pointer;
        typedef std::bidirectional_iterator_tag iterator_category;

        friend set;
        // Элемент на который сейчас ссылается итератор.
        // Разыменование итератора end() неопределено.
        // Разыменование невалидного итератора неопределено.
        T const& operator*() const{
            return (*it)->val;
        }

        T const* operator->() const{
            return &(*it)->val;
        }
        // Переход к элементу со следующим по величине ключом.
        // Инкремент итератора end() неопределен.
        // Инкремент невалидного итератора неопределен.
        iterator& operator++(){
            it++;
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
            it--;
            return *this;
        }

        iterator operator--(int){
            iterator next = *this;
            --*this;
            return next;
        }

        iterator(){
            owner = nullptr;
        }

        iterator(typename list<shared_ptr<node> >::const_iterator rhs, set<T> const* owner_){
            it = rhs;
            owner = owner_;
        };

        iterator(iterator const& rhs){
            it = rhs.it;
            owner = rhs.owner;
        };

        iterator operator=(iterator const& rhs){
            it = rhs.it;
            owner = rhs.owner;
            return *this;
        }
        // Сравнение. Итераторы считаются эквивалентными если они ссылаются на один и тот же элемент.
        // Сравнение с невалидным итератором не определено.
        // Сравнение итераторов двух разных контейнеров не определено.

        bool operator==(iterator const& rhs) const{
            return it == rhs.it;
        }

        bool operator!=(set<T>::const_iterator const& rhs) const{
            return it != rhs.it;
        }

        bool operator!=(typename list <std::shared_ptr <node > >::const_iterator rhs) const {
            return it != rhs;
        }

    private:
        typename list<shared_ptr<set<T>::node> >::const_iterator it;
        set<T> const* owner;
    };


    struct node
    {
        T val;
        shared_ptr<node> left;
        shared_ptr<node> right;
        typename list<shared_ptr<node> >::iterator ptr;

        node(T const& new_val) : val(new_val), left(nullptr), right(nullptr){}
    };


private:
    shared_ptr<node> head;
    list<shared_ptr<node> > content;

    void copy(shared_ptr<node> head, shared_ptr<node> const& src_head, set<T> const& src){
        if (src_head->left){
            head->left = shared_ptr<node> (new node(src_head->left->val));
            head->left->ptr = content.insert(head->ptr, head->left);
            copy(head->left, src_head->left, src);
        }
        if (src_head->right){
            head->right = shared_ptr<node> (new node(src_head->right->val));
            auto next_ptr = head->ptr;
            head->right->ptr = content.insert(++next_ptr, src_head->right);
            copy(head->right, src_head->right, src);
        }
    }

    shared_ptr<node> appr_find(shared_ptr<node> vertex, T const& x) const{
        if (vertex->val > x){
            if (vertex->left) return appr_find(vertex->left, x);
            return vertex;
        }
        if (vertex->val < x){
            if (vertex->right) return appr_find(vertex->right, x);
            return vertex;
        }
        return vertex;
    }

    shared_ptr<node> parent(shared_ptr<node> vertex, T const& x) const{
        if (vertex->val == x) return nullptr;
        if ((vertex->left && vertex->left->val == x) || (vertex->right && vertex->right->val == x)) 
            return vertex;
        if (vertex->val > x)
            return parent(vertex->left, x);
        return parent(vertex->right, x);
    }
};


template <class T>
void swap(set<T> &l, set<T> &r){
    swap(l.content, r.content);
    swap(l.head, r.head); 
}

#endif // SET_H
