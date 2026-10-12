#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "gen/organization.hpp"

namespace testdata {

inline nlohmann::json validJson() {
    return nlohmann::json::parse(R"({
      "fullName": "Тестовый университет «Пример»",
      "shortName": "ТУ «Пример»",
      "legalForm": "АНО ВО",
      "description": "Описание <организации> & \"кавычки\"",
      "activities": ["Высшее образование", "Дополнительное образование"],
      "address": "г. Москва, ул. Тестовая, д. 1",
      "website": "https://example.org",
      "officialFont": {"name": "Raleway", "googleFontsFamily": "Raleway:wght@400;700",
                       "category": "геометрический гротеск", "note": "официальный"},
      "alternativeFonts": [
        {"name": "Roboto", "googleFontsFamily": "Roboto:wght@400;700", "category": "неогротеск", "why": "a"},
        {"name": "PT Serif", "googleFontsFamily": "PT+Serif:wght@400;700", "category": "антиква", "why": "b"},
        {"name": "Montserrat", "googleFontsFamily": "Montserrat:wght@400;700", "category": "геометрический гротеск", "why": "c"},
        {"name": "Open Sans", "googleFontsFamily": "Open+Sans:wght@400;700", "category": "гуманистический гротеск", "why": "d"},
        {"name": "JetBrains Mono", "googleFontsFamily": "JetBrains+Mono:wght@400;700", "category": "моноширинный", "why": "e"}
      ]
    })");
}

inline gen::Organization validOrganization() {
    return gen::parseOrganization(validJson().dump());
}

}  // namespace testdata
