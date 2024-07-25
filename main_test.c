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

bool test__ignore_is_match__exact_file(void)
{
    bool result;
    char entry[] = "ignore_this.c";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    result = ignore_is_match(&ignore_list, "ignore_this.c");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "do_not_ignore_this.c");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__directory(void)
{
    char entry[] = "build/";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};
    bool result = ignore_is_match(&ignore_list, "build/somefile.c");
    ASSERT_TEST(result == true);

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

    result = ignore_is_match(&ignore_list, "logs/debug.log");
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
    char entry[] = "**/test/file_a.c";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    bool result = ignore_is_match(&ignore_list, "project/test/file_a.c");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "project/test/file_b.c");
    ASSERT_TEST(result == false);

    return true;
}

bool test__ignore_is_match__dot_folders(void)
{
    char entry[] = ".git";
    char *entries[] = {entry};
    ignore_list_t ignore_list = {.entries = entries, .count = 1};

    bool result = ignore_is_match(&ignore_list, ".git");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, ".gitignore");
    ASSERT_TEST(result == false);

    return true;
}

int main(void)
{
    TEST(test__ignore_is_match__empty_list);
    TEST(test__ignore_is_match__exact_file);
    TEST(test__ignore_is_match__directory);
    TEST(test__ignore_is_match__wildcard_type);
    TEST(test__ignore_is_match__wildcard_type_with_exception);
    TEST(test__ignore_is_match__double_wildcard_directory);
    TEST(test__ignore_is_match__double_wildcard_directory_file);
    TEST(test__ignore_is_match__double_wildcard_directory_specific_file);
    TEST(test__ignore_is_match__dot_folders);

    return display_test_summary();
}