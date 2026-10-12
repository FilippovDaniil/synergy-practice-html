#include "gen/organization.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace gen {
namespace {

using nlohmann::json;

std::string requireString(const json& obj, const std::string& key, const std::string& ctx) {
    if (!obj.contains(key)) {
        throw std::runtime_error("missing required field '" + ctx + key + "'");
    }
    const json& v = obj.at(key);
    if (!v.is_string()) {
        throw std::runtime_error("field '" + ctx + key + "' must be a string");
    }
    std::string s = v.get<std::string>();
    if (s.empty()) {
        throw std::runtime_error("field '" + ctx + key + "' must not be empty");
    }
    return s;
}

std::string optionalString(const json& obj, const std::string& key, const std::string& ctx) {
    if (!obj.contains(key)) return {};
    const json& v = obj.at(key);
    if (!v.is_string()) {
        throw std::runtime_error("field '" + ctx + key + "' must be a string");
    }
    return v.get<std::string>();
}

Font parseFont(const json& obj, const std::string& ctx, const char* noteKey) {
    if (!obj.is_object()) {
        throw std::runtime_error("field '" + ctx + "' must be an object");
    }
    const std::string prefix = ctx + ".";
    Font f;
    f.name = requireString(obj, "name", prefix);
    f.googleFontsFamily = requireString(obj, "googleFontsFamily", prefix);
    f.category = requireString(obj, "category", prefix);
    f.note = optionalString(obj, noteKey, prefix);
    return f;
}

}  // namespace

Organization parseOrganization(const std::string& text) {
    json root;
    try {
        root = json::parse(text);
    } catch (const json::parse_error& e) {
        throw std::runtime_error(std::string("invalid JSON: ") + e.what());
    }
    if (!root.is_object()) {
        throw std::runtime_error("invalid JSON: root must be an object");
    }

    Organization org;
    org.fullName = requireString(root, "fullName", "");
    org.shortName = requireString(root, "shortName", "");
    org.legalForm = requireString(root, "legalForm", "");
    org.description = requireString(root, "description", "");
    org.address = requireString(root, "address", "");
    org.website = requireString(root, "website", "");

    if (!root.contains("activities")) {
        throw std::runtime_error("missing required field 'activities'");
    }
    const json& acts = root.at("activities");
    if (!acts.is_array() || acts.empty()) {
        throw std::runtime_error("field 'activities' must be a non-empty array");
    }
    for (std::size_t i = 0; i < acts.size(); ++i) {
        if (!acts[i].is_string() || acts[i].get<std::string>().empty()) {
            throw std::runtime_error("field 'activities[" + std::to_string(i) +
                                     "]' must be a non-empty string");
        }
        org.activities.push_back(acts[i].get<std::string>());
    }

    if (!root.contains("officialFont")) {
        throw std::runtime_error("missing required field 'officialFont'");
    }
    org.officialFont = parseFont(root.at("officialFont"), "officialFont", "note");

    if (!root.contains("alternativeFonts")) {
        throw std::runtime_error("missing required field 'alternativeFonts'");
    }
    const json& alts = root.at("alternativeFonts");
    if (!alts.is_array()) {
        throw std::runtime_error("field 'alternativeFonts' must be an array");
    }
    if (alts.size() != kAlternativeFontCount) {
        throw std::runtime_error("field 'alternativeFonts' must contain exactly " +
                                 std::to_string(kAlternativeFontCount) + " fonts, got " +
                                 std::to_string(alts.size()));
    }
    for (std::size_t i = 0; i < alts.size(); ++i) {
        org.alternativeFonts.push_back(
            parseFont(alts[i], "alternativeFonts[" + std::to_string(i) + "]", "why"));
    }
    return org;
}

Organization loadOrganization(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("cannot open file '" + path + "'");
    }
    std::ostringstream buf;
    buf << in.rdbuf();
    try {
        return parseOrganization(buf.str());
    } catch (const std::runtime_error& e) {
        throw std::runtime_error(path + ": " + e.what());
    }
}

}  // namespace gen
