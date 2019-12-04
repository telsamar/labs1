#pragma once

#include <any>
#include <string>

namespace Utils
{

bool isCharSpace(char c);

bool isCharQuote(char c);

bool isCharNumber(char c);

bool isCharEscaping(char c);

bool isCharSugar(char c);

double stringToNumber(const std::string &string);

template <typename T>
bool isAnyEqual(const std::any &any, T value) {
    if (any.type() != typeid(T)) {
        return false;
    }

    return std::any_cast<T>(any) == value;
}


}
