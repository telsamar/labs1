#include <gtest/gtest.h>

#include "SharedPtr.hpp"
#include <string>


TEST(SharedPtr, FromPointer)
{
    auto sourcePtr = new int{789};
    auto ptr = SharedPtr<int>{sourcePtr};

    EXPECT_EQ(static_cast<bool>(ptr), true);
    EXPECT_EQ(ptr.use_count(), 1u);
    EXPECT_EQ(ptr.get(), sourcePtr);
    EXPECT_EQ(*ptr, 789);
}

class SharedPtrFixture: public ::testing::Test
{
protected:
    struct Destructable
    {
        static bool isDestructed;

        ~Destructable()
        {
            Destructable::isDestructed = true;
        }
    };

    SharedPtr<std::string> ptrNone{};

    std::string *sourceString = new std::string{"I am a string"};
    SharedPtr<std::string> ptrString{sourceString};

    Destructable *destructable = new Destructable{};
    SharedPtr<Destructable> ptrDestructable{destructable};

    void SetUp() override
    {
        SharedPtrFixture::Destructable::isDestructed = false;
    }

    void TearDown() override
    {

    }
};

bool SharedPtrFixture::Destructable::isDestructed = false;

TEST_F(SharedPtrFixture, Empty)
{
    EXPECT_EQ(static_cast<bool>(ptrNone), false);
    EXPECT_EQ(ptrNone.use_count(), 1u);
    EXPECT_EQ(ptrNone.get(), nullptr);
}

TEST_F(SharedPtrFixture, FromPointerString)
{
    EXPECT_EQ(static_cast<bool>(ptrString), true);
    EXPECT_EQ(ptrString.use_count(), 1u);
    EXPECT_EQ(ptrString.get(), sourceString);
    EXPECT_EQ(*ptrString, std::string("I am a string"));
    EXPECT_EQ(ptrString->size(), sourceString->size());
}

TEST_F(SharedPtrFixture, CopyConstructor)
{
    ASSERT_EQ(std::is_copy_constructible_v<SharedPtr<int>>, true);

    SharedPtr<std::string> newString{ptrString};

    EXPECT_EQ(static_cast<bool>(newString), true);
    EXPECT_EQ(newString.use_count(), 2u);
    EXPECT_EQ(newString.get(), sourceString);
    EXPECT_EQ(*newString, std::string("I am a string"));
    EXPECT_EQ(newString->size(), sourceString->size());

    EXPECT_EQ(static_cast<bool>(ptrString), true);
    EXPECT_EQ(ptrString.use_count(), 2u);
    EXPECT_EQ(ptrString.get(), sourceString);
    EXPECT_EQ(*ptrString, std::string("I am a string"));
    EXPECT_EQ(ptrString->size(), sourceString->size());
}

TEST_F(SharedPtrFixture, MoveConstructor)
{
    ASSERT_EQ(std::is_move_constructible_v<SharedPtr<int>>, true);

    SharedPtr<std::string> newString{std::move(ptrString)};

    EXPECT_EQ(static_cast<bool>(newString), true);
    EXPECT_EQ(newString.use_count(), 1u);
    EXPECT_EQ(newString.get(), sourceString);
    EXPECT_EQ(*newString, std::string("I am a string"));
    EXPECT_EQ(newString->size(), sourceString->size());

    EXPECT_EQ(static_cast<bool>(ptrString), false);
    EXPECT_EQ(ptrNone.get(), nullptr);
}

TEST_F(SharedPtrFixture, AssignmentCopy)
{
    ASSERT_EQ(std::is_copy_assignable_v<SharedPtr<int>>, true);

    SharedPtr<std::string> newString;
    newString = ptrString;

    EXPECT_EQ(static_cast<bool>(newString), true);
    EXPECT_EQ(newString.use_count(), 2u);
    EXPECT_EQ(newString.get(), sourceString);
    EXPECT_EQ(*newString, std::string("I am a string"));
    EXPECT_EQ(newString->size(), sourceString->size());

    EXPECT_EQ(static_cast<bool>(ptrString), true);
    EXPECT_EQ(ptrString.use_count(), 2u);
    EXPECT_EQ(ptrString.get(), sourceString);
    EXPECT_EQ(*ptrString, std::string("I am a string"));
    EXPECT_EQ(ptrString->size(), sourceString->size());
}

TEST_F(SharedPtrFixture, AssignmentCopySelf)
{
    SharedPtr<std::string> *dirtyPointer = &ptrString;
    ptrString = *dirtyPointer;      // Copy it to itself

    EXPECT_EQ(static_cast<bool>(ptrString), true);
    EXPECT_EQ(ptrString.use_count(), 1u);
    EXPECT_EQ(ptrString.get(), sourceString);
    EXPECT_EQ(*ptrString, std::string("I am a string"));
    EXPECT_EQ(ptrString->size(), sourceString->size());
}

TEST_F(SharedPtrFixture, AssignmentMove)
{
    ASSERT_EQ(std::is_move_assignable_v<SharedPtr<int>>, true);

    SharedPtr<std::string> newString;
    newString = std::move(ptrString);

    EXPECT_EQ(static_cast<bool>(newString), true);
    EXPECT_EQ(newString.use_count(), 1u);
    EXPECT_EQ(newString.get(), sourceString);
    EXPECT_EQ(*newString, std::string("I am a string"));
    EXPECT_EQ(newString->size(), sourceString->size());

    EXPECT_EQ(static_cast<bool>(ptrString), false);
    EXPECT_EQ(ptrNone.get(), nullptr);
}

TEST_F(SharedPtrFixture, AssignmentMoveSelf)
{
    SharedPtr<std::string> *dirtyPointer = &ptrString;
    ptrString = std::move(*dirtyPointer);

    EXPECT_EQ(static_cast<bool>(ptrString), true);
    EXPECT_EQ(ptrString.use_count(), 1u);
    EXPECT_EQ(ptrString.get(), sourceString);
    EXPECT_EQ(*ptrString, std::string("I am a string"));
    EXPECT_EQ(ptrString->size(), sourceString->size());
}

TEST_F(SharedPtrFixture, Reset)
{
    EXPECT_EQ(SharedPtrFixture::Destructable::isDestructed, false);

    ptrDestructable.reset();

    EXPECT_EQ(SharedPtrFixture::Destructable::isDestructed, true);

    EXPECT_EQ(static_cast<bool>(ptrDestructable), false);
    EXPECT_EQ(ptrDestructable.use_count(), 1u);
    EXPECT_EQ(ptrDestructable.get(), nullptr);
}

TEST_F(SharedPtrFixture, ResetWithPointer)
{
    EXPECT_EQ(SharedPtrFixture::Destructable::isDestructed, false);

    auto *newDestructable = new Destructable{};
    ptrDestructable.reset(newDestructable);

    EXPECT_EQ(SharedPtrFixture::Destructable::isDestructed, true);

    EXPECT_EQ(static_cast<bool>(ptrDestructable), true);
    EXPECT_EQ(ptrDestructable.use_count(), 1u);
    EXPECT_EQ(ptrDestructable.get(), newDestructable);
}

TEST_F(SharedPtrFixture, ResetWithPointerString)
{
    auto *newString = new std::string{"I am a new string"};
    ptrString.reset(newString);

    EXPECT_EQ(static_cast<bool>(ptrString), true);
    EXPECT_EQ(ptrString.use_count(), 1u);
    EXPECT_EQ(ptrString.get(), newString);
    EXPECT_EQ(*ptrString, std::string("I am a new string"));
    EXPECT_EQ(ptrString->size(), newString->size());
}

TEST_F(SharedPtrFixture, Swap)
{
    auto *newString = new std::string{"I am a new string"};
    SharedPtr<std::string> ptrNewString{newString};

    ptrString.swap(ptrNewString);

    EXPECT_EQ(static_cast<bool>(ptrString), true);
    EXPECT_EQ(ptrString.use_count(), 1u);
    EXPECT_EQ(ptrString.get(), newString);
    EXPECT_EQ(*ptrString, std::string("I am a new string"));
    EXPECT_EQ(ptrString->size(), newString->size());

    EXPECT_EQ(static_cast<bool>(ptrNewString), true);
    EXPECT_EQ(ptrNewString.use_count(), 1u);
    EXPECT_EQ(ptrNewString.get(), sourceString);
    EXPECT_EQ(*ptrNewString, std::string("I am a string"));
    EXPECT_EQ(ptrNewString->size(), sourceString->size());
}

TEST_F(SharedPtrFixture, SwapSelf)
{
    ptrString.swap(ptrString);

    EXPECT_EQ(static_cast<bool>(ptrString), true);
    EXPECT_EQ(ptrString.use_count(), 1u);
    EXPECT_EQ(ptrString.get(), sourceString);
    EXPECT_EQ(*ptrString, std::string("I am a string"));
    EXPECT_EQ(ptrString->size(), sourceString->size());
}


