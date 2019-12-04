#include <cstring>
#include <JsonException.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "Utils.hpp"

bool Utils::isCharSpace(char c)
{
    return boost::is_any_of(" \t\r\n")(c);
}

bool Utils::isCharQuote(char c)
{
    return boost::is_any_of("\'\"")(c);
}

bool Utils::isCharNumber(char c)
{
    return boost::is_any_of("0123456789-e.")(c);
}

bool Utils::isCharEscaping(char c)
{
    return c == '\\';
}

double Utils::stringToNumber(const std::string &string)
{
    return boost::lexical_cast<double>(string);
}

bool Utils::isCharSugar(char c)
{
    return boost::is_any_of(",:[]{}")(c);
}