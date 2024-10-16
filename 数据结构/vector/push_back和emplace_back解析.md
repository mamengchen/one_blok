---
title: 数据结构/vector/push_back和emplace_back解析
category: /小书匠/日记/2024-10
grammar_cjkRuby: true
---

emplace_back 是 C++ 标准库中的 std::vector 提供的成员函数，用于在容器的末尾直接构造元素，而不是像 push_back 那样复制或移动一个已经存在的对象。
emplace_back 可以减少不必要的拷贝和移动操作，提升性能，尤其是在需要频繁添加对象时。

----------
### emplace_back 的工作原理
emplace_back 允许你传入构造对象所需的参数，直接在容器的内存位置上构造对象。<font color = "red">这意味着对象会在目标位置直接构造，避免了创建临时对象然后移动或拷贝的开销。</font>
> 语法： vector.emplace_back(args....);
> args... ：是传递给要构造对象的构造函数的参数。

---

### 1. push_back 实现
push_back 的主要功能是将现有对象添加到std::vector的末尾。
它有两种方式：

- 如果传入是左值引用，它会调用拷贝构造函数，将对象复制到容器中。
- 如果传入是右值引用，它会调用移动构造函数，将对象移动到容器中。

``` c++
template <typename T>
void push_back(const T& value) {
    if (size == capacity) {
        reallocate();  // 如果容量不足，重新分配空间
    }
    // 使用拷贝构造函数，将对象复制到新的内存空间
    new (&data[size]) T(value);
    ++size;
}

template <typename T>
void push_back(T&& value) {
    if (size == capacity) {
        reallocate();  // 如果容量不足，重新分配空间
    }
    // 使用移动构造函数，将对象移动到新的内存空间
    new (&data[size]) T(std::move(value));
    ++size;
}

```

### 2. emplace_back 实现
emplace_back 不需要现有的对象，而是直接在容器的内存中原地构造对象。它使用模板参数的完美转发来调用对象的构造函数。
``` c++
template <typename... Args>
void emplace_back(Args&&... args) {
    if (size == capacity) {
        reallocate();  // 如果容量不足，重新分配空间
    }
    // 使用传入的参数直接构造对象，不需要临时对象或额外的拷贝/移动
    new (&data[size]) T(std::forward<Args>(args)...);
    ++size;
}
```
**工作原理**：与push_back类似，首先检查是否需要重新分配空间。然后通过传入的参数列表，直接在容器内存中使用 placement new构造对象。

### 3.区别、性能差异

push_back 需要一个已经存在的对象，操作（是左值调用拷贝构造函数，如果是右值则调用移动构造函数）
emplace_back 可以直接使用参数构造对象，无需先创建临时对象。
通常来说 emplace_back 性能更高，尤其在对象较为复杂的情况下。
