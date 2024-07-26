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

    return display_test_summary();
}