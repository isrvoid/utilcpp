/*
Copyright (c) 2016 Johannes Teichrieb
License: opensource.org/licenses/MIT
*/
#pragma once

#include <string>
#include <semaphore.h>

namespace util {

class Semaphore {
    const std::string name;
    sem_t* const sem;

public:
    Semaphore(std::string&& name);
    Semaphore(std::string&& name, size_t init);

    size_t value();
    void post();
    void wait();
    void setValue(size_t value);

#ifdef UNITTEST
    void unlink();
#endif

private:
    void increaseBy(size_t n);
    void decreaseBy(size_t n);
};

} // namespace util
