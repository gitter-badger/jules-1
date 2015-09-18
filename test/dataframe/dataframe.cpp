#include "catch.hpp"
#include "dataframe/dataframe.hpp"

#include <algorithm>
#include <array>
#include <string>
#include <sstream>
#include <typeinfo>

TEST_CASE("null dataframe", "[dataframe]")
{
    using jules::dataframe;

    dataframe df;

    CHECK(df.empty());
    CHECK(df.null());
    CHECK(df.nrow() == 0);
    CHECK(df.ncol() == 0);
}

TEST_CASE("null dataframe colbind", "[dataframe]")
{
    using jules::column;
    using jules::dataframe;

    column a(0, 0);
    dataframe null;
    dataframe df = null.colbind(a).colbind(column(0, 0));

    CHECK_FALSE(null.null());
    CHECK_FALSE(df.null());

    CHECK(null.empty());
    CHECK(df.empty());

    CHECK(df.nrow() == 0);
    CHECK(df.ncol() == 2);
}

TEST_CASE("dataframe colbind", "[dataframe]")
{
    using jules::column;
    using jules::dataframe;

    column c("c", 0, 0);
    column a("a", 0, 0);
    dataframe null;
    dataframe df = null.colbind(c).colbind(a).colbind(column("b", 0, 0));

    CHECK_FALSE(null.null());
    CHECK_FALSE(df.null());

    CHECK(null.empty());
    CHECK(df.empty());

    CHECK(df.nrow() == 0);
    CHECK(df.ncol() == 3);

    auto cols = df.colnames();
    auto nullcols = null.colnames();
    std::array<std::string, 3> names = {"c", "a", "b"};

    REQUIRE(cols.size() == 3);
    auto mm = std::mismatch(cols.begin(), cols.end(), names.begin());
    CHECK(mm.first == cols.end());

    REQUIRE(nullcols.size() == 3);
    auto mm1 = std::mismatch(cols.begin(), cols.end(), nullcols.begin());
    CHECK(mm1.first == cols.end());

    column d("d", 0, 0);
    dataframe some_df{{"A", {1, 2, 3, 4}}, {"a", {"h", " ", "w", "0"}}};
    CHECK_THROWS(some_df.colbind(a));
    CHECK_THROWS(some_df.colbind(d));
}

TEST_CASE("dataframe select by name", "[dataframe]")
{
    using jules::dataframe;

    dataframe df{{"int", {1, 2, 3, 4}}, {"str", {"h", " ", "w", "0"}}};
    CHECK_THROWS(df.select(""));
    CHECK_THROWS(df.select("h"));
    CHECK(df.select("int").elements_type() == typeid(int));
    CHECK(df.select("str").elements_type() == typeid(std::string));
    CHECK(df.select("str").size() == 4);
}

TEST_CASE("invalid exception dataframe", "[dataframe]")
{
    using jules::dataframe;

    auto create_invalid_dataframe = [] { return dataframe{{1, 2, 3, 4}, {1, 2}}; };
    CHECK_THROWS(create_invalid_dataframe());
}

TEST_CASE("assigning a null dataframe", "[dataframe]")
{
    using jules::dataframe;

    dataframe some_df{{"int", {1, 2, 3, 4}}, {"const char*", {"hello", " ", "world", "!"}}};

    CHECK(!some_df.empty());

    some_df = {};
    CHECK(some_df.empty());
}

TEST_CASE("assigning itself", "[dataframe]")
{
    using jules::dataframe;

    dataframe some_df{{"int", {1, 2, 3, 4}}, {"const char*", {"hello", " ", "world", "!"}}};

    CHECK_NOTHROW(some_df = some_df);
}

TEST_CASE("reading an empty dataframe with header", "[dataframe]")
{
    std::string input = "x\ty\tz\n";

    std::stringstream stream(input);

    auto df = jules::dataframe::read(stream);
    CHECK(df.nrow() == 0);
    CHECK(df.ncol() == 3);
    CHECK(df.empty());
}

TEST_CASE("reading an empty dataframe with line-break", "[dataframe]")
{
    std::string input = "\n";

    std::stringstream stream(input);

    auto df = jules::dataframe::read(stream);
    CHECK(df.nrow() == 0);
    CHECK(df.ncol() == 0);
    CHECK(df.empty());
}

TEST_CASE("reading an empty dataframe from an empty string", "[dataframe]")
{
    std::string input = "";

    std::stringstream stream(input);

    auto df = jules::dataframe::read(stream);
    CHECK(df.nrow() == 0);
    CHECK(df.ncol() == 0);
    CHECK(df.empty());
    CHECK(df.null());

    std::stringstream null_stream;

    auto df2 = jules::dataframe::read(null_stream);
    CHECK(df2.nrow() == 0);
    CHECK(df2.ncol() == 0);
    CHECK(df2.empty());
    CHECK(df.null());
}

TEST_CASE("reading a dataframe", "[dataframe]")
{
    std::string input = "\t\t \n1 \t 2\t  3\n";

    std::stringstream stream(input);

    auto df = jules::dataframe::read(stream);
    CHECK(df.nrow() == 1);
    CHECK(df.ncol() == 3);
    auto cols = df.colnames();
    CHECK(cols.size() == 3);
    CHECK(cols[0] == "");
    CHECK(cols[1] == "");
    CHECK(cols[2] == " ");
}

TEST_CASE("reading an inconsistent dataframe", "[dataframe]")
{
    std::string input = "\ty \t\n1 \t 2\t  3";

    std::stringstream stream(input);

    jules::dataframe df;
    CHECK_THROWS(df = jules::dataframe::read(stream));

    CHECK(df.nrow() == 0);
    CHECK(df.ncol() == 0);
    CHECK(df.null());
    CHECK(df.empty());
    CHECK_THROWS(df.select(0));
}

TEST_CASE("reading and writing a well-formed dataframe", "[dataframe]")
{
    using jules::dataframe;

    std::string data = "y\tx\tz\n" + std::to_string(0.0) + "\t" + std::to_string(1.0) + "\t" +
                       std::to_string(2.0) + "\n" + std::to_string(3.0) + "\t" + std::to_string(4.0) + "\t" +
                       std::to_string(5.0) + "\n";

    std::stringstream is(data);

    auto df = jules::dataframe::read(is);
    CHECK(df.nrow() == 2);
    CHECK(df.ncol() == 3);

    std::stringstream os1;
    dataframe::write(df, os1);
    CHECK(data == os1.str());

    std::stringstream os2;
    dataframe::write({{"y", {0, 3}}, {"x", {1, 4}}, {"z", {2, 5}}}, os2);
    CHECK(data == os2.str());

    auto cols = df.colnames();
    std::array<std::string, 3> names = {"y", "x", "z"};
    // CHECK(cols == names);

    REQUIRE(cols.size() == 3);
    auto mm = std::mismatch(cols.begin(), cols.end(), names.begin());
    CHECK(mm.first == cols.end());

    CHECK_THROWS(df.select(-1));
    CHECK_THROWS(df.select(4));

    auto c1 = df.select(0);
    CHECK(c1.name() == "y");

    auto c2 = df.select(1);
    CHECK(c2.name() == "x");

    auto c3 = df.select(2);
    CHECK(c3.name() == "z");
}
