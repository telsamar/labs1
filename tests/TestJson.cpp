#include <gtest/gtest.h>

#include "Json.hpp"

TEST(Json, NullJson)
{
    Json json{};
    EXPECT_EQ(json.getSize(), 0u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), true);
}

TEST(Json, ExampleJson)
{
    Json object = Json::parse(R"(
        {
            "lastname" : "Ivanov",
            "firstname" : "Ivan",
            "age" : 25,
            "islegal" : false,
            "marks" : [
                4,5,5,5,2,3
            ],
            "address" : {
                "city" : "Moscow",
                "street" : "Vozdvijenka"
            }
        }
    )");

    EXPECT_EQ(std::any_cast<std::string>(object["lastname"]), "Ivanov");
    EXPECT_EQ(std::any_cast<bool>(object["islegal"]), false);
    EXPECT_EQ(std::any_cast<double>(object["age"]), 25);

    Json &marks = *std::any_cast<Json *>(object["marks"]);
    EXPECT_EQ(std::any_cast<double>(marks[0]), 4);
    EXPECT_EQ(std::any_cast<double>(marks[1]), 5);

    Json &address = *std::any_cast<Json *>(object["address"]);
    EXPECT_EQ(std::any_cast<std::string>(address["city"]), "Moscow");
    EXPECT_EQ(std::any_cast<std::string>(address["street"]), "Vozdvijenka");
}

TEST(Json, ParseFile)
{
    auto json = Json::parseFile("../tests/TestData.json");

    EXPECT_EQ(json.getSize(), 2u);
    EXPECT_EQ(json.is_object(), true);
    EXPECT_EQ(json.is_array(), false);
    EXPECT_EQ(json.is_null(), false);
}

TEST(Json, ParseFileException)
{
    EXPECT_THROW(
        Json::parseFile("__definitely_not_existing_file__"),
        JsonParseFileException
    );
}

TEST(Json, CopyAssigmentOperator)
{
    ASSERT_EQ(std::is_copy_assignable_v<Json>, true);

    const Json &json = Json{R"({"key":[[1,2],2], "k": "v"})"};

    Json jsonCopy = json;
    EXPECT_EQ(jsonCopy.getSize(), 2u);
    EXPECT_EQ(jsonCopy.is_object(), true);
    EXPECT_EQ(jsonCopy.is_array(), false);
    EXPECT_EQ(jsonCopy.is_null(), false);

    {
        Json &nested = *std::any_cast<Json *>(jsonCopy["key"]);

        EXPECT_EQ(nested.getSize(), 2u);
        EXPECT_EQ(nested.is_object(), false);
        EXPECT_EQ(nested.is_array(), true);
        EXPECT_EQ(nested.is_null(), false);

        {
            Json &list = *std::any_cast<Json *>(nested[0]);

            EXPECT_EQ(list.getSize(), 2u);
            EXPECT_EQ(list.is_object(), false);
            EXPECT_EQ(list.is_array(), true);
            EXPECT_EQ(list.is_null(), false);
        }
    }

    EXPECT_EQ(std::any_cast<std::string>(jsonCopy["k"]), "v");
}

TEST(Json, CopyAssigmentOperatorSelf)
{
    Json json{R"([1,2,3])"};
    Json *dirtyPointer = &json;

    json = *dirtyPointer;
    EXPECT_EQ(json.getSize(), 3u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);
}

TEST(Json, MoveAssigmentOperator)
{
    ASSERT_EQ(std::is_move_assignable_v<Json>, true);

    Json json = Json{R"({"key":[1,2]})"};

    Json jsonCopy = std::move(json);
    EXPECT_EQ(json.getSize(), 0u);

    EXPECT_EQ(jsonCopy.getSize(), 1u);
    EXPECT_EQ(jsonCopy.is_object(), true);
    EXPECT_EQ(jsonCopy.is_array(), false);
    EXPECT_EQ(jsonCopy.is_null(), false);

    {
        Json &nested = *std::any_cast<Json *>(jsonCopy["key"]);

        EXPECT_EQ(nested.getSize(), 2u);
        EXPECT_EQ(nested.is_object(), false);
        EXPECT_EQ(nested.is_array(), true);
        EXPECT_EQ(nested.is_null(), false);
    }
}

TEST(Json, MoveAssigmentOperatorSelf)
{
    Json json{R"([1,2,3])"};
    Json *dirtyPointer = &json;

    *dirtyPointer = std::move(json);
    EXPECT_EQ(json.getSize(), 3u);
    EXPECT_EQ(json.is_object(), false);
    EXPECT_EQ(json.is_array(), true);
    EXPECT_EQ(json.is_null(), false);
}

TEST(Json, AddToObject)
{
    Json json{"{}"};
    json.addToObjectKey("key", std::string("value"));

    EXPECT_EQ(std::any_cast<std::string>(json["key"]), "value");
}

TEST(Json, AddToObjectException)
{
    Json json{"[]"};

    EXPECT_THROW(
        json.addToObjectKey("key", std::string("value")),
        JsonException
    );
}

TEST(Json, AddToArrayException)
{
    Json json{"{}"};

    EXPECT_THROW(
        json.addToArray(std::string("value")),
        JsonException
    );
}

TEST(Json, GetKeys)
{
    Json json{R"({
        "a": 1,
        "b": 1,
        "c": 1
    })"};

    auto keys = json.getKeys();
    EXPECT_NE(std::find(keys.cbegin(), keys.cend(), "a"), keys.cend());
    EXPECT_NE(std::find(keys.cbegin(), keys.cend(), "b"), keys.cend());
    EXPECT_NE(std::find(keys.cbegin(), keys.cend(), "c"), keys.cend());
}

TEST(Json, GetKeysException)
{
    Json json{"[]"};

    EXPECT_THROW(
        auto keys = json.getKeys(),
        JsonException
    );
}

TEST(Json, OperatorArrayTypeException)
{
    Json json{"[]"};

    EXPECT_THROW(
        json["test"],
        JsonException
    );
}

TEST(Json, OperatorObjectTypeException)
{
    Json json{"{}"};

    EXPECT_THROW(
        json[0],
        JsonException
    );
}

TEST(Json, OperatorArrayExceptionSize)
{
    Json json{"[]"};

    EXPECT_THROW(
        json[999],
        JsonException
    );
}

TEST(Json, OperatorObjectTypeExceptionNoKey)
{
    Json json{"{'key':1}"};

    EXPECT_THROW(
        json["__definitely_not_existing_key__"],
        JsonException
    );
}

TEST(Json, NoJson)
{
    EXPECT_THROW(
        Json json{"false"},
        JsonParseException
    );
}