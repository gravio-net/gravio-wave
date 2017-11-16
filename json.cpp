#include "json.h"

#include <QFile>

using namespace gravio::wave::json;

rapidjson::CrtAllocator gAllocator;

std::string Error::GetError(int code)
{
	switch(code)
	{
        case rapidjson::ParseErrorCode::kParseErrorNone: return std::string("No error.");
        case rapidjson::ParseErrorCode::kParseErrorDocumentEmpty: return std::string("The document is empty.");
        case rapidjson::ParseErrorCode::kParseErrorDocumentRootNotSingular: return std::string("The document root must not follow by other values.");
        case rapidjson::ParseErrorCode::kParseErrorValueInvalid: return std::string("Invalid value.");
        case rapidjson::ParseErrorCode::kParseErrorObjectMissName: return std::string("Missing a name for object member.");
        case rapidjson::ParseErrorCode::kParseErrorObjectMissColon: return std::string("Missing a colon after a name of object member.");
        case rapidjson::ParseErrorCode::kParseErrorObjectMissCommaOrCurlyBracket: return std::string("Missing a comma or '}' after an object member.");
        case rapidjson::ParseErrorCode::kParseErrorArrayMissCommaOrSquareBracket: return std::string("Missing a comma or ']' after an array element.");
        case rapidjson::ParseErrorCode::kParseErrorStringUnicodeEscapeInvalidHex: return std::string("Incorrect hex digit after \\u escape in string.");
        case rapidjson::ParseErrorCode::kParseErrorStringUnicodeSurrogateInvalid: return std::string("The surrogate pair in string is invalid.");
        case rapidjson::ParseErrorCode::kParseErrorStringEscapeInvalid: return std::string("Invalid escape character in string.");
        case rapidjson::ParseErrorCode::kParseErrorStringMissQuotationMark: return std::string("Missing a closing quotation mark in string.");
        case rapidjson::ParseErrorCode::kParseErrorStringInvalidEncoding: return std::string("Invalid encoding in string.");
        case rapidjson::ParseErrorCode::kParseErrorNumberTooBig: return std::string("Number too big to be stored in double.");
        case rapidjson::ParseErrorCode::kParseErrorNumberMissFraction: return std::string("Miss fraction part in number.");
        case rapidjson::ParseErrorCode::kParseErrorNumberMissExponent: return std::string("Miss exponent in number.");
        case rapidjson::ParseErrorCode::kParseErrorTermination: return std::string("Parsing was terminated.");
        case rapidjson::ParseErrorCode::kParseErrorUnspecificSyntaxError: return std::string("Unspecific syntax error.");
        default: return std::string("No error.");
	}
}

//
// JsonValue
//
ValueType Value::Type()
{
    if (IsBool()) return ValueType::Bool;
    else if(IsString()) return ValueType::String;
    else if (IsInt()) return ValueType::Int;
    else if (IsUint()) return ValueType::Uint;
    else if (IsInt64()) return ValueType::Int64;
    else if (IsUint64()) return ValueType::Uint64;
    else if (IsDouble()) return ValueType::Double;
    else if (IsObject()) return ValueType::Object;
    else if (IsArray()) return ValueType::Array;
    return ValueType::Null;
}

Value Value::operator[](const std::wstring& name)
{
    return Value(value_->operator[](name), *document_);
}

Value Value::operator[](int index)
{
    return Value(value_->operator[](index), *document_);
}

Value Value::AddBool(const std::wstring& name, bool value)
{
    RapidJsonValue lName, lValue; lName.SetString(name, document_->GetAllocator());
    if (!IsObject()) value_->SetObject();

    lValue.SetBool(value);

    return Value(value_->AddMember(lName, lValue, document_->GetAllocator()), *document_);
}

Value Value::AddString(const std::wstring& name, const std::wstring& value)
{
    RapidJsonValue lName, lValue; lName.SetString(name, document_->GetAllocator());
    if (!IsObject()) value_->SetObject();

    lValue.SetString(value, document_->GetAllocator());

    return Value(value_->AddMember(lName, lValue, document_->GetAllocator()), *document_);
}

Value Value::AddInt(const std::wstring& name, int value)
{
    RapidJsonValue lName, lValue; lName.SetString(name, document_->GetAllocator());
    if (!IsObject()) value_->SetObject();

    lValue.SetInt(value);

    return Value(value_->AddMember(lName, lValue, document_->GetAllocator()), *document_);
}

Value Value::AddUInt(const std::wstring& name, unsigned int value)
{
    RapidJsonValue lName, lValue; lName.SetString(name, document_->GetAllocator());
    if (!IsObject()) value_->SetObject();

    lValue.SetUint(value);

    return Value(value_->AddMember(lName, lValue, document_->GetAllocator()), *document_);
}

Value Value::AddInt64(const std::wstring& name, int64_t value)
{
    RapidJsonValue lName, lValue; lName.SetString(name, document_->GetAllocator());
    if (!IsObject()) value_->SetObject();

    lValue.SetInt64(value);

    return Value(value_->AddMember(lName, lValue, document_->GetAllocator()), *document_);
}

Value Value::AddUInt64(const std::wstring& name, uint64_t value)
{
    RapidJsonValue lName, lValue; lName.SetString(name, document_->GetAllocator());
    if (!IsObject()) value_->SetObject();

    lValue.SetUint64(value);

    return Value(value_->AddMember(lName, lValue, document_->GetAllocator()), *document_);
}

Value Value::AddDouble(const std::wstring& name, double value)
{
    RapidJsonValue lName, lValue; lName.SetString(name, document_->GetAllocator());
    if (!IsObject()) value_->SetObject();

    lValue.SetDouble(value);

    return Value(value_->AddMember(lName, lValue, document_->GetAllocator()), *document_);
}

Value Value::AddObject(const std::wstring& name)
{
	RapidJsonValue lName, lValue;
	lName.SetString(name, document_->GetAllocator()); lValue.SetObject();
	value_->AddMember(lName, lValue, document_->GetAllocator());

    return Value(value_->FindMember(name)->value, *document_);
}

Value Value::AddArray(const std::wstring& name)
{
	RapidJsonValue lName, lValue;
	lName.SetString(name, document_->GetAllocator());
	lValue.SetArray();
	value_->AddMember(lName, lValue, document_->GetAllocator());

    return Value(value_->FindMember(name)->value, *document_);
}

Value Value::NewArrayItem()
{
	RapidJsonValue lValue;
	if (!IsArray()) value_->SetArray();
	value_->PushBack(lValue, document_->GetAllocator());

    return Value(value_->operator[](value_->Size()-1), *document_);
}

void Value::MakeArrayItem(Value& value)
{
}

Value Value::NewItem()
{
	RapidJsonValue lName(L"__raw__"), lValue;
	value_->AddMember(lName, lValue, document_->GetAllocator());
	
	RapidJsonValue::MemberIterator lMember = value_->FindMember(L"__raw__");
	lMember->name.SetNull();

    return Value(lMember->value, *document_);
}

// Iteration
ValueMemberIterator Value::Begin()
{
    return ValueMemberIterator(value_->MemberBegin(), *document_);
}

ValueMemberIterator Value::End()
{
    return ValueMemberIterator(value_->MemberEnd(), *document_);
}

//
// Document
//
void Document::LoadFromString(const std::wstring& source)
{
	if(document_.Parse(source).HasParseError())
	{
        throw std::string("[JsonDocument/LoadFromString/Error]: ") + Error::GetError(document_.GetParseError());
	}
}

void Document::LoadFromString(const std::string& source)
{
	if(document_.Parse<0, rapidjson::UTF8<> >(source).HasParseError())
	{
        throw std::string("[JsonDocument/LoadFromString/Error]: ") + Error::GetError(document_.GetParseError());
	}
}

void Document::LoadFromStream(const std::vector<unsigned char>& source)
{
	if(document_.Parse<0, rapidjson::UTF8<> >((char*)&source[0], source.size()).HasParseError())
	{
        throw std::string("[JsonDocument/LoadFromStream/Error]: ") + Error::GetError(document_.GetParseError());
	}
}

void Document::LoadFromFile(const std::string& source)
{
    QFile lFile(QString(source.c_str()));
    if (!lFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QByteArray lData = lFile.readAll();
    QString lText(lData);

    lFile.close();
	
    if(document_.Parse(lText.toStdWString()).HasParseError())
	{
        throw std::string("[JsonDocument/LoadFromFile/Error]: ") + Error::GetError(document_.GetParseError());
	}
}

void Document::SaveToFile(const std::string& dest)
{
    // WriteToString
}

bool Document::HasErrors()
{
	return document_.HasParseError();
}

std::string Document::LastError()
{
    return Error::GetError(document_.GetParseError());
}

void Document::WriteToString(std::wstring& stream)
{
	RapidJsonStringBuffer lBuffer;
	RapidJsonWriter lWriter(lBuffer);
	document_.Accept(lWriter);
	stream.insert(0, lBuffer.GetString());
}

void Document::WriteToString(std::string& stream)
{
	RapidJsonUTF8StringBuffer lBuffer;
	RapidJsonOutputStream lOutStream(lBuffer, false);
	RapidJsonUTF8PrettyWriter lWriter(lOutStream);
	document_.Accept(lWriter);
	stream.insert(0, lBuffer.GetString());
}

void Document::WriteToStream(std::vector<unsigned char>& stream)
{
	RapidJsonUTF8StringBuffer lBuffer;
	RapidJsonOutputStream lOutStream(lBuffer, false);
	RapidJsonUTF8PrettyWriter lWriter(lOutStream);
	document_.Accept(lWriter);
	stream.resize(lBuffer.GetSize());
	memcpy(&stream[0], lBuffer.GetString(), lBuffer.GetSize());
}

Value Document::operator[](const std::wstring& name)
{
    return Value(document_[name], document_);
}

Value Document::AddObject(const std::wstring& name)
{
    return Value::AddObject(name);
}

Value Document::AddArray(const std::wstring& name)
{
    return Value::AddArray(name);
}

void Document::Clone(Document& other)
{
    other.document_.CopyFrom(document_, other.document_.GetAllocator());
}
