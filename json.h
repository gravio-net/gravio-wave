#pragma once

#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 0

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

#include "exception.h"

#include <vector>
#include <locale>

#include <QString>

namespace gravio {
namespace wave {
namespace json {

enum ValueType
{
    Null,
    Bool,
    Object,
    Array,
    Int,
    Uint,
    Int64,
    Uint64,
    Double,
    String,
    DateTime
};

template <typename Type, bool arrayed = true>
class RawPointerGuard
{
public:
    RawPointerGuard(Type * pointer) : pointer_(pointer) {}
    RawPointerGuard(const RawPointerGuard<Type>& right) { pointer_ = right.pointer_; }
            RawPointerGuard<Type>& operator = (const RawPointerGuard<Type>& right) { pointer_ = right.pointer_; return *this; }

    ~RawPointerGuard() { if (!pointer_) return; if (arrayed) { delete[] pointer_; } else { delete pointer_; } }
private:
    Type* pointer_;
};

typedef rapidjson::GenericDocument<rapidjson::UTF16<> > RapidJsonDocument;
typedef rapidjson::GenericValue<rapidjson::UTF16<> > RapidJsonValue;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF16<> > RapidJsonStringBuffer;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF8<> > RapidJsonUTF8StringBuffer;
typedef rapidjson::PrettyWriter<RapidJsonStringBuffer, rapidjson::UTF16<>, rapidjson::UTF16<> > RapidJsonWriter;
typedef rapidjson::EncodedOutputStream<rapidjson::UTF8<>, RapidJsonUTF8StringBuffer> RapidJsonOutputStream;
typedef rapidjson::PrettyWriter<RapidJsonOutputStream, rapidjson::UTF16<>, rapidjson::UTF8<> > RapidJsonUTF8PrettyWriter;

//
// implementation of "::Json"
//
class Error
{
public:
    static std::string getError(int);
};

class Value;
class ValueMemberIterator
{
private:
    explicit ValueMemberIterator() : document_(0) {}

public:
    ValueMemberIterator(RapidJsonValue::MemberIterator iterator, RapidJsonDocument& document) : iterator_(iterator), document_(&document) {}

    ValueMemberIterator& operator++ (int) { return operator++(); }
    ValueMemberIterator& operator-- (int) { return operator--(); }
    ValueMemberIterator& operator++ ()
    {
        iterator_++;
        return *this;
    }

    ValueMemberIterator& operator-- ()
    {
        iterator_--;
        return *this;
    }

    bool operator != (const ValueMemberIterator& iterator) { return iterator_ != ((ValueMemberIterator&)iterator).iterator_; }
    bool operator == (const ValueMemberIterator& iterator) { return iterator_ != ((ValueMemberIterator&)iterator).iterator_; }

    Value getValue();

private:
    RapidJsonValue::MemberIterator iterator_;
    RapidJsonDocument* document_;
};

class Document;
class Value
{
public:
    explicit Value() { value_ = 0; document_ = 0; }

public:
    Value(RapidJsonValue& value, RapidJsonDocument& document) : value_(&value), document_(&document) {}
    virtual ~Value() {}

    // get/set value
    bool getBool()
    {
        if (isBool())
        {
            if (isFalse()) return false;
            return true;
        }

        return false;
    }

    void setBool(bool value)
    {
        checkReference();
        value_->SetBool(value);
    }

    std::wstring getString()
    {
        checkReference();
        return std::wstring(value_->GetString());
    }

    std::string getAString()
    {
        checkReference();

        QString lString = QString::fromStdWString(value_->GetString()); // TODO: tricky
        return lString.toStdString();
    }

    QString getQString()
    {
        checkReference();
        return QString::fromStdWString(getString().c_str());
    }

    void setString(std::wstring& value)
    {
        checkReference();
        value_->SetString(value, document_->GetAllocator());
    }

    int getInt()
    {
        checkReference();
        return value_->GetInt();
    }

    void setInt(int value)
    {
        checkReference();
        value_->SetInt(value);
    }

    unsigned int getUInt()
    {
        checkReference();
        return value_->GetUint();
    }

    void setUInt(unsigned int value)
    {
        checkReference();
        value_->SetUint(value);
    }

    int64_t getInt64()
    {
        checkReference();
        return value_->GetInt64();
    }

    void setInt64(int64_t value)
    {
        checkReference();
        value_->SetInt64(value);
    }

    uint64_t getUInt64()
    {
        checkReference();
        return value_->GetUint64();
    }

    void setUInt64(uint64_t value)
    {
        checkReference();
        value_->SetUint64(value);
    }

    double getDouble()
    {
        checkReference();
        return value_->GetDouble();
    }

    void setDouble(double value)
    {
        checkReference();
        value_->SetDouble(value);
    }

    // get member by name
    Value operator[](const std::wstring& name);

    // get array item by id
    Value operator[](size_t index);

    // Iteration
    ValueMemberIterator begin();
    ValueMemberIterator end();

    // Find value by name
    bool find(const std::wstring&, Value&);

    // type check
    ValueType type();
    bool isNull()	const { return value_ ? value_->IsNull() : true; }
    bool isFalse()	const { return value_ ? value_->IsFalse() : false; }
    bool isTrue()	const { return value_ ? value_->IsTrue() : false; }
    bool isBool()	const { return value_ ? value_->IsBool() : false; }
    bool isObject()	const { return value_ ? value_->IsObject() : false; }
    bool isArray()	const { return value_ ? value_->IsArray() : false; }
    bool isNumber()	const { return value_ ? value_->IsNumber() : false; }
    bool isInt()	const { return value_ ? value_->IsInt() : false; }
    bool isUint()	const { return value_ ? value_->IsUint() : false; }
    bool isInt64()	const { return value_ ? value_->IsInt64() : false; }
    bool isUint64()	const { return value_ ? value_->IsUint64() : false; }
    bool isDouble()	const { return value_ ? value_->IsDouble() : false; }
    bool isString()	const { return value_ ? value_->IsString() : false; }

    // array methods
    size_t size() const { checkReference(); return value_->Size(); }
    size_t capacity() const { checkReference(); return value_->Capacity(); }
    bool empty() const { checkReference(); return value_->Empty(); }

    // make
    void toNull() { checkReference(); value_->SetNull(); }
    void toArray() { checkReference(); value_->SetArray(); }
    void toObject() { checkReference(); value_->SetObject(); }

    Value addBool(const std::wstring&, bool);
    Value addString(const std::wstring&, const std::wstring&);
    Value addInt(const std::wstring&, int);
    Value addUInt(const std::wstring&, unsigned int);
    Value addInt64(const std::wstring&, int64_t);
    Value addUInt64(const std::wstring&, uint64_t);
    Value addDouble(const std::wstring&, double);

    Value addObject(const std::wstring&);
    Value addArray(const std::wstring&);
    Value newArrayItem();
    void makeArrayItem(Value&);
    Value newItem();

    std::string toString();
    std::wstring toWString();

    virtual void writeToString(std::wstring&);
    virtual void writeToString(std::string&);
    virtual void writeToStream(std::vector<unsigned char>&);

    virtual void clone(Document&);

private:
    void checkReference() const
    {
        if (!value_) NULL_REFERENCE_EXCEPTION();
    }

    void assign(RapidJsonValue* value, RapidJsonDocument* document)
    {
        value_ = value;
        document_ = document;
    }

private:
    RapidJsonValue* value_;
    RapidJsonDocument* document_;
};

// json::Document
class Document : public Value
{
    friend class Value;

public:
    Document() : Value(document_, document_) {}
    virtual ~Document() {}
    void loadFromString(const std::wstring&);
    void loadFromString(const std::string&);
    void loadFromStream(const std::vector<unsigned char>&);
    void loadFromFile(const std::string&);
    bool hasErrors();
    std::string lastError();
    void writeToString(std::wstring&);
    void writeToString(std::string&);
    void writeToStream(std::vector<unsigned char>&);
    void saveToFile(const std::string& dest);
    Value operator[](const std::wstring& name);
    Value addObject(const std::wstring&);
    Value addArray(const std::wstring&);
    void clone(Document&);

protected:
    RapidJsonDocument document_;
};

} // json
} // wave
} // gravio
