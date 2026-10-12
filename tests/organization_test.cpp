#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "gen/organization.hpp"
#include "test_data.hpp"

using gen::parseOrganization;

namespace {

// Asserts that parsing throws std::runtime_error whose message contains `fragment`.
void expectError(const std::string& json, const std::string& fragment) {
    try {
        parseOrganization(json);
        FAIL() << "expected exception containing: " << fragment;
    } catch (const std::runtime_error& e) {
        EXPECT_NE(std::string(e.what()).find(fragment), std::string::npos)
            << "message was: " << e.what();
    }
}

}  // namespace

TEST(Organization, ParsesValidJson) {
    const auto org = testdata::validOrganization();
    EXPECT_EQ(org.fullName, "Тестовый университет «Пример»");
    EXPECT_EQ(org.activities.size(), 2u);
    EXPECT_EQ(org.officialFont.name, "Raleway");
    EXPECT_EQ(org.officialFont.note, "официальный");
    ASSERT_EQ(org.alternativeFonts.size(), 5u);
    EXPECT_EQ(org.alternativeFonts[4].name, "JetBrains Mono");
    EXPECT_EQ(org.alternativeFonts[1].note, "b");
}

TEST(Organization, BrokenJsonThrows) {
    expectError("{ \"fullName\": ", "invalid JSON");
}

TEST(Organization, RootNotObjectThrows) {
    expectError("[1, 2]", "root must be an object");
}

TEST(Organization, MissingFullNameThrows) {
    auto j = testdata::validJson();
    j.erase("fullName");
    expectError(j.dump(), "missing required field 'fullName'");
}

TEST(Organization, EmptyStringFieldThrows) {
    auto j = testdata::validJson();
    j["website"] = "";
    expectError(j.dump(), "'website' must not be empty");
}

TEST(Organization, WrongTypeThrows) {
    auto j = testdata::validJson();
    j["address"] = 42;
    expectError(j.dump(), "'address' must be a string");
}

TEST(Organization, EmptyActivitiesThrows) {
    auto j = testdata::validJson();
    j["activities"] = nlohmann::json::array();
    expectError(j.dump(), "'activities' must be a non-empty array");
}

TEST(Organization, MissingFontFieldThrows) {
    auto j = testdata::validJson();
    j["alternativeFonts"][2].erase("name");
    expectError(j.dump(), "missing required field 'alternativeFonts[2].name'");
}

TEST(Organization, MissingOfficialFontThrows) {
    auto j = testdata::validJson();
    j.erase("officialFont");
    expectError(j.dump(), "missing required field 'officialFont'");
}

TEST(Organization, FourAlternativeFontsThrows) {
    auto j = testdata::validJson();
    j["alternativeFonts"].erase(0);
    expectError(j.dump(), "exactly 5 fonts, got 4");
}

TEST(Organization, SixAlternativeFontsThrows) {
    auto j = testdata::validJson();
    j["alternativeFonts"].push_back(j["alternativeFonts"][0]);
    expectError(j.dump(), "exactly 5 fonts, got 6");
}

TEST(Organization, MissingFileThrows) {
    EXPECT_THROW(gen::loadOrganization("no/such/file.json"), std::runtime_error);
}
