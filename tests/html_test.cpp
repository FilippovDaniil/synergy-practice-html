#include <string>

#include <gtest/gtest.h>

#include "gen/html.hpp"
#include "test_data.hpp"

using gen::escapeHtml;
using gen::renderHtml;

namespace {

std::size_t countOccurrences(const std::string& haystack, const std::string& needle) {
    std::size_t count = 0;
    for (std::size_t pos = haystack.find(needle); pos != std::string::npos;
         pos = haystack.find(needle, pos + needle.size())) {
        ++count;
    }
    return count;
}

}  // namespace

TEST(EscapeHtml, EscapesSpecialCharacters) {
    EXPECT_EQ(escapeHtml("<a href=\"x\">Tom & Jerry's</a>"),
              "&lt;a href=&quot;x&quot;&gt;Tom &amp; Jerry&#39;s&lt;/a&gt;");
}

TEST(EscapeHtml, KeepsCyrillicUnchanged) {
    const std::string text = "Университет «Синергия» — ёжик";
    EXPECT_EQ(escapeHtml(text), text);
}

TEST(EscapeHtml, EmptyString) {
    EXPECT_EQ(escapeHtml(""), "");
}

TEST(RenderHtml, H1ContainsFullName) {
    const auto org = testdata::validOrganization();
    const std::string html = renderHtml(org);
    EXPECT_NE(html.find("<h1>" + escapeHtml(org.fullName) + "</h1>"), std::string::npos);
}

TEST(RenderHtml, DocumentBasics) {
    const std::string html = renderHtml(testdata::validOrganization());
    EXPECT_EQ(html.rfind("<!DOCTYPE html>", 0), 0u);
    EXPECT_NE(html.find("<html lang=\"ru\">"), std::string::npos);
    EXPECT_NE(html.find("<meta charset=\"UTF-8\">"), std::string::npos);
    EXPECT_NE(html.find("name=\"viewport\""), std::string::npos);
    EXPECT_NE(html.find("href=\"styles.css\""), std::string::npos);
}

TEST(RenderHtml, EscapesJsonStrings) {
    const std::string html = renderHtml(testdata::validOrganization());
    EXPECT_NE(html.find("Описание &lt;организации&gt; &amp; &quot;кавычки&quot;"),
              std::string::npos);
    EXPECT_EQ(html.find("<организации>"), std::string::npos);
}

TEST(RenderHtml, ContainsAllSections) {
    const auto org = testdata::validOrganization();
    const std::string html = renderHtml(org);
    for (const char* id : {"about", "legal-form", "activities", "address", "contacts", "fonts"}) {
        EXPECT_NE(html.find("id=\"" + std::string(id) + "\""), std::string::npos) << id;
    }
    EXPECT_NE(html.find("<li>Высшее образование</li>"), std::string::npos);
    EXPECT_NE(html.find("href=\"https://example.org\""), std::string::npos);
}

TEST(RenderHtml, NonHttpWebsiteIsNotALink) {
    auto org = testdata::validOrganization();
    org.website = "javascript:alert(1)";
    const std::string html = renderHtml(org);
    EXPECT_EQ(html.find("href=\"javascript:"), std::string::npos);
}

TEST(RenderHtml, GoogleFontsLinkHasAllSixFamilies) {
    const auto org = testdata::validOrganization();
    const std::string html = renderHtml(org);
    const std::size_t pos = html.find("https://fonts.googleapis.com/css2?");
    ASSERT_NE(pos, std::string::npos);
    EXPECT_EQ(countOccurrences(html, "https://fonts.googleapis.com/css2?"), 1u);
    const std::string link = html.substr(pos, html.find('"', pos) - pos);
    EXPECT_EQ(countOccurrences(link, "family="), 6u);
    for (const auto& f : gen::allFonts(org)) {
        EXPECT_NE(link.find("family=" + escapeHtml(f.googleFontsFamily)), std::string::npos)
            << f.name;
    }
    EXPECT_NE(link.find("cyrillic"), std::string::npos);
}

TEST(RenderHtml, SixSwitcherButtons) {
    const auto org = testdata::validOrganization();
    const std::string html = renderHtml(org);
    EXPECT_EQ(countOccurrences(html, "<button type=\"button\" class=\"font-btn"), 6u);
    EXPECT_EQ(countOccurrences(html, "aria-pressed=\"true\""), 1u);
    EXPECT_NE(html.find("data-font=\"&#39;Raleway&#39;, sans-serif\""), std::string::npos);
    EXPECT_NE(html.find("data-font=\"&#39;PT Serif&#39;, serif\""), std::string::npos);
    EXPECT_NE(html.find("data-font=\"&#39;JetBrains Mono&#39;, monospace\""), std::string::npos);
}

TEST(RenderHtml, SixComparisonSamples) {
    const auto org = testdata::validOrganization();
    const std::string html = renderHtml(org);
    EXPECT_EQ(countOccurrences(html, "<figure class=\"sample "), 6u);
    for (const auto& f : gen::allFonts(org)) {
        EXPECT_NE(html.find("<strong>" + escapeHtml(f.name) + "</strong> — " +
                            escapeHtml(f.category)),
                  std::string::npos)
            << f.name;
    }
}

TEST(RenderHtml, SwitcherScriptIsShort) {
    const std::string html = renderHtml(testdata::validOrganization());
    const std::size_t start = html.find("<script>");
    const std::size_t end = html.find("</script>");
    ASSERT_NE(start, std::string::npos);
    ASSERT_NE(end, std::string::npos);
    const std::string js = html.substr(start, end - start);
    EXPECT_LE(countOccurrences(js, "\n"), 20u);
}
