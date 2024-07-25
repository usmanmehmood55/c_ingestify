#include "common.h"
#include "ignore.h"
#include "ingestify.h"

#include "c_asserts.h"

// Test case to check that an empty ignore list does not ignore any files
bool test_ignore_empty_list(void)
{
    ignore_list_t ignore_list = { .entries = NULL, .count = 0 };
    bool result = ignore_is_match(&ignore_list, "somefile.c");
    ASSERT_TEST(result == false);

    return true;
}

// Test case to check that files matching an ignore pattern are correctly ignored
bool test_ignore_matching_file(void)
{
    char *entries[] = { "ignore_this.c" };
    ignore_list_t ignore_list = { .entries = entries, .count = 1 };
    bool result = ignore_is_match(&ignore_list, "ignore_this.c");
    ASSERT_TEST(result == true);

    return true;
}

// Test case to check that files not matching any ignore patterns are not ignored
bool test_ignore_non_matching_file(void)
{
    char *entries[] = { "ignore_this.c" };
    ignore_list_t ignore_list = { .entries = entries, .count = 1 };
    bool result = ignore_is_match(&ignore_list, "do_not_ignore_this.c");
    ASSERT_TEST(result == false);

    return true;
}

// Test case to check that directories matching an ignore pattern are correctly ignored
bool test_ignore_matching_directory(void)
{
    char entry[] = "build/";
    char *entries[] = { entry };
    ignore_list_t ignore_list = { .entries = entries, .count = 1 };
    bool result = ignore_is_match(&ignore_list, "build/somefile.c");
    ASSERT_TEST(result == true);

    return true;
}

// Test case to check that wildcards in ignore patterns are correctly handled
bool test_ignore_wildcard(void)
{
    char *entries[] = { "*.o" };
    ignore_list_t ignore_list = { .entries = entries, .count = 1 };
    bool result = ignore_is_match(&ignore_list, "objectfile.o");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, "objectfile.c");
    ASSERT_TEST(result == false);

    return true;
}

// Test case to check that ignore patterns match substrings in paths
bool test_ignore_substring_match(void)
{
    char entry[] = "**/test/";
    char *entries[] = { entry };
    ignore_list_t ignore_list = { .entries = entries, .count = 1 };
    bool result = ignore_is_match(&ignore_list, "project/test/file.c");
    ASSERT_TEST(result == true);

    return true;
}

// Test case to check that ignore patterns match substrings in paths
bool test_ignore_dot_folders(void)
{
    char entry[] = ".git";
    char *entries[] = { entry };
    ignore_list_t ignore_list = { .entries = entries, .count = 1 };

    bool result = ignore_is_match(&ignore_list, ".git");
    ASSERT_TEST(result == true);

    result = ignore_is_match(&ignore_list, ".gitignore");
    ASSERT_TEST(result == false);

    return true;
}

int main(void)
{
    TEST(test_ignore_empty_list);
    TEST(test_ignore_matching_file);
    TEST(test_ignore_non_matching_file);
    TEST(test_ignore_matching_directory);
    TEST(test_ignore_wildcard);
    TEST(test_ignore_substring_match);
    TEST(test_ignore_dot_folders);

    return display_test_summary();
}