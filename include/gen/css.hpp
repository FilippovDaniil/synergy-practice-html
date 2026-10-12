#pragma once

#include <string>

#include "gen/organization.hpp"

namespace gen {

// styles.css: --font-main defaults to the official font; .font-N classes
// (N = 0..5) bind each of the 6 fonts for the switcher and comparison block.
std::string renderCss(const Organization& org);

}  // namespace gen
