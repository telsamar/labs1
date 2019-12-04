#include <boost/algorithm/string.hpp>
#include <functional>
#include <iostream>
#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include "JsonParser.hpp"
#include "Utils.hpp"

Json *JsonParser::parse(const std::string &string)
{
    PartsType parts = fullSplit(string);

    auto it = parts.cbegin();
    std::unique_ptr<Json> result{std::any_cast<Json *>(parsePart(it, parts.cend()))};

    if (it != parts.cend()) {
        throw JsonParseUnexpectedChar{"Excepted end of JSON"};
    }

    return result.release();
}

std::optional<std::any>
JsonParser::ejectString(std::string::const_iterator &iterator, const std::string::const_iterator &end)
{
    if (!Utils::isCharQuote(*iterator)) {
        return std::nullopt;
    }

    char openQuote = *iterator;
    auto stringStart = ++iterator;

    std::string::const_iterator stringEnd;
    do {
        stringEnd = std::find(iterator, end, openQuote);
        if (stringEnd == end) {
            throw JsonParseUnexpectedEof{"Expected end of the string"};
        }

        iterator = stringEnd;
        iterator++;
    } while (Utils::isCharEscaping(*(stringEnd - 1)));

    std::string result(stringStart, stringEnd);

    static const std::unordered_map<std::string, std::string> toReplace = {
        {"\\\\", "\\"},
        {"\\\"", "\""},
        {"\\\'", "\'"},
        {"\\n", "\n"},
        {"\\t", "\t"},
    };
    for (const auto &pair : toReplace) {
        boost::replace_all(result, pair.first, pair.second);
    }

    return result;
}

std::optional<std::any>
JsonParser::ejectNumber(std::string::const_iterator &iterator, const std::string::const_iterator &end)
{
    if (!Utils::isCharNumber(*iterator)) {
        return std::nullopt;
    }

    auto startNumber = std::find_if(iterator, end, Utils::isCharNumber);
    if (startNumber == end) {
        return std::nullopt;
    }

    auto endNumber = std::find_if_not(startNumber, end, Utils::isCharNumber);
    iterator = endNumber;

    std::string number = std::string(startNumber, endNumber);
    try {
        return Utils::stringToNumber(number);
    } catch (boost::bad_lexical_cast &) {
        throw JsonParseCannotParseNumber{"Cannot parse number '" + number + "'"};
    }
}

std::optional<std::any>
JsonParser::ejectKeyword(std::string::const_iterator &iterator, const std::string::const_iterator &end)
{
    static const std::unordered_map<std::string, std::any> results = {
        {"true", static_cast<bool>(true)},
        {"false", static_cast<bool>(false)},
        {"null", std::any{}},
    };

    size_t length = end - iterator;

    for (const auto &pair : results) {
        if (length < pair.first.size()) {
            continue;
        }

        if (std::string(iterator, iterator + pair.first.size()) == pair.first) {
            iterator += pair.first.size();
            return pair.second;
        }
    }

    return std::nullopt;
}

JsonParser::PartsType JsonParser::fullSplit(const std::string &input)
{
    std::list<std::function<
        std::optional<std::any>(std::string::const_iterator &, const std::string::const_iterator &)
    >> ejectors = {
        &ejectString,
        &ejectNumber,
        &ejectKeyword,
    };

    PartsType splitted;
    for (std::string::const_iterator it = input.cbegin(); it != input.cend();) {
        std::optional<std::any> part;
        for (auto &ejector : ejectors) {
            part = ejector(it, input.cend());
            if (part.has_value()) {
                splitted.emplace_back(part.value());
                break;
            }
        }
        if (part.has_value()) {
            continue;
        }

        if (Utils::isCharSugar(*it)) {
            splitted.emplace_back(std::string{*it});
            it++;
            continue;
        }
        if (Utils::isCharSpace(*it)) {
            it = std::find_if_not(it, input.cend(), Utils::isCharSpace);
            continue;
        }
        throw JsonParseUnexpectedChar{"Unexpected char '" + std::string{*it} + "'"};

    }
    return splitted;
}

std::any
JsonParser::parsePart(std::list<std::any>::const_iterator &iterator,
                      const std::list<std::any>::const_iterator &end,
                      bool inStart)
{
    static auto startValid = [](const std::string &str) {
        if (str.size() > 1) {
            return false;
        }

        return boost::is_any_of("[{")(str[0]);
    };

    if (inStart && (iterator->type() != typeid(std::string) || !startValid(std::any_cast<std::string>(*iterator)))) {
        throw JsonParseUnexpectedChar{"Expected start of JSON"};
    }

    if (Utils::isAnyEqual(*iterator, std::string{"["})) {
        return parseArray(++iterator, end);
    }
    if (Utils::isAnyEqual(*iterator, std::string{"{"})) {
        return parseObject(++iterator, end);
    }
    auto prev = iterator;
    iterator++;
    return *prev;
}

Json *
JsonParser::parseArray(PartsType::const_iterator &iterator, const PartsType::const_iterator &end)
{
    auto jsonResult = std::make_unique<Json>(Json::ArrayType{});

    if (Utils::isAnyEqual(*iterator, std::string{"]"})) {
        iterator++;
        return jsonResult.release();
    }

    while (iterator != end) {
        std::any part = parsePart(iterator, end, false);
        jsonResult->addToArray(part);

        if (Utils::isAnyEqual(*iterator, std::string{"]"})) {
            iterator++;
            return jsonResult.release();
        }
        if (!Utils::isAnyEqual(*iterator, std::string{","})) {
            throw JsonParseUnexpectedChar{"Expected ','"};
        }

        iterator++;
    }

    throw JsonParseUnexpectedEof{"Expected end of array"};
}

Json *
JsonParser::parseObject(std::list<std::any>::const_iterator &iterator, const std::list<std::any>::const_iterator &end)
{
    auto jsonResult = std::make_unique<Json>(Json::ObjectType{});

    if (Utils::isAnyEqual(*iterator, std::string{"}"})) {
        iterator++;
        return jsonResult.release();
    }

    while (iterator != end) {
        if (iterator->type() != typeid(std::string)) {
            throw JsonParseUnexpectedChar{"Expected key"};
        }

        auto key = std::any_cast<std::string>(*iterator);
        if (auto keys = jsonResult->getKeys(); std::find(keys.cbegin(), keys.cend(), key) != keys.cend()) {
            throw JsonParseDuplicatedKeyError{"Duplicated key '" + key + "'"};
        }
        iterator++;

        if (!Utils::isAnyEqual(*iterator, std::string{":"})) {
            throw JsonParseUnexpectedChar{"Expected ':'"};
        }

        auto value = parsePart(++iterator, end, false);
        jsonResult->addToObjectKey(key, value);
        if (iterator == end) {
            break;
        }

        if (Utils::isAnyEqual(*iterator, std::string{"}"})) {
            iterator++;
            return jsonResult.release();
        }
        if (!Utils::isAnyEqual(*iterator, std::string{","})) {
            throw JsonParseUnexpectedChar{"Expected ','"};
        }

        iterator++;
    }

    throw JsonParseUnexpectedEof{"Expected end of array"};
}
