#pragma once

#include <stdexcept>

class JsonException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class JsonUnexpectedType : public JsonException
{
public:
    using JsonException::JsonException;
};

class JsonUnexpectedKey : public JsonException
{
public:
    using JsonException::JsonException;
};

class JsonParseException : public JsonException
{
public:
    using JsonException::JsonException;
};

class JsonParseInternalError : public JsonParseException
{
public:
    using JsonParseException::JsonParseException;
};

class JsonParseUnexpectedEof : public JsonParseException
{
public:
    using JsonParseException::JsonParseException;
};

class JsonParseUnexpectedChar : public JsonParseException
{
public:
    using JsonParseException::JsonParseException;
};

class JsonParseCannotParseNumber : public JsonParseException
{
public:
    using JsonParseException::JsonParseException;
};

class JsonParseFileException: public JsonParseException
{
public:
    using JsonParseException::JsonParseException;
};

class JsonParseDuplicatedKeyError: public JsonParseException
{
public:
    using JsonParseException::JsonParseException;
};