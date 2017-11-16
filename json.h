#pragma once

#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 0

#include "json.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

#include <vector>

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
    static std::string GetError(int);
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

    Value GetValue();

private:
    RapidJsonValue::MemberIterator iterator_;
    RapidJsonDocument* document_;
};

class Value
{
private:
    explicit Value() {}

public:
    Value(RapidJsonValue& value, RapidJsonDocument& document) : value_(&value), document_(&document) {}
    virtual ~Value() {}

    // get/set value
    bool GetBool()
    {
        if (IsBool())
        {
            if (IsFalse()) return false;
            return true;
        }

        return false;
    }

    void SetBool(bool value)
    {
        value_->SetBool(value);
    }

    std::wstring GetString()
    {
        return std::wstring(value_->GetString());
    }

    void SetString(std::wstring& value)
    {
        value_->SetString(value, document_->GetAllocator());
    }

    int GetInt()
    {
        return value_->GetInt();
    }

    void SetInt(int value)
    {
        value_->SetInt(value);
    }

    unsigned int GetUInt()
    {
        return value_->GetUint();
    }

    void SetUInt(unsigned int value)
    {
        value_->SetUint(value);
    }

    int64_t GetInt64()
    {
        return value_->GetInt64();
    }

    void SetInt64(int64_t value)
    {
        value_->SetInt64(value);
    }

    uint64_t GetUInt64()
    {
        return value_->GetUint64();
    }

    void SetUInt64(uint64_t value)
    {
        value_->SetUint64(value);
    }

    double GetDouble()
    {
        return value_->GetDouble();
    }

    void SetDouble(double value)
    {
        value_->SetDouble(value);
    }

    // get member by name
    Value operator[](const std::wstring& name);

    // get array item by id
    Value operator[](int index);

    // Iteration
    ValueMemberIterator Begin();
    ValueMemberIterator End();

    // type check
    ValueType Type();
    bool IsNull()	{ return value_->IsNull(); }
    bool IsFalse()	{ return value_->IsFalse(); }
    bool IsTrue()	{ return value_->IsTrue(); }
    bool IsBool()	{ return value_->IsBool(); }
    bool IsObject()	{ return value_->IsObject(); }
    bool IsArray()	{ return value_->IsArray(); }
    bool IsNumber()	{ return value_->IsNumber(); }
    bool IsInt()	{ return value_->IsInt(); }
    bool IsUint()	{ return value_->IsUint(); }
    bool IsInt64()	{ return value_->IsInt64(); }
    bool IsUint64()	{ return value_->IsUint64(); }
    bool IsDouble()	{ return value_->IsDouble(); }
    bool IsString()	{ return value_->IsString(); }

    // array methods
    size_t Size() const { return value_->Size(); }
    size_t Capacity() const { return value_->Capacity(); }
    bool Empty() const { return value_->Empty(); }

    // make
    void ToNull() { value_->SetNull(); }
    void ToArray() { value_->SetArray(); }
    void ToObject() { value_->SetObject(); }

    Value AddBool(const std::wstring&, bool);
    Value AddString(const std::wstring&, const std::wstring&);
    Value AddInt(const std::wstring&, int);
    Value AddUInt(const std::wstring&, unsigned int);
    Value AddInt64(const std::wstring&, int64_t);
    Value AddUInt64(const std::wstring&, uint64_t);
    Value AddDouble(const std::wstring&, double);

    Value AddObject(const std::wstring&);
    Value AddArray(const std::wstring&);
    Value NewArrayItem();
    void MakeArrayItem(Value&);
    Value NewItem();

private:
    RapidJsonValue* value_;
    RapidJsonDocument* document_;
};

// json::Document
class Document : public Value
{
public:
    Document() : Value(document_, document_) {}
    virtual ~Document() {}
    void LoadFromString(const std::wstring&);
    void LoadFromString(const std::string&);
    void LoadFromStream(const std::vector<unsigned char>&);
    void LoadFromFile(const std::string&);
    bool HasErrors();
    std::string LastError();
    void WriteToString(std::wstring&);
    void WriteToString(std::string&);
    void WriteToStream(std::vector<unsigned char>&);
    void SaveToFile(const std::string& dest);
    Value operator[](const std::wstring& name);
    Value AddObject(const std::wstring&);
    Value AddArray(const std::wstring&);
    void Clone(Document&);

protected:
    RapidJsonDocument document_;
};

} // json
} // wave
} // gravio
