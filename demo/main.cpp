#include <iostream>
#include <Json.hpp>

std::string hello()
{
    std::cout << "The demo app. Read JSON file and prints result\n";
    std::cout << "Enter filename: ";

    std::string filename;
    std::getline(std::cin, filename);

    return filename;
}

std::string operator*(const std::string &string, int number)
{
    std::string result;
    for (int i = 0; i < number; i++) {
        result += string;
    }
    return result;
}

void printTabs(size_t size)
{
    std::cout << std::string("-") * size << " ";
}

void printValue(const std::any &value, size_t &tabSize);

void printJson(Json &json)
{
    static size_t tabSize = 2;

    if (json.is_object()) {
        for (const std::string &key: json.getKeys()) {
            printTabs(tabSize);
            std::cout << key << " : ";

            printValue(json[key], tabSize);
        }

        tabSize -= 2;
    } else if (json.is_array()) {
        for (size_t i = 0; i < json.getSize(); i++) {
            printTabs(tabSize);

            printValue(json[i], tabSize);
        }

        tabSize -= 2;
    } else {
        std::cout << "NONE\n";
    }
}

int main()
{
    std::string filename = hello();

    auto json = Json::parseFile(filename);

    printJson(json);
}

void printValue(const std::any &value, size_t &tabSize)
{
    if (value.type() == typeid(Json *)) {
        tabSize += 2;
        std::cout << "\n";

        printJson(*std::any_cast<Json *>(value));
        return;
    }
    if (value.type() == typeid(std::string)) {
        std::cout << '"' << std::any_cast<std::string>(value) << "\"\n";
        return;
    }
    if (value.type() == typeid(double)) {
        std::cout << std::any_cast<double>(value) << "\n";
        return;
    } else if (value.type() == typeid(bool)) {
        std::cout << (std::any_cast<bool>(value) ? "true" : "false") << "\n";
        return;
    }

    std::cout << "null\n";
}