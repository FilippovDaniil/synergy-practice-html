#include "gen/html.hpp"

#include <string>
#include <vector>

namespace gen {
namespace {

constexpr const char* kSampleText =
    "Съешь же ещё этих мягких французских булок, да выпей чаю. "
    "The quick brown fox jumps over the lazy dog. 0123456789";

// Font switcher: buttons set --font-main; #font-N in the URL preselects a font.
constexpr const char* kSwitcherJs = R"JS(
(function () {
  var buttons = document.querySelectorAll('.font-btn');
  function select(btn) {
    document.documentElement.style.setProperty('--font-main', btn.dataset.font);
    buttons.forEach(function (b) { b.setAttribute('aria-pressed', String(b === btn)); });
    document.getElementById('current-font').textContent = btn.dataset.name;
  }
  buttons.forEach(function (btn) {
    btn.addEventListener('click', function () {
      select(btn);
      history.replaceState(null, '', '#font-' + btn.dataset.index);
    });
  });
  var m = /^#font-(\d)$/.exec(location.hash);
  if (m && buttons[m[1]]) select(buttons[m[1]]);
})();
)JS";

bool isHttpUrl(const std::string& url) {
    return url.rfind("https://", 0) == 0 || url.rfind("http://", 0) == 0;
}

}  // namespace

std::string escapeHtml(std::string_view text) {
    std::string out;
    out.reserve(text.size());
    for (char ch : text) {
        switch (ch) {
            case '&': out += "&amp;"; break;
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '"': out += "&quot;"; break;
            case '\'': out += "&#39;"; break;
            default: out += ch; break;
        }
    }
    return out;
}

std::string googleFontsUrl(const Organization& org) {
    std::string url = "https://fonts.googleapis.com/css2?";
    for (const Font& f : allFonts(org)) {
        url += "family=" + f.googleFontsFamily + "&";
    }
    url += "subset=cyrillic,latin&display=swap";
    return url;
}

std::string renderHtml(const Organization& org) {
    const std::vector<Font> fonts = allFonts(org);
    std::string h;
    h.reserve(8192);

    h += "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n";
    h += "  <meta charset=\"UTF-8\">\n";
    h += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
    h += "  <title>" + escapeHtml(org.shortName) + " — место практической подготовки</title>\n";
    h += "  <link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n";
    h += "  <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n";
    h += "  <link rel=\"stylesheet\" href=\"" + escapeHtml(googleFontsUrl(org)) + "\">\n";
    h += "  <link rel=\"stylesheet\" href=\"styles.css\">\n";
    h += "</head>\n<body>\n";

    // Header
    h += "<header class=\"hero\">\n  <div class=\"container\">\n";
    h += "    <p class=\"eyebrow\">Организация, на базе которой я прохожу практическую подготовку</p>\n";
    h += "    <h1>" + escapeHtml(org.fullName) + "</h1>\n";
    h += "    <p class=\"lead\">" + escapeHtml(org.shortName) + "</p>\n";
    h += "  </div>\n</header>\n\n";

    // Font switcher
    h += "<nav class=\"switcher\" aria-label=\"Выбор шрифта\">\n  <div class=\"container\">\n";
    h += "    <span class=\"switcher-label\">Шрифт страницы: <strong id=\"current-font\">" +
         escapeHtml(fonts[0].name) + "</strong></span>\n";
    h += "    <div class=\"switcher-buttons\">\n";
    for (std::size_t i = 0; i < fonts.size(); ++i) {
        const Font& f = fonts[i];
        h += "      <button type=\"button\" class=\"font-btn font-" + std::to_string(i) + "\"";
        h += " data-index=\"" + std::to_string(i) + "\"";
        h += " data-name=\"" + escapeHtml(f.name) + "\"";
        h += " data-font=\"" + escapeHtml(fontStack(f)) + "\"";
        h += " aria-pressed=\"" + std::string(i == 0 ? "true" : "false") + "\">";
        h += escapeHtml(f.name);
        if (i == 0) h += " <small>(официальный)</small>";
        h += "</button>\n";
    }
    h += "    </div>\n  </div>\n</nav>\n\n";

    h += "<main class=\"container\">\n";

    // About
    h += "  <section class=\"card\" id=\"about\">\n    <h2>О компании</h2>\n";
    h += "    <p>" + escapeHtml(org.description) + "</p>\n  </section>\n\n";

    // Legal form
    h += "  <section class=\"card\" id=\"legal-form\">\n    <h2>Организационно-правовая форма</h2>\n";
    h += "    <p>" + escapeHtml(org.legalForm) + "</p>\n  </section>\n\n";

    // Activities
    h += "  <section class=\"card\" id=\"activities\">\n    <h2>Виды деятельности</h2>\n    <ul>\n";
    for (const std::string& a : org.activities) {
        h += "      <li>" + escapeHtml(a) + "</li>\n";
    }
    h += "    </ul>\n  </section>\n\n";

    // Address
    h += "  <section class=\"card\" id=\"address\">\n    <h2>Адрес и география</h2>\n";
    h += "    <p>" + escapeHtml(org.address) + "</p>\n  </section>\n\n";

    // Contacts
    h += "  <section class=\"card\" id=\"contacts\">\n    <h2>Контакты и сайт</h2>\n    <p>Официальный сайт: ";
    if (isHttpUrl(org.website)) {
        h += "<a href=\"" + escapeHtml(org.website) + "\" rel=\"noopener\">" +
             escapeHtml(org.website) + "</a>";
    } else {
        h += escapeHtml(org.website);
    }
    h += "</p>\n  </section>\n\n";

    // Font comparison
    h += "  <section class=\"card\" id=\"fonts\">\n    <h2>Сравнение шрифтов</h2>\n";
    h += "    <p class=\"muted\">Один и тот же абзац, набранный официальным шрифтом "
         "организации и пятью альтернативами.</p>\n";
    h += "    <div class=\"samples\">\n";
    for (std::size_t i = 0; i < fonts.size(); ++i) {
        const Font& f = fonts[i];
        h += "      <figure class=\"sample font-" + std::to_string(i) + "\">\n";
        h += "        <p class=\"sample-text\">" + std::string(kSampleText) + "</p>\n";
        h += "        <figcaption><strong>" + escapeHtml(f.name) + "</strong> — " +
             escapeHtml(f.category);
        if (i == 0) h += " · официальный шрифт";
        if (!f.note.empty()) h += "<br><span class=\"muted\">" + escapeHtml(f.note) + "</span>";
        h += "</figcaption>\n      </figure>\n";
    }
    h += "    </div>\n  </section>\n";

    h += "</main>\n\n";
    h += "<footer class=\"footer\">\n  <div class=\"container\">\n";
    h += "    <p>Страница сгенерирована программой <code>html_generator</code> (C++17) "
         "из файла <code>data/organization.json</code>.</p>\n";
    h += "  </div>\n</footer>\n\n";

    h += "<script>" + std::string(kSwitcherJs) + "</script>\n";
    h += "</body>\n</html>\n";
    return h;
}

}  // namespace gen
