#include <gtest/gtest.h>

#include "Json.hpp"

TEST(JsonObject, EmptyObject)
{
    Json json{"{}"};
    EXPECT_EQ(json.getSize(), 0u);
    EXPECT_EQ(json.is_object(), true);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), false);
}

TEST(JsonObject, EmptyObjectWithSpaces)
{
    Json json{"{  \n  \t \t \n }"};
    EXPECT_EQ(json.getSize(), 0u);
    EXPECT_EQ(json.is_object(), true);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), false);
}

TEST(JsonObject, SimpleObject)
{
    Json json{R"({ "key" : "value" })"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), true);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), false);

    EXPECT_EQ(std::any_cast<std::string>(json["key"]), "value");
}

TEST(JsonObject, NestedObject)
{
    Json json{R"({ "key" : { "number":   9.5 } })"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), true);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), false);

    Json &nested = *std::any_cast<Json *>(json["key"]);
    EXPECT_EQ(nested.getSize(), 1u);
    EXPECT_EQ(nested.is_object(), true);
    EXPECT_EQ(nested.is_array(), false);
    EXPECT_EQ(nested.is_null(), false);

    EXPECT_EQ(std::any_cast<double>(nested["number"]), 9.5);
}

TEST(JsonObject, NestedWithArray)
{
    Json json{R"({ "key" : [1,2,3] })"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), true);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), false);

    Json &nested = *std::any_cast<Json *>(json["key"]);
    EXPECT_EQ(nested.getSize(), 3u);
    EXPECT_EQ(nested.is_object(), false);
    EXPECT_EQ(nested.is_array(), true);
    EXPECT_EQ(nested.is_null(), false);

    EXPECT_EQ(std::any_cast<double>(nested[0]), 1);
}

TEST(JsonObject, NestedWithArrayWithValues)
{
    Json json{R"({ "key" : [1,2,3], "key2": true })"};
    EXPECT_EQ(json.getSize(), 2u);
    EXPECT_EQ(json.is_object(), true);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), false);

    Json &nested = *std::any_cast<Json *>(json["key"]);
    EXPECT_EQ(nested.getSize(), 3u);
    EXPECT_EQ(nested.is_object(), false);
    EXPECT_EQ(nested.is_array(), true);
    EXPECT_EQ(nested.is_null(), false);

    EXPECT_EQ(std::any_cast<double>(nested[0]), 1);
    EXPECT_EQ(std::any_cast<double>(nested[1]), 2);
    EXPECT_EQ(std::any_cast<double>(nested[2]), 3);

    EXPECT_EQ(std::any_cast<bool>(json["key2"]), true);
}

TEST(JsonObject, WrongKeywordStart)
{
    EXPECT_THROW(
        Json{R"({ "some_key": "some_value", "new_key": undefined    })"},
        JsonParseException
    );
}

TEST(JsonObject, WrongEnd)
{
    EXPECT_THROW(
        Json{R"({ "key": {    })"},
        JsonParseException
    );
}

TEST(JsonObject, WrongObject)
{
    EXPECT_THROW(
        Json{R"({ "some_key": "some_value", "value again"    })"},
        JsonParseException
    );
}



TEST(JsonObject, WrongKeywordEnd)
{
    EXPECT_THROW(
        Json{R"({ "some_key": "some_value", "new_key": fallen    })"},
        JsonParseException
    );
}

TEST(JsonObject, CharAfterEndException)
{
    EXPECT_THROW(
        Json{R"({},)"},
        JsonParseException
    );
}

TEST(JsonObject, ExpectedKeyException)
{
    EXPECT_THROW(
        Json{R"({ 123: 123 })"},
        JsonParseException
    );
}

TEST(JsonObject, ExpectedSomething)
{
    EXPECT_THROW(
        Json{R"({ "123": })"},
        JsonParseException
    );
}

TEST(JsonObject, KeyRepeat)
{
    EXPECT_THROW(
        Json{R"({ "123":1, "123": 2 })"},
        JsonParseException
    );
}

TEST(JsonObject, BackslashInKey)
{
    Json json{R"({ "1\"2" : true })"};
    EXPECT_EQ(json.getSize(), 1u);
    EXPECT_EQ(json.is_object(), true);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), false);

    EXPECT_EQ(std::any_cast<bool>(json["1\"2"]), true);
}

TEST(JsonObject, BackslashInKeyBad)
{
    EXPECT_THROW(
        Json{R"({ "\1"":1 })"},
        JsonParseException
    );
}

