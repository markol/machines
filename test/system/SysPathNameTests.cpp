#include <gtest/gtest.h>
#include "system/pathname.hpp"

/** * *
 * I decided to pick something "simple" like the pathname stuff to setup Google Test.
 **/

TEST(SysPathNameTests, NoArgsIsUnsetTest) {
    SysPathName noArgsConstructed = SysPathName();

    ASSERT_EQ(false, noArgsConstructed.set());
}

TEST(SysPathNameTests, NoArgsPathDoesntExist) {
    SysPathName noArgsConstructed = SysPathName();

    ASSERT_EQ(false, noArgsConstructed.existsAsDirectory());
    ASSERT_EQ(false, noArgsConstructed.existsAsFile());
    ASSERT_EQ(false, noArgsConstructed.insensitiveExistsAsFile());
}

TEST(SysPathNameTests, EmptyPathDoesntExist) {
    const char* blankPath = "";
    auto emptyPath = SysPathName{ blankPath };

    ASSERT_EQ(false, emptyPath.existsAsDirectory());
    ASSERT_EQ(false, emptyPath.existsAsFile());
    ASSERT_EQ(false, emptyPath.insensitiveExistsAsFile());
}
