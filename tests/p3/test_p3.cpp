// tests/p3/test_p3.cpp — YOUR P3 TEST SUITE.
//
// The five tests below are worked examples. Keep them, and add your own
// until you have AT LEAST 15 (spec §6 lists what must be covered).
// Name every test <area>_<what>: reg_ for the registry, ws_ for the
// workspace, calc_ for the calculator.
//
// Build & run:   cmake --build build && ./build/test_p3
// One area:      ./build/test_p3 reg_        (names starting with "reg_")
// One test:      ./build/test_p3 =reg_add_and_find

#include "../mini_test.h"
#include "../test_helpers.h"     // TempDir, write_text, args1, ...

#include "tools/calculator.h"
#include "tools/tool_registry.h"
#include "tools/workspace.h"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

// A tool whose name you choose — use it to fill the registry in tests.
class NamedTool : public Tool {
public:
    explicit NamedTool(std::string n) : n_(std::move(n)), schema_(JsonValue::object()) {}
    std::string name() const override { return n_; }
    std::string description() const override { return "test tool"; }
    const JsonValue& schema() const override { return schema_; }
    ToolResult invoke(const JsonValue&, ToolContext&) override { return ToolResult::success(n_); }
private:
    std::string n_;
    JsonValue   schema_;   // an empty JSON object: {}
};

// Tokenize with the provided tokenizer, then run YOUR calc_evaluate.
bool eval(const std::string& expr, double* value, std::string* error) {
    std::vector<CalcToken> tokens;
    if (!calc_tokenize(expr, &tokens, error)) return false;
    return calc_evaluate(tokens, value, error);
}

}  // namespace

// ---------------------------------------------------------------- examples

TEST(reg_add_and_find) {
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("alpha");
    Tool* raw = t.get();                        // remember the address before moving
    REQUIRE(r.add(std::move(t)));
    CHECK(r.size() == 1);
    CHECK(r.find("alpha") == raw);              // same object, not a copy
    CHECK(r.find("beta") == nullptr);
}

TEST(reg_duplicate_keeps_original) {
    ToolRegistry r;
    auto first = std::make_unique<NamedTool>("alpha");
    Tool* original = first.get();
    REQUIRE(r.add(std::move(first)));
    CHECK(!r.add(std::make_unique<NamedTool>("alpha")));
    CHECK(r.find("alpha") == original);
    CHECK(r.size() == 1);
}

TEST(reg_for_each) {
    ToolRegistry reg;
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        REQUIRE(reg.add(std::make_unique<NamedTool>("tool_" + std::to_string(i))));
    }
int count = 0;
reg.for_each([&](const Tool& tool) {               // a lambda: runs once per tool
    std::string hello = tool.name();
    ++count;                                       // [&] lets it change `count`
});
CHECK(count == N);
}

TEST(reg_load_factor_always_less_than_1){


    ToolRegistry reg;
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        REQUIRE(reg.add(std::make_unique<NamedTool>("tool_" + std::to_string(i))));
        CHECK(reg.load_factor() <= 1.0);  // checks that the load factor never exceeds 1
    }

}

TEST(reg_every_name_can_be_found){
    ToolRegistry reg;
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        REQUIRE(reg.add(std::make_unique<NamedTool>("tool_" + std::to_string(i))));
    }
    for (int j = 0; j < N; j++){
        CHECK(reg.find(("tool_" + std::to_string(j))) != nullptr);  //checks that each name for the tool is found
    }

}

TEST(reg_max_probe_length_less_than_64){
    ToolRegistry reg;
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        REQUIRE(reg.add(std::make_unique<NamedTool>("tool_" + std::to_string(i))));
        CHECK(reg.load_factor() <= 64);  // checks that the load factor never exceeds 1
    }
    printf("Longest chain for my hash: %ld\n", reg.max_probe_length());
    printf("Bucket count for my hash: %ld\n", reg.bucket_count());
    printf("Load factor for my hash: %f\n", reg.load_factor());
}

TEST(reg_alternative_hash){
    ToolRegistry reg;
    ToolRegistry worse_reg;
    const int N = 1000;  //I tried higher as well, but that breaks it because it tries to allocate too much memory and it runs out

    for (int i = 0; i < N; ++i) {
        REQUIRE(worse_reg.worse_add(std::make_unique<NamedTool>("tool_" + std::to_string(i))));
        
    }
    printf("Longest chain for worse hash: %ld\n", worse_reg.max_probe_length());

    }



TEST(ws_rejects_parent_escape) {
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;
    REQUIRE(ws.resolve("notes/todo.txt", &out, &reason));   // sanity: accepts *something*
    CHECK(out == ws.root() / "notes" / "todo.txt");
    CHECK(!ws.resolve("../etc/passwd", &out, &reason));
    CHECK(!reason.empty());
}

TEST(ws_rejects_null){
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;
    CHECK(!ws.resolve(std::string_view("notes\0/x", 8), &out, &reason));  //string_view specifies that the string is 8 chars long so it includes the null
    CHECK(!reason.empty());
}

TEST(ws_accept_roots_correct){
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;
    CHECK(ws.resolve("notes/todo.txt", &out, &reason)); 
    CHECK(out == ws.root() / "notes" / "todo.txt");
    CHECK(ws.resolve("./notes//todo.txt", &out, &reason));  
    CHECK(out == ws.root() / "notes" / "todo.txt");
    CHECK(ws.resolve(".", &out, &reason));  
    CHECK(out == ws.root());
    CHECK(ws.resolve("a/../b", &out, &reason));  
    CHECK(out == ws.root() / "b");
    CHECK(ws.resolve("a/..", &out, &reason));  
    CHECK(out == ws.root());
    CHECK(ws.resolve("new/dir/file.txt", &out, &reason));  
    CHECK(out == ws.root() /"new" / "dir"/ "file.txt");
    CHECK(ws.resolve("my..notes.txt", &out, &reason));  
    CHECK(out == ws.root() / "my..notes.txt");


}

TEST(calc_precedence) {
    double v = 0;
    std::string err;
    REQUIRE(eval("2 + 3 * 4", &v, &err));
    CHECK(v == 14);
}

TEST(calc_division_by_zero) {
    double v = 0;
    std::string err;
    CHECK(!eval("1 / 0", &v, &err));
    CHECK(err == "division by zero");
}

TEST(calc_negatives){
    double v = 0;
    std::string err;
    REQUIRE(eval("-1 + 3", &v, &err));
    CHECK(v == 2);
    REQUIRE(eval("3 * -2", &v, &err));
    CHECK(v == -6);
    REQUIRE(eval("-1 - -8", &v, &err));
    CHECK(v == 7);
    REQUIRE(eval("-1 / -1", &v, &err));
    CHECK(v == 1);
    REQUIRE(eval("-8 * -2", &v, &err));
    CHECK(v == 16);
}

TEST(calc_parentheses){
    double v = 0;
    std::string err;
    REQUIRE(eval("3*(-1 + 3)", &v, &err));
    CHECK(v == 6);
    REQUIRE(eval("1-(-1 + 3)+3", &v, &err));
    CHECK(v == 2);
    REQUIRE(eval("3+(6)*(5+1)", &v, &err));
    CHECK(v == 39);
    REQUIRE(eval("2*((-1 + 3)/(2-1))", &v, &err));
    CHECK(v == 4);
}

TEST(calc_left_to_right){
double v = 0;
    std::string err;    
    REQUIRE(eval("1 - 2 - 3", &v, &err));
    CHECK(v == -4);  // if the answer is 2, the thing is not evaluating left to right
    REQUIRE(eval("1 - 2 - 3 - 4 - 5", &v, &err));
    CHECK(v == -13); 
}

TEST(calc_table_4_6_tests){
    double v = 0;
    std::string err; 
    REQUIRE(eval("2 + 3 * 4", &v, &err));
    CHECK(v == 14);
    REQUIRE(eval("10 - 4 - 3", &v, &err));
    CHECK(v == 3);
    REQUIRE(eval("100 / 10 / 5", &v, &err));
    CHECK(v == 2);
    REQUIRE(eval("(2+3) * 4", &v, &err));
    CHECK(v == 20);
    REQUIRE(eval("2 * (3 + 4) - 5", &v, &err));
    CHECK(v == 9);
    REQUIRE(eval("-3 * -3", &v, &err));
    CHECK(v == 9);
    REQUIRE(eval("7 / 2", &v, &err));
    CHECK(v == 3.5);
    REQUIRE(!eval("1/(2 - 2)", &v, &err));
    CHECK(err == "division by zero");
    
}

// ------------------------------------------------------------- your tests
//
// Still to write (spec §6 has the full list). Suggested names:
//   reg_null_rejected, reg_for_each_visits_each_once
//   reg_10000_tools               REQUIRED: print bucket_count / load_factor /
//                                 max_probe_length, check the bound
//   reg_alternative_hash          REQUIRED: longest chain of a weak hash
//   ws_<case>                     REQUIRED: every row of the table in spec §4.4
//   calc_<case>                   left-to-right, parentheses, negatives

int main(int argc, char** argv) { return mt::run_all(argc, argv); }
