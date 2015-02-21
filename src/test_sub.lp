default main
namespace main {
    .using TestMain
    .function test() {
        ldstr "   it's test function in test_sub"
        invoke println, 1

        ret
    }
}
