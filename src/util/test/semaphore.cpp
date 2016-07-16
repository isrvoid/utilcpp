#include <gtest/gtest.h>

#include <util/semaphore.h>

namespace {

class SemaphoreTest : public ::testing::Test {
protected:
	util::Semaphore sem{"/gtestfix", 1};

	virtual ~SemaphoreTest() {
		sem.unlink();
	}
};

TEST_F(SemaphoreTest, value) {
	ASSERT_EQ(1, sem.value());
}

TEST_F(SemaphoreTest, post) {
	sem.post();
	ASSERT_EQ(2, sem.value());
}

TEST_F(SemaphoreTest, wait) {
	sem.wait();
	ASSERT_EQ(0, sem.value());
}

TEST_F(SemaphoreTest, setValue) {
	sem.setValue(7);
	ASSERT_EQ(7, sem.value());
}

TEST_F(SemaphoreTest, setLowerValue) {
	sem.setValue(10);
	sem.setValue(3);
	ASSERT_EQ(3, sem.value());
}

TEST_F(SemaphoreTest, setSameValue) {
	auto initial = sem.value();
	sem.setValue(initial);
	ASSERT_EQ(initial, sem.value());
}

} // namespace
