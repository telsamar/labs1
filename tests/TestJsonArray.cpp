#include <gtest/gtest.h>

#include "Json.hpp"

TEST(JsonArray, EmptyArray)
{
    Json json{"[]"};
    EXPECT_EQ(json.getSize(), 0u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);
}

TEST(JsonArray, SimpleArray)
{
    Json json{"[ 1 ]"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    EXPECT_EQ(std::any_cast<double>(json[0]), 1.);
}

TEST(JsonArray, MultitypeArray)
{
    Json json{R"([1.5, "I am a string", "another string", false])"};
    EXPECT_EQ(json.getSize(), 4u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    EXPECT_EQ(std::any_cast<double>(json[0]), 1.5);
    EXPECT_EQ(std::any_cast<std::string>(json[1]), "I am a string");
    EXPECT_EQ(std::any_cast<std::string>(json[2]), "another string");
    EXPECT_EQ(std::any_cast<bool>(json[3]), false);
}

TEST(JsonArray, NestedArray)
{
    Json json{"[ [ 1 ] ]"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    Json &nested = *std::any_cast<Json *>(json[0]);
    EXPECT_EQ(nested.getSize(), 1u);
    EXPECT_EQ(nested.is_object(), false);
    EXPECT_EQ(nested.is_array(), true);
    EXPECT_EQ(nested.is_null(), false);

    EXPECT_EQ(std::any_cast<double>(nested[0]), 1.);
}

TEST(JsonArray, NestedWithObject)
{
    Json json{R"([ {"k":false}])"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    Json &nested = *std::any_cast<Json *>(json[0]);
    EXPECT_EQ(nested.getSize(), 1u);
    EXPECT_EQ(nested.is_object(), true);
    EXPECT_EQ(nested.is_array(), false);
    EXPECT_EQ(nested.is_null(), false);

    EXPECT_EQ(std::any_cast<bool>(nested["k"]), false);
}

TEST(JsonArray, NumberWithoutSpaceAfter)
{
    Json json{"[1]"};

    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    EXPECT_EQ(std::any_cast<double>(json[0]), 1.);
}

TEST(JsonArray, WithObjects)
{
    Json json{R"([{"a": "b"}, {"b": true}, {"test": 123}])"};

    EXPECT_EQ(json.getSize(), 3u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    {
        Json &nested = *std::any_cast<Json *>(json[0]);
        EXPECT_EQ(nested.getSize(), 1u);
        EXPECT_EQ(nested.is_object(), true);
        EXPECT_EQ(nested.is_array(), false);
        EXPECT_EQ(nested.is_null(), false);

        EXPECT_EQ(std::any_cast<std::string>(nested["a"]), "b");
    }

    {
        Json &nested = *std::any_cast<Json *>(json[1]);
        EXPECT_EQ(nested.getSize(), 1u);
        EXPECT_EQ(nested.is_object(), true);
        EXPECT_EQ(nested.is_array(), false);
        EXPECT_EQ(nested.is_null(), false);

        EXPECT_EQ(std::any_cast<bool>(nested["b"]), true);
    }

    {
        Json &nested = *std::any_cast<Json *>(json[2]);
        EXPECT_EQ(nested.getSize(), 1u);
        EXPECT_EQ(nested.is_object(), true);
        EXPECT_EQ(nested.is_array(), false);
        EXPECT_EQ(nested.is_null(), false);

        EXPECT_EQ(std::any_cast<double>(nested["test"]), 123);
    }
}

TEST(JsonArray, WithNull)
{
    Json json{"[ null ]"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    EXPECT_EQ(json[0].has_value(), false);
}

TEST(JsonArray, WithAnything)
{
    Json json{R"(
        [
            null,
            654,
            false,
            [ 1, 2, 3 ],
            100,
            { "a" : "b" },
            "i am string"
        ]
    )"};

    EXPECT_EQ(json.getSize(), 7u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    // null
    EXPECT_EQ(json[0].has_value(), false);

    // 654
    EXPECT_EQ(std::any_cast<double>(json[1]), 654);

    // false
    EXPECT_EQ(std::any_cast<bool>(json[2]), false);

    {       // [ 1, 2, 3 ]
        Json &nested = *std::any_cast<Json *>(json[3]);

        EXPECT_EQ(nested.getSize(), 3u);
        EXPECT_EQ(nested.is_object(), false);
        EXPECT_EQ(nested.is_array(), true);
        EXPECT_EQ(nested.is_null(), false);

        // 1
        EXPECT_EQ(std::any_cast<double>(nested[0]), 1);

        // 2
        EXPECT_EQ(std::any_cast<double>(nested[1]), 2);

        // 3
        EXPECT_EQ(std::any_cast<double>(nested[2]), 3);
    }

    // 100
    EXPECT_EQ(std::any_cast<double>(json[4]), 100);

    {       // { "a" : "b" }
        Json &nested = *std::any_cast<Json *>(json[5]);
        EXPECT_EQ(nested.getSize(), 1u);
        EXPECT_EQ(nested.is_object(), true);
        EXPECT_EQ(nested.is_array(), false);
        EXPECT_EQ(nested.is_null(), false);

        EXPECT_EQ(std::any_cast<std::string>(nested["a"]), "b");
    }

    // "i am string"
    EXPECT_EQ(std::any_cast<std::string>(json[6]), "i am string");
}

TEST(JsonArray, WrongNumber)
{
    EXPECT_THROW(
        Json{R"([ 123 456 ])"},
        JsonParseException
    );
}

TEST(JsonArray, WrongNumberWithDots)
{
    EXPECT_THROW(
        Json{R"([ 1.2.3 ])"},
        JsonParseException
    );
}

TEST(JsonArray, HackString)
{
    Json json{R"([ "',..:", '".:,:' ])"};
    EXPECT_EQ(json.getSize(), 2u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    EXPECT_EQ(std::any_cast<std::string>(json[0]), "',..:");
    EXPECT_EQ(std::any_cast<std::string>(json[1]), R"(".:,:)");
}

TEST(JsonArray, BackslashInString)
{
    Json json{R"([ "word1 \" word2" ])"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);

    EXPECT_EQ(std::any_cast<std::string>(json[0]), "word1 \" word2");
}
