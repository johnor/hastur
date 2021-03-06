#include "html/parse.h"

#include <catch2/catch.hpp>

using namespace std::literals;

namespace {

TEST_CASE("parser", "[parser]") {
    SECTION("doctype") {
        auto nodes = html::parse("<!doctype html>"sv);
        REQUIRE(nodes.size() == 1);

        auto doctype = nodes[0];
        REQUIRE(doctype.children.size() == 0);
        REQUIRE(std::get<dom::Doctype>(doctype.data).doctype == "html"s);
    }

    SECTION("weirdly capitalized doctype") {
        auto nodes = html::parse("<!docTYpe html>"sv);
        REQUIRE(nodes.size() == 1);

        auto doctype = nodes[0];
        REQUIRE(doctype.children.size() == 0);
        REQUIRE(std::get<dom::Doctype>(doctype.data).doctype == "html"s);
    }

    SECTION("single element") {
        auto nodes = html::parse("<html></html>"sv);
        REQUIRE(nodes.size() == 1);

        auto html = nodes[0];
        REQUIRE(html.children.size() == 0);
        REQUIRE(std::get<dom::Element>(html.data).name == "html"s);
        REQUIRE(std::get<dom::Element>(html.data).attributes.size() == 0);
    }

    SECTION("self-closing single element") {
        auto nodes = html::parse("<br>"sv);
        REQUIRE(nodes.size() == 1);

        auto br = nodes[0];
        REQUIRE(br.children.size() == 0);
        REQUIRE(std::get<dom::Element>(br.data).name == "br"s);
        REQUIRE(std::get<dom::Element>(br.data).attributes.size() == 0);
    }

    SECTION("self-closing single element with slash") {
        auto nodes = html::parse("<img/>"sv);
        REQUIRE(nodes.size() == 1);

        auto img = nodes[0];
        REQUIRE(img.children.size() == 0);
        REQUIRE(std::get<dom::Element>(img.data).name == "img"s);
        REQUIRE(std::get<dom::Element>(img.data).attributes.size() == 0);
    }

    SECTION("multiple elements") {
        auto nodes = html::parse("<span></span><div></div>"sv);
        REQUIRE(nodes.size() == 2);

        auto span = nodes[0];
        REQUIRE(span.children.size() == 0);
        REQUIRE(std::get<dom::Element>(span.data).name == "span"s);
        REQUIRE(std::get<dom::Element>(span.data).attributes.size() == 0);

        auto div = nodes[1];
        REQUIRE(div.children.size() == 0);
        REQUIRE(std::get<dom::Element>(div.data).name == "div"s);
        REQUIRE(std::get<dom::Element>(div.data).attributes.size() == 0);
    }

    SECTION("nested elements") {
        auto nodes = html::parse("<html><body></body></html>"sv);
        REQUIRE(nodes.size() == 1);

        auto html = nodes[0];
        REQUIRE(html.children.size() == 1);
        REQUIRE(std::get<dom::Element>(html.data).name == "html"s);
        REQUIRE(std::get<dom::Element>(html.data).attributes.size() == 0);

        auto body = html.children[0];
        REQUIRE(std::get<dom::Element>(body.data).name == "body"s);
        REQUIRE(std::get<dom::Element>(body.data).attributes.size() == 0);
    }

    SECTION("single-quoted attribute") {
        auto nodes = html::parse("<meta charset='utf-8'/>"sv);
        REQUIRE(nodes.size() == 1);

        auto meta = nodes[0];
        REQUIRE(meta.children.size() == 0);

        auto meta_data = std::get<dom::Element>(meta.data);
        REQUIRE(meta_data.name == "meta"s);
        REQUIRE(meta_data.attributes.size() == 1);
        REQUIRE(meta_data.attributes.at("charset") == "utf-8"s);
    }

    SECTION("double-quoted attribute") {
        auto nodes = html::parse("<meta charset=\"utf-8\"/>"sv);
        REQUIRE(nodes.size() == 1);

        auto meta = nodes[0];
        REQUIRE(meta.children.size() == 0);

        auto meta_data = std::get<dom::Element>(meta.data);
        REQUIRE(meta_data.name == "meta"s);
        REQUIRE(meta_data.attributes.size() == 1);
        REQUIRE(meta_data.attributes.at("charset"s) == "utf-8"s);
    }

    SECTION("multiple attributes") {
        auto nodes = html::parse("<meta name=\"viewport\" content=\"width=100em, initial-scale=1\"/>"sv);
        REQUIRE(nodes.size() == 1);

        auto meta = nodes[0];
        REQUIRE(meta.children.size() == 0);

        auto meta_data = std::get<dom::Element>(meta.data);
        REQUIRE(meta_data.name == "meta"s);
        REQUIRE(meta_data.attributes.size() == 2);
        REQUIRE(meta_data.attributes.at("name"s) == "viewport"s);
        REQUIRE(meta_data.attributes.at("content"s) == "width=100em, initial-scale=1"s);
    }

    SECTION("multiple nodes with attributes") {
        auto nodes = html::parse("<html bonus='hello'><body style='fancy'></body></html>"sv);
        REQUIRE(nodes.size() == 1);

        auto html = nodes[0];
        REQUIRE(html.children.size() == 1);
        auto html_data = std::get<dom::Element>(html.data);
        REQUIRE(html_data.name == "html"s);
        REQUIRE(html_data.attributes.size() == 1);
        REQUIRE(html_data.attributes.at("bonus"s) == "hello"s);

        auto body = html.children[0];
        auto body_data = std::get<dom::Element>(body.data);
        REQUIRE(body_data.name == "body"s);
        REQUIRE(body_data.attributes.size() == 1);
        REQUIRE(body_data.attributes.at("style"s) == "fancy"s);
    }

    SECTION("text node") {
        auto nodes = html::parse("<html>fantastic, the future is now</html>"sv);
        REQUIRE(nodes.size() == 1);

        auto html = nodes[0];
        REQUIRE(html.children.size() == 1);
        REQUIRE(std::get<dom::Element>(html.data).name == "html"s);
        REQUIRE(std::get<dom::Element>(html.data).attributes.size() == 0);

        auto text = html.children[0];
        REQUIRE(std::get<dom::Text>(text.data).text == "fantastic, the future is now"s);
    }
}

} // namespace
