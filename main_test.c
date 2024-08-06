#include "common.h"
#include "ignore.h"
#include "ingestify.h"

#include "c_asserts.h"

bool test__ignore_is_match__empty_list(void)
{
    ignore_list_t ignore_list = {.entries = NULL, .count = 0};

    bool result = ignore_is_match(&ignore_list, "somefile.c");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__wildcard_type(void)
{
    bool result;
    char entry[] = "*.log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "foo.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, ".log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "log.debug");
    ASSERT_TEST(result == false);

    result = ignore_is_match(&ignore_list, "log.foo");
    ASSERT_TEST(result == false);

    result = ignore_is_match(&ignore_list, "log.");
    ASSERT_TEST(result == false);

    result = ignore_is_match(&ignore_list, "logs/log.debug");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__wildcard_type_with_exception(void)
{
    bool result;
    char entry_1[] = "*.log";
    char entry_2[] = "!important.log";
    char *entries[] = {entry_1, entry_2};
    ignore_list_t ignore_list = {.entries = entries, .count = 2};

    result = ignore_is_match(&ignore_list, "debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "foo.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, ".log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/important.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__double_wildcard_directory(void)
{
    bool result;
    char entry[] = "**/logs/";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "logs/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/monday/foo.bar");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "build/logs/debug.log");
    ASSERT_TEST(result == true);

    return true;
}

bool test__ignore_is_match__double_wildcard_directory_file(void)
{
    bool result;
    char entry[] = "**/logs/debug.log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "logs/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "build/logs/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/build/debug.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__double_wildcard_directory_specific_file(void)
{
    bool result;
    char entry[] = "**/test/file_a.c";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "project/test/file_a.c");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "project/test/file_b.c");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__dot_folders(void)
{
    bool result;
    char entry[] = ".git";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, ".git");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, ".gitignore");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__question_mark(void)
{
    bool result;
    char entry[] = "debug?.log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "debug0.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debugg.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug10.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__bracket_num_range(void)
{
    bool result;
    char entry[] = "debug[0-9].log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "debug0.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug1.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug9.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug10.log");
    ASSERT_TEST(result == false);

    result = ignore_is_match(&ignore_list, "debug11.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__bracket_specific_nums(void)
{
    bool result;
    char entry[] = "debug[01].log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "debug0.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug1.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug2.log");
    ASSERT_TEST(result == false);

    result = ignore_is_match(&ignore_list, "debug01.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__bracket_not_specific_nums(void)
{
    bool result;
    char entry[] = "debug[!01].log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "debug2.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug0.log");
    ASSERT_TEST(result == false);

    result = ignore_is_match(&ignore_list, "debug1.log");
    ASSERT_TEST(result == false);

    result = ignore_is_match(&ignore_list, "debug01.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__bracket_char_range(void)
{
    bool result;
    char entry[] = "debug[a-z].log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "debuga.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debugb.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debugz.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug1.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__dir_or_file(void)
{
    bool result;
    char entry[] = "logs";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "logs");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/latest/foo.bar");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "build/logs/foo.bar");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "build/logs/latest/debug.log");
    ASSERT_TEST(result == true);

    return true;
}

bool test__ignore_is_match__directory(void)
{
    bool result;
    char entry[] = "logs/";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "logs/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/latest/foo.bar");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "build/logs/foo.bar");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "build/logs/latest/debug.log");
    ASSERT_TEST(result == true);

    return true;
}

bool test__ignore_is_match__double_wildcard_mid_path(void)
{
    bool result;
    char entry[] = "logs/**/debug.log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "logs/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/monday/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/monday/pm/debug.log");
    ASSERT_TEST(result == true);

    return true;
}

bool test__ignore_is_match__wildcard_in_dir_name(void)
{
    bool result;
    char entry[] = "logs/*day/debug.log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "logs/monday/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/tuesday/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "logs/latest/debug.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__exact_path(void)
{
    bool result;
    char entry[] = "logs/debug.log";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "logs/debug.log");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "debug.log");
    ASSERT_TEST(result == false);

    result = ignore_is_match(&ignore_list, "build/logs/debug.log");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__self_test_generic(void)
{
    char entry_0[] = ".git";
    char entry_1[] = "build";
    char entry_2[] = "file_b.txt";
    char entry_3[] = "test/file_b.txt";
    char entry_4[] = "*.hex";
    char entry_5[] = "components/c_asserts/.gitignore";
    char entry_6[] = "components/c_asserts/CMakeLists.txt";
    char entry_7[] = "components/c_asserts/LICENSE";
    char entry_8[] = "components/c_asserts/README.md";
    char entry_9[] = "main_test.c";

    char *entries[] = { entry_0, entry_1, entry_2, entry_3, entry_4, entry_5, entry_6, entry_7, entry_8, entry_9 };
    ignore_list_t ignore_list = { .entries = entries, .count = 10 };

    ASSERT_TEST(ignore_is_match(&ignore_list, ".git")                                == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "build")                               == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/c_asserts/.gitignore")     == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/c_asserts/CMakeLists.txt") == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/c_asserts/LICENSE")        == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/c_asserts/README.md")      == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "main_test.c")                         == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "test/file_b.txt")                     == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "test/file_c.hex")                     == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, "test/file_d.hex")                     == true);
    ASSERT_TEST(ignore_is_match(&ignore_list, ".gitignore")                          == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, ".gitmodules")                         == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, ".vscode/c_cpp_properties.json")       == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, ".vscode/launch.json")                 == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, ".vscode/settings.json")               == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, ".vscode/tasks.json")                  == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "CMakeLists.txt")                      == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/common/CMakeLists.txt")    == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/common/common.c")          == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/common/common.h")          == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/c_asserts/.git")           == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/c_asserts/c_asserts.c")    == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/c_asserts/c_asserts.h")    == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/ignore/CMakeLists.txt")    == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/ignore/ignore.c")          == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/ignore/ignore.h")          == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/ingestify/CMakeLists.txt") == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/ingestify/ingestify.c")    == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "components/ingestify/ingestify.h")    == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "ingestify_ignore.txt")                == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "LICENSE")                             == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "main.c")                              == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "README.md")                           == false);
    ASSERT_TEST(ignore_is_match(&ignore_list, "test/file_a.txt")                     == false);

    return true;
}

bool test__ignore_read_list__generic(void)
{
    char *ignore_file_path = "test/ingestify_ignore.txt";
    ignore_list_t *ignore_list = ignore_read_list(ignore_file_path);

    ASSERT_TEST(EXISTS(ignore_list));
    ASSERT_TEST(ignore_list->count == 11);
    ASSERT_TEST(strncmp(ignore_list->entries[0]  , ".git",                                __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[1]  , "build",                               __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[2]  , "file_b.txt",                          __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[3]  , "test/file_b.txt",                     __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[4]  , "*.hex",                               __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[5]  , "components/c_asserts/.gitignore",     __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[6]  , "components/c_asserts/CMakeLists.txt", __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[7]  , "components/c_asserts/LICENSE",        __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[8]  , "components/c_asserts/README.md",      __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[9]  , "main_test.c",                         __PATH_MAX) == 0);
    ASSERT_TEST(strncmp(ignore_list->entries[10] , "LICENSE",                             __PATH_MAX) == 0);

    ignore_free_list(ignore_list);

    return true;
}

int main(void)
{
    TEST(test__ignore_is_match__empty_list);
    TEST(test__ignore_is_match__wildcard_type);
    TEST(test__ignore_is_match__wildcard_type_with_exception);
    TEST(test__ignore_is_match__double_wildcard_directory);
    TEST(test__ignore_is_match__double_wildcard_directory_file);
    TEST(test__ignore_is_match__double_wildcard_directory_specific_file);
    TEST(test__ignore_is_match__dot_folders);
    TEST(test__ignore_is_match__question_mark);
    TEST(test__ignore_is_match__bracket_num_range);
    TEST(test__ignore_is_match__bracket_specific_nums);
    TEST(test__ignore_is_match__bracket_not_specific_nums);
    TEST(test__ignore_is_match__bracket_char_range);
    TEST(test__ignore_is_match__dir_or_file);
    TEST(test__ignore_is_match__directory);
    TEST(test__ignore_is_match__double_wildcard_mid_path);
    TEST(test__ignore_is_match__wildcard_in_dir_name);
    TEST(test__ignore_is_match__exact_path);

    TEST(test__ignore_is_match__self_test_generic);
    TEST(test__ignore_read_list__generic);

    return display_test_summary();
}