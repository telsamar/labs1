#include <stack>
#include <fstream>

#include "Json.hpp"
#include "JsonParser.hpp"

Json &Json::operator=(const Json &json)
{
    if (this == &json) {
        return *this;
    }

    this->~Json();

    if (json.objectData) {
        objectData = new ObjectType;

        // Копирование
        for (const auto &pair: *json.objectData) {
            if (pair.second.type() == typeid(Json *)) {
                auto &oldJson = *std::any_cast<Json *>(pair.second);

                static_cast<ObjectType &>(*objectData)[pair.first] = new Json(oldJson);
            } else {
                (*objectData)[pair.first] = pair.second;
            }
        }
    }
    if (json.arrayData) {
        arrayData = new ArrayType;
        arrayData->reserve(json.arrayData->size());

        // Копирование
        for (const std::any &value: *json.arrayData) {
            if (value.type() == typeid(Json *)) {
                auto &oldJson = *std::any_cast<Json *>(value);
                arrayData->emplace_back(new Json(oldJson));
            } else {
                arrayData->push_back(value);
            }
        }
    }

    return *this;
}

Json &Json::operator=(Json &&json) noexcept
{
    if (this == &json) {
        return *this;
    }

    this->~Json();

    objectData = json.objectData;
    arrayData = json.arrayData;
    json.objectData = nullptr;
    json.arrayData = nullptr;

    return *this;
}

void Json::addToObjectKey(const std::string &key, const std::any &value)
{
    if (!objectData) {
        throw JsonUnexpectedType("Expected JSON object");
    }

    (*objectData)[key] = value;
}

void Json::addToArray(const std::any &value)
{
    if (!arrayData) {
        throw JsonUnexpectedType("Expected JSON array");
    }

    arrayData->push_back(value);
}

std::vector<std::string> Json::getKeys() const
{
    if (!objectData) {
        throw JsonUnexpectedType("Expected JSON object");
    }

    std::vector<std::string> result;
    result.reserve(objectData->size());
    for (const auto &pair: *objectData) {
        result.push_back(pair.first);
    }

    return result;
}

bool Json::is_array() const
{
    if (Json::is_null()) {
        return false;
    }

    return arrayData != nullptr;
}

bool Json::is_object() const
{
    if (Json::is_null()) {
        return false;
    }

    return objectData != nullptr;
}

Json::Json(const std::string &string)
{
    std::unique_ptr<Json> result{JsonParser::parse(string)};
    *this = std::move(*result);
}

Json::~Json()
{
    if (objectData) {
        for (const auto &pair: *objectData) {
            if (pair.second.type() == typeid(Json *)) {
                delete std::any_cast<Json *>(pair.second);
            }
        }
    }

    if (arrayData) {
        for (const std::any &value: *arrayData) {
            if (value.type() == typeid(Json *)) {
                delete std::any_cast<Json *>(value);
            }
        }
    }

    delete objectData;
    delete arrayData;
}

std::any &Json::operator[](const std::string &key)
{
    if (!objectData) {
        throw JsonUnexpectedType("Expected JSON object");
    }

    if (objectData->find(key) == objectData->cend()) {
        throw JsonUnexpectedKey("Expected JSON object key: " + key);
    }

    return (*objectData)[key];
}

std::any &Json::operator[](int index)
{
    if (!arrayData) {
        throw JsonUnexpectedType("Expected JSON array");
    }

    if (arrayData->size() <= static_cast<size_t>(index)) {
        throw JsonUnexpectedKey("Expected JSON array index: " + std::to_string(index));
    }

    return (*arrayData)[index];
}

size_t Json::getSize() const
{
    if (arrayData) {
        return arrayData->size();
    }
    if (objectData) {
        return objectData->size();
    }
    return 0;
}

Json Json::parseFile(const std::string &pathToFile)
{
    std::ifstream fileStream(pathToFile);
    if (fileStream.fail()) {
        throw JsonParseFileException("Cannot read file: " + pathToFile);
    }

    std::string fullFile(
        std::istreambuf_iterator<char>{fileStream},
        std::istreambuf_iterator<char>()
    );

    return Json(fullFile);
}
