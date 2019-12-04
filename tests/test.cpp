//Copyright 2019 <telsamar>
#include <gtest/gtest.h>
#include <Json.hpp>
TEST(Parse, Text){
    std::string json = "{ \"lastname\" : \"Ivanov\" , \"firstname\" : \"Ivan\" "
                       ",  \"age\" : 25 , \"islegal\" : false , \"marks\""
                       " : [ 4 , 5 , 5 , 5 , 2 , 3 ] , \"address\" : {  "
                       "\"city\" :"" \"Moscow\" ,  \"street\" :"
                                   " \"Vozdvijenka\" } })";
    Json object = Json::parse(json);
    EXPECT_EQ(std::any_cast<std::string>(object["lastname"]), "Ivanov");
    EXPECT_EQ(std::any_cast<bool>(object["islegal"]), false);
    EXPECT_EQ(std::any_cast<int>(object["age"]), 25);

    auto marks = std::any_cast<std::vector<std::any>>(object["marks"]);
    EXPECT_EQ(std::any_cast<int>(marks[0]), 4);
    EXPECT_EQ(std::any_cast<int>(marks[1]), 5);

    auto address = std::any_cast<std::map<std::string,
    std::any>>(object["address"]);
    EXPECT_EQ(std::any_cast<std::string>(address["city"]), "Moscow");
    EXPECT_EQ(std::any_cast<std::string>(address["street"]), "Vozdvijenka");
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
