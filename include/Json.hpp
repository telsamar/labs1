#pragma once

#include <string>
#include <any>
#include <unordered_map>
#include <vector>

#include "JsonException.hpp"

class Json
{
public:
    using ObjectType = std::unordered_map<std::string, std::any>;       // Тип сериализованного json-объекта
    using ArrayType = std::vector<std::any>;                            // Тип сериализованного json-массива

    // Конструктор из строки, содержащей Json-данные.
    explicit Json(const std::string &string);

    // Конструктор из сериализованного объекта
    explicit Json(const ObjectType &object)
        : objectData(new ObjectType(object))
    {}

    // Конструктор из сериализованного массива
    explicit Json(const ArrayType &object)
        : arrayData(new ArrayType(object))
    {}

    // Пустой конструктор
    Json() = default;

    // Конструктор копирования
    Json(const Json &json)
    {
        *this = json;
    }

    // Конструктор перемещения
    Json(Json &&json) noexcept
    {
        *this = std::forward<Json>(json);
    }

    // Копирующее присваивание
    Json &operator=(const Json &json);

    // Перемещающее присваивание
    Json &operator=(Json &&json) noexcept;

    // Добавить в словарь ключ, значение
    void addToObjectKey(const std::string &key, const std::any &value);

    // Добавить значение в массив
    void addToArray(const std::any &value);

    // Получить список ключей, если JSON-объект
    [[nodiscard]] std::vector<std::string> getKeys() const;

    // Метод возвращает true, если данный экземпляр содержит в себе JSON-массив. Иначе false.
    [[nodiscard]] bool is_array() const;

    // Метод возвращает true, если данный экземпляр содержит в себе JSON-объект. Иначе false.
    [[nodiscard]] bool is_object() const;

    // Метод возвращает true, если данный экземпляр не содержит в себе не объект, не массив.
    [[nodiscard]] bool is_null() const
    {
        return objectData == nullptr && arrayData == nullptr;
    }

    // Метод возвращает размер json множества (как массива, так и объекта)
    [[nodiscard]] size_t getSize() const;

    // Метод возвращает значение по ключу key, если экземпляр является JSON-объектом.
    // Значение может иметь один из следующих типов: Json, std::string, double, bool или быть пустым.
    // Если экземпляр является JSON-массивом, генерируется исключение.
    std::any &operator[](const std::string &key);

    // Метод возвращает значение по индексу index, если экземпляр является JSON-массивом.
    // Значение может иметь один из следующих типов: Json, std::string, double, bool или быть пустым.
    // Если экземпляр является JSON-объектом, генерируется исключение.
    std::any &operator[](int index);

    // Метод возвращает объект класса Json из строки, содержащей Json-данные.
    static Json parse(const std::string &string)
    {
        return Json(string);
    }

    // Метод возвращает объекта класса Json из файла, содержащего Json-данные в текстовом формате.
    static Json parseFile(const std::string &pathToFile);

    virtual ~Json();

private:
    ObjectType *objectData = nullptr;
    ArrayType *arrayData = nullptr;
};