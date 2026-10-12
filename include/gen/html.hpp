#pragma once

#include <string>
#include <string_view>

#include "gen/organization.hpp"

namespace gen {

// Escapes & < > " ' for safe use in HTML text and attribute values.
// Other bytes (including UTF-8 Cyrillic) are copied unchanged.
std::string escapeHtml(std::string_view text);

// Google Fonts css2 URL with all 6 families (unescaped).
std::string googleFontsUrl(const Organization& org);

// Full index.html document (UTF-8).
std::string renderHtml(const Organization& org);

}  // namespace gen
