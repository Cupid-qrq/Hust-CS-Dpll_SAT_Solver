#pragma once

#include "definitions.h"

template <class T> class myArray {
public:
  // 数据、大小、容量
  T *data;
  int Size;
  int capacity;

  // 构造、析构函数
  myArray() : Size(0), capacity(1) { data = new T[1]; }

  ~myArray() { delete[] data; }

  // 有参构造,参数为初始大小和默认值
  myArray(int n, T t) : Size(n), capacity(n) {
    data = new T[n];
    for (int i = 0; i < n; i++) {
      data[i] = t;
    }
  }

  // 拷贝构造
  myArray(const myArray &other) : Size(other.Size), capacity(other.capacity) {
    data = new T[capacity];
    copy(other.data, other.data + Size, data);
  }

  // 重载赋值运算符
  myArray &operator=(const myArray &other) {
    if (this != &other) {
      delete[] data; // 释放旧内存
      Size = other.Size;
      capacity = other.capacity;
      data = new T[capacity];
      copy(other.data, other.data + Size, data);
    }
    return *this;
  }

  // STL常用函数模拟
  void push_back(T t) {
    if (Size >= capacity) {
      resize(capacity * 2); // 扩容
    }
    data[Size++] = t; // 添加新元素
  }

  void pop_back() {
    if (Size > 0) {
      Size--; // 减少大小
    } else {
      throw std::out_of_range("myArray is empty");
    }
  }

  int size() const {
    return this->Size; // 返回当前大小
  }

  T &operator[](int index) {
    if (index < 0 || index >= Size) {
      throw std::out_of_range("index out of range");
    }
    return data[index];
  }

  const T &operator[](int index) const {
    if (index < 0 || index >= Size) {
      throw std::out_of_range("index out of range");
    }
    return data[index];
  }

  bool empty() const {
    return Size == 0; // 检查是否为空
  }

  void clear() {
    Size = 0; // 清空向量
  }

  // 单纯扩容函数
  void resize(int new_capacity) {
    T *new_data = new T[new_capacity];
    copy(data, data + Size, new_data);
    delete[] data;
    data = new_data;
    capacity = new_capacity; // 调整容量
  }

  // 扩容并赋值函数
  void assign(int new_size, const T &value) {
    if (new_size > capacity) {
      resize(new_size);
    }
    for (int i = 0; i < new_size; i++) {
      data[i] = value;
    }
    Size = new_size;
  }

  // 迭代器类，用于实现如std::begin和std::end等函数
  class iterator {
    T *ptr;

  public:
    iterator(T *p) : ptr(p) {}

    T &operator*() { return *ptr; }
    T *operator->() { return ptr; }

    iterator &operator++() {
      ++ptr; // 前缀递增
      return *this;
    }
    iterator operator++(int) {
      iterator temp = *this; // 后缀递增
      ++ptr;
      return temp;
    }

    bool operator==(const iterator &other) const {
      return ptr == other.ptr; // 比较指针是否相等
    }
    bool operator!=(const iterator &other) const {
      return ptr != other.ptr; // 比较指针是否不相等
    }

    iterator &operator--() {
      --ptr; // 前缀递减
      return *this;
    }
    iterator operator--(int) {
      iterator temp = *this; // 后缀递减
      --ptr;
      return temp;
    }
  };

  // 常量迭代器类
  class const_iterator {
  public:
    const T *ptr;

    const_iterator(const T *p) : ptr(p) {}

    const T &operator*() const { return *ptr; }
    const T *operator->() const { return ptr; }

    const_iterator &operator++() {
      ++ptr; // 前缀递增
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator temp = *this; // 后缀递增
      ++ptr;
      return temp;
    }

    bool operator==(const const_iterator &other) const {
      return ptr == other.ptr; // 比较指针是否相等
    }
    bool operator!=(const const_iterator &other) const {
      return ptr != other.ptr; // 比较指针是否不相等
    }

    const_iterator &operator--() {
      --ptr; // 前缀递减
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator temp = *this; // 后缀递减
      --ptr;
      return temp;
    }
  };

  // 返回迭代器
  iterator begin() {
    return iterator(data); // 返回起始迭代器
  }

  iterator end() {
    return iterator(data + Size); // 返回结束迭代器
  }

  const_iterator begin() const {
    return const_iterator(data); // 返回常量起始迭代器
  }

  const_iterator end() const {
    return const_iterator(data + Size); // 返回常量结束迭代器
  }
};