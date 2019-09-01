#include "semaphore.h"

#include <stdexcept>
#include <limits>
#include <cassert>
#include <algorithm>
#include <fcntl.h>

using namespace std;

namespace util {

Semaphore::Semaphore(string&& name) : name(name), sem(sem_open(name.data(), O_CREAT, 0644, 0)) {
    if (sem == SEM_FAILED)
        throw runtime_error("Couldn't open semaphore.");
}

Semaphore::Semaphore(string&& name, size_t init) : Semaphore(move(name)) {
    setValue(init);
}

size_t Semaphore::value() {
    int val{};
    sem_getvalue(sem, &val);
    return max(0, val);
}

void Semaphore::post() {
    sem_post(sem);
}

void Semaphore::wait() {
    sem_wait(sem);
}

void Semaphore::setValue(size_t goal) {
    assert(goal <= numeric_limits<int>::max());
    auto current = value();
    if (goal > current)
        increaseBy(goal - current);
    else if (goal < current)
        decreaseBy(current - goal);
}

#ifdef UNITTEST
void Semaphore::unlink() {
    sem_unlink(name.data());
}
#endif

void Semaphore::increaseBy(size_t n) {
    for (size_t i = 0; i < n; ++i)
        post();
}

void Semaphore::decreaseBy(size_t n) {
    for (size_t i = 0; i < n; ++i)
        wait();
}

} // namespace util
