//
// Created by Yunan Chen on 2022/12/29.
//

#ifndef THREADPOOL_NONCOPYABLE_H
#define THREADPOOL_NONCOPYABLE_H
class noncopyable {
protected:
    // 默认的构造函数和析构函数是 protected，
    // 不允许创建 noncopyable 实例，但允许子类创建实例
    // （即允许派生类构造和析构）。
    noncopyable() = default;
    ~noncopyable() = default;

private:
    // 使用 delete 关键字禁止编译器自动产生复制构造函数和复制赋值操作符。
    noncopyable(const noncopyable&) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};
#endif //THREADPOOL_NONCOPYABLE_H
