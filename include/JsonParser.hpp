#pragma once

#include <stack>
#include <functional>
#include <memory>
#include <list>
#include "Json.hpp"

class JsonParser
{
public:
    using PartsType = std::list<std::any>;

    static Json *parse(const std::string &string);

private:
    static PartsType fullSplit(const std::string &input);

    static std::optional<std::any>
    ejectString(std::string::const_iterator &iterator, const std::string::const_iterator &end);

    static std::optional<std::any>
    ejectNumber(std::string::const_iterator &iterator, const std::string::const_iterator &end);

    static std::optional<std::any>
    ejectKeyword(std::string::const_iterator &iterator, const std::string::const_iterator &end);

    static std::any
    parsePart(PartsType::const_iterator &iterator, const PartsType::const_iterator &end, bool inStart = true);

    static Json *
    parseArray(PartsType::const_iterator &iterator, const PartsType::const_iterator &end);

    static Json *
    parseObject(PartsType::const_iterator &iterator, const PartsType::const_iterator &end);
};