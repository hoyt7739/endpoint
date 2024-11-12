#include "commun_command.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

#define READ_VALUE(data, member, type)                              \
    do {                                                            \
        auto iter = object.FindMember(#member);                     \
        if (object.MemberEnd() != iter && iter->value.Is##type()) { \
            data.member = iter->value.Get##type();                  \
        }                                                           \
    } while (false)

#define WRITE_VALUE(data, member, type) \
    do {                                \
        writer.Key(#member);            \
        writer.type(data.member);       \
    } while (false)

commun_interact::commun_interact(const std::string& json) {
    from_json(json);
}

void commun_interact::from_json(const std::string& json) {
    entries.clear();

    if (json.empty()) {
        return;
    }

    Document doc;
    if (doc.Parse(json).HasParseError() || !doc.IsArray()) {
        return;
    }

    for (auto& object : doc.GetArray()) {
        interact_entry entry;
        READ_VALUE(entry, index, Int);
        READ_VALUE(entry, name, String);
        READ_VALUE(entry, desc, String);

        auto items_iter = object.FindMember("items");
        if (object.MemberEnd() != items_iter && items_iter->value.IsArray()) {
            for (auto& object : items_iter->value.GetArray()) {
                interact_item item;
                READ_VALUE(item, time, Int64);
                READ_VALUE(item, key, String);
                READ_VALUE(item, value, String);
                entry.items.emplace_back(item);
            }
        }

        entries.emplace_back(entry);
    }
}

std::string commun_interact::to_json() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartArray();
    for (auto& entry : entries) {
        writer.StartObject();
        WRITE_VALUE(entry, index, Int);
        WRITE_VALUE(entry, name, String);
        WRITE_VALUE(entry, desc, String);

        writer.Key("items");
        writer.StartArray();
        for (auto& item : entry.items) {
            writer.StartObject();
            WRITE_VALUE(item, time, Int64);
            WRITE_VALUE(item, key, String);
            WRITE_VALUE(item, value, String);
            writer.EndObject();
        }
        writer.EndArray();

        writer.EndObject();
    }
    writer.EndArray();

    return sb.GetString();
}
