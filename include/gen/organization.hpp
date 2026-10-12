#pragma once

#include <string>
#include <vector>

namespace gen {

struct Font {
    std::string name;               // CSS font-family name, e.g. "Raleway"
    std::string googleFontsFamily;  // Google Fonts spec, e.g. "Raleway:wght@400;700"
    std::string category;           // typeface class, e.g. "геометрический гротеск"
    std::string note;               // why the font was chosen / remark
};

struct Organization {
    std::string fullName;
    std::string shortName;
    std::string legalForm;
    std::string description;
    std::vector<std::string> activities;
    std::string address;
    std::string website;
    std::vector<std::string> phones;  // optional
    Font officialFont;
    std::vector<Font> alternativeFonts;  // exactly 5
};

inline constexpr std::size_t kAlternativeFontCount = 5;

// Official font first, then the alternatives (6 fonts in total).
std::vector<Font> allFonts(const Organization& org);

// CSS font-family stack with a generic fallback derived from the category,
// e.g. "'PT Serif', serif".
std::string fontStack(const Font& font);

// Parses organization data from a JSON string. Throws std::runtime_error with
// a readable message on malformed JSON or missing/invalid fields.
Organization parseOrganization(const std::string& json);

// Reads the file at `path` and parses it. Throws std::runtime_error.
Organization loadOrganization(const std::string& path);

}  // namespace gen
