#include <string>

#include <gtest/gtest.h>

#include "gen/css.hpp"
#include "gen/html.hpp"
#include "test_data.hpp"

using gen::renderCss;

TEST(RenderCss, FontMainDefaultsToRaleway) {
    const std::string css = renderCss(testdata::validOrganization());
    EXPECT_NE(css.find("--font-main: 'Raleway', sans-serif;"), std::string::npos);
    EXPECT_NE(css.find("font-family: var(--font-main);"), std::string::npos);
}

TEST(RenderCss, MentionsAllSixFamilies) {
    const auto org = testdata::validOrganization();
    const std::string css = renderCss(org);
    const std::string html = gen::renderHtml(org);
    const auto fonts = gen::allFonts(org);
    ASSERT_EQ(fonts.size(), 6u);
    for (std::size_t i = 0; i < fonts.size(); ++i) {
        const std::string rule =
            ".font-" + std::to_string(i) + " { font-family: " + gen::fontStack(fonts[i]) + ";";
        EXPECT_NE(css.find(rule), std::string::npos) << rule;
        EXPECT_NE(html.find(gen::escapeHtml(fonts[i].name)), std::string::npos) << fonts[i].name;
    }
}

TEST(RenderCss, FontStackFallbacks) {
    gen::Font f{"X", "X", "антиква", ""};
    EXPECT_EQ(gen::fontStack(f), "'X', serif");
    f.category = "моноширинный";
    EXPECT_EQ(gen::fontStack(f), "'X', monospace");
    f.category = "гуманистический гротеск";
    EXPECT_EQ(gen::fontStack(f), "'X', sans-serif");
}

TEST(RenderCss, HasResponsiveRules) {
    const std::string css = renderCss(testdata::validOrganization());
    EXPECT_NE(css.find("@media (max-width: 600px)"), std::string::npos);
}
