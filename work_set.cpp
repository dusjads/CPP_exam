#include "set.h"
#include <iostream>
#include <cassert>

const set::value_type inf = 1e9;

set::node::node(set::value_type new_val){
	val = new_val;
	left = nullptr;
	right = nullptr;
	parent = nullptr;
}

set::iterator::iterator(set* my_set){
	owner = my_set;
	my_set->v.push_back(this);
}

set::iterator set::iterator::operator=(iterator rhs){
	cur = rhs.cur;
	is_valid = rhs.is_valid;
	owner->v.push_back(this);
	return *this;
}


set::iterator::~iterator ()
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

// void set::iterator::swap(iterator &rhs)
// {
//     std::swap(cur, rhs.cur);
//     std::swap(is_valid, rhs.is_valid);
//     std::swap(owner, rhs.owner);
// }

// Элемент на который сейчас ссылается итератор.
// Разыменование итератора end() неопределено.
// Разыменование невалидного итератора неопределено.
set::value_type const& set::iterator::operator*() const{
	assert(is_valid);
	return cur->val;
}

// Переход к элементу со следующим по величине ключом.
// Инкремент итератора end() неопределен.
// Инкремент невалидного итератора неопределен.
set::iterator& set::iterator::operator++(){
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
	// 	is_end = true;
	// 	return *this;
	// }
	assert(cur->parent);
	cur = cur->parent;
	return *this;
}

set::iterator set::iterator::operator++(int){
	iterator prev = *this;
	++*this;
	return prev;
}

// Переход к элементу с предыдущим по величине ключом.
// Декремент итератора begin() неопределен.
// Декремент невалидного итератора неопределен.
set::iterator& set::iterator::operator--(){
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

set::iterator set::iterator::operator--(int){
	iterator next = *this;
	--*this;
	return next;
}

set::set() {
    head = new node(inf);
}

set::node* set::copy(node* src){
	node* new_node = new node(src->val);
	if (src->right){
		new_node->right = copy(src->right);
		new_node->right->parent = new_node;
	}
	if (src->left){
		new_node = copy(src->left);
		new_node->left->parent = new_node;
	}
	return new_node;
}

// Создает копию указанного set-а.
set::set(set const& source){
	head = copy(source.head);
}

bool set::empty(){
	// std::cout << "in empty ";
	// std::cout << head->val << '\n';
	return head->val == inf;
}

void set::clear(){
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
set& set::operator=(set const& rhs){
	clear();
	head = copy(rhs.head);
	return *this;
}

// Деструктор. Вызывается при удалении объектов set.
// Инвалидирует все итераторы ссылающиеся на элементы этого set
// (включая итераторы ссылающиеся на элемент следующий за последним).
set::~set(){
	// std::cout << "in ~set\n";
	clear();
	delete head;
}
// Вершина, под которой надо вставить
// end для пустого
set::iterator set::appr_find(set::value_type x){
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
set::iterator set::find(set::value_type x){
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
std::pair<set::iterator, bool> set::insert(set::value_type x){
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


void set::erase_node(node* old){
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
	set::value_type key = max_left->val;
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
void set::erase(iterator it){
	// std::cout << "in erase\n";
	// std::cout << it.is_valid << ' ' << (it.cur->val != inf) << '\n';
	assert(it.is_valid && it.cur->val != inf);
	set::value_type val = it.cur->val;
	erase_node(it.cur);
	for (auto &i : v){
		if (i->cur->val == val){
			i->is_valid = false;
		}
	}
	delete it.cur;
}

// Возващает итератор на элемент с минимальный ключом.
set::iterator set::begin(){
	return appr_find(-inf);
}
// Возващает итератор на элемент следующий за элементом с максимальным ключом.
set::iterator set::end(){
	return find(inf);
}

bool set::iterator::equal(set::iterator const right){
	// if (is_end || right.is_end)
	// 	return is_end && right.is_end;
	return cur == right.cur;
}

bool operator==(set::iterator left, set::iterator right){
	return left.equal(right);
}

bool operator!=(set::iterator left, set::iterator right){
	return !left.equal(right);
}

void set::info(set::iterator a){
	node* b = a.cur;
	std::cout << "value " << b->val << '\n';
	std::cout << "parent ";
	((b->parent) ? std::cout << (b->parent->val) : std::cout << "none") << '\n';
	std::cout << "right ";
	((b->right) ? std::cout << b->right->val :std::cout << "none") << '\n';
	std::cout << "left "; 
	((b->left) ? std::cout << b->left->val : std::cout <<"none") << '\n';

}

set::value_type main()
{
    set c;
    c.insert(5);
    c.insert(6);
    c.insert(0);
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
   	it3--;
    std::cout << *it3;

       
}