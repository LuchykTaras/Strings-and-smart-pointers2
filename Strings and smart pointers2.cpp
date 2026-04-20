#include <iostream>
#include <utility>
#include <cstddef>

// ===== Реалізація SmartUniquePtr =====
template <typename T>
class SmartUniquePtr {
private:
    T* ptr;
public:
    explicit SmartUniquePtr(T* p = nullptr) : ptr(p) {}

    // Забороняємо копіювання
    SmartUniquePtr(const SmartUniquePtr&) = delete;
    SmartUniquePtr& operator=(const SmartUniquePtr&) = delete;

    // Дозволяємо переміщення
    SmartUniquePtr(SmartUniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    SmartUniquePtr& operator=(SmartUniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    ~SmartUniquePtr() {
        delete ptr;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
};

// ===== Реалізація SmartSharedPtr =====
template <typename T>
class SmartSharedPtr {
private:
    T* ptr;
    std::size_t* count;
public:
    explicit SmartSharedPtr(T* p = nullptr) : ptr(p), count(new std::size_t(1)) {}

    SmartSharedPtr(const SmartSharedPtr& other) : ptr(other.ptr), count(other.count) {
        ++(*count);
    }

    SmartSharedPtr& operator=(const SmartSharedPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            count = other.count;
            ++(*count);
        }
        return *this;
    }

    ~SmartSharedPtr() {
        release();
    }

    void release() {
        if (count) {
            --(*count);
            if (*count == 0) {
                delete ptr;
                delete count;
            }
        }
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    std::size_t use_count() const { return *count; }
};

// ===== Тестовий клас =====
struct Test {
    int value;
    Test(int v) : value(v) {}
    void show() { std::cout << "Value: " << value << std::endl; }
};

// ===== Тестування =====
int main() {
    std::cout << "=== Тест SmartUniquePtr ===" << std::endl;
    SmartUniquePtr<Test> uptr(new Test(42));
    uptr->show();

    SmartUniquePtr<Test> uptr2 = std::move(uptr);
    if (!uptr.get())
        std::cout << "uptr тепер пустий після move" << std::endl;
    uptr2->show();

    std::cout << "\n=== Тест SmartSharedPtr ===" << std::endl;
    SmartSharedPtr<Test> sptr1(new Test(100));
    SmartSharedPtr<Test> sptr2 = sptr1; // копія
    SmartSharedPtr<Test> sptr3 = sptr2; // ще одна копія

    sptr1->show();
    std::cout << "use_count: " << sptr1.use_count() << std::endl;

    sptr2->show();
    std::cout << "use_count: " << sptr2.use_count() << std::endl;

    sptr3->show();
    std::cout << "use_count: " << sptr3.use_count() << std::endl;

    return 0;
}
