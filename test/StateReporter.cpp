/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "gtest.h"
#include <clocale>
#include <gtest/gtest.h>
#include <string>

class displayTest : public ::testing::Test
{
public:
	static void SetUpTestCase();
	static void TearDownTestCase();
	void SetUp();

private:
	static const char* previousLocale;
};
const char* displayTest::previousLocale = nullptr;

void displayTest::SetUpTestCase()
{
	previousLocale = std::setlocale(LC_CTYPE, nullptr);
	const char* newLocale = std::setlocale(LC_CTYPE, "en_US.UTF-8");
	if (strcmp(newLocale, "en_US.UTF-8") != 0)
	{
		std::setlocale(LC_CTYPE, previousLocale);
		previousLocale = nullptr;
	}
}

void displayTest::TearDownTestCase()
{
	if (previousLocale != nullptr)
	{
		setlocale(LC_CTYPE, previousLocale);
	}
}

void displayTest::SetUp()
{
	if (previousLocale == nullptr)
	{
		FAIL() << "System does not support locale en_US.UTF-8 required for this test.";
	}
}

namespace display
{
	int len(const std::string&);
	std::string substr(const std::string&, std::ptrdiff_t, std::ptrdiff_t = 0);
} // namespace display

TEST_F(displayTest, len0)
{
	std::string s{""};
	EXPECT_EQ(0, display::len(s));
}

TEST_F(displayTest, lenAlpha)
{
	std::string s{"azertyuiopqsdfghjklmwxcvbn"};
	EXPECT_EQ(26, display::len(s));
}

TEST_F(displayTest, lenFullAlpha)
{
	std::string s{"ｑｗｅｒｔｙｕｉｏｐａｓｄｆｇｈｊｋｌｚｘｃｖｂｎｍ"};
	EXPECT_EQ(52, display::len(s));
}

TEST_F(displayTest, lenFullWidthSlash)
{
	std::string s{"／"};
	EXPECT_EQ(2, display::len(s));
}
TEST_F(displayTest, lenHalfKana)
{
	std::string s{"ｱｲｳｴｵｶｷｸｹｺ"};
	EXPECT_EQ(10, display::len(s));
}

TEST_F(displayTest, lenFullKana)
{
	std::string s{"アイウエオカキクケコ"};
	EXPECT_EQ(20, display::len(s));
}

TEST_F(displayTest, substrAlphaLenOnly)
{
	std::string s{"azertyuiopqsdfghjklmwxcvbn"};
	std::string e{"azertyuiop"};
	EXPECT_EQ(e, display::substr(s, 0, 10));
}

TEST_F(displayTest, substrAlphaStartOnly)
{
	std::string s{"azertyuiopqsdfghjklmwxcvbn"};
	std::string e{"qsdfghjklmwxcvbn"};
	EXPECT_EQ(e, display::substr(s, 10));
}

TEST_F(displayTest, substrAlphaNegLenOnly)
{
	std::string s{"azertyuiopqsdfghjklmwxcvbn"};
	std::string e{"azertyuiop"};
	EXPECT_EQ(e, display::substr(s, 0, -16));
}

TEST_F(displayTest, substrAlphaNegStartOnly)
{
	std::string s{"azertyuiopqsdfghjklmwxcvbn"};
	std::string e{"qsdfghjklmwxcvbn"};
	EXPECT_EQ(e, display::substr(s, -16));
}

TEST_F(displayTest, substrAlphaEmptyResultEndBeforeStart)
{
	std::string s{"azertyuiopqsdfghjklmwxcvbn"};
	std::string e{""};
	EXPECT_EQ(e, display::substr(s, 16, -16));
}

TEST_F(displayTest, substrHalfNormalNegEnd)
{
	std::string s{"ｱｲｳｴｵｶｷｸｹｺ"};
	std::string e{"ｴｵｶｷ"};
	EXPECT_EQ(e, display::substr(s, 3, -3));
}

TEST_F(displayTest, substrFullNormalNegEnd)
{
	std::string s{"アイウエオカキクケコ"};
	std::string e{"エオカキ"};
	EXPECT_EQ(e, display::substr(s, 6, -6));
}

TEST_F(displayTest, substrMixedExact)
{
	std::string s{"ｱイｳエｵカｷクｹコ"};
	std::string e{"エｵカｷ"};
	EXPECT_EQ(e, display::substr(s, 4, 6));
}

TEST_F(displayTest, substrMixedExactNegEnd)
{
	std::string s{"ｱイｳエｵカｷクｹコ"};
	std::string e{"エｵカｷ"};
	EXPECT_EQ(e, display::substr(s, 4, -5));
}

TEST_F(displayTest, substrMixedExactNegStart)
{
	std::string s{"ｱイｳエｵカｷクｹコ"};
	std::string e{"エｵカｷ"};
	EXPECT_EQ(e, display::substr(s, -11, 6));
}

TEST_F(displayTest, substrMixedExactNegStartNegEnd)
{
	std::string s{"ｱイｳエｵカｷクｹコ"};
	std::string e{"エｵカｷ"};
	EXPECT_EQ(e, display::substr(s, -11, -5));
}

TEST_F(displayTest, substrMixedHalf)
{
	std::string s{"ｱイｳエｵカｷクｹコ"};
	std::string e{"ｳエｵカｷ"};
	EXPECT_EQ(e, display::substr(s, 2, 8));
}

TEST_F(displayTest, substrMixedHalfNegEnd)
{
	std::string s{"ｱイｳエｵカｷクｹコ"};
	std::string e{"ｳエｵカｷ"};
	EXPECT_EQ(e, display::substr(s, 2, -4));
}

TEST_F(displayTest, substrMixedHalfNegStart)
{
	std::string s{"ｱイｳエｵカｷクｹコ"};
	std::string e{"ｳエｵカｷ"};
	EXPECT_EQ(e, display::substr(s, -13, 8));
}

TEST_F(displayTest, substrMixedHalfNegStartNegEnd)
{
	std::string s{"ｱイｳエｵカｷクｹコ"};
	std::string e{"ｳエｵカｷ"};
	EXPECT_EQ(e, display::substr(s, -13, -4));
}
