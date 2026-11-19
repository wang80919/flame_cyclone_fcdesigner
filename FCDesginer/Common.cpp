#include "Common.h"
#include "resource.h"

FCA_TYPE_LIMIT g_FcaLimitForDesign[FT_Max] =
{
    FCA_TYPE_LIMIT(Fca_Type::FT_Full,     0, 0, IDS_STRING_PRODUCT_FULL),
    FCA_TYPE_LIMIT(Fca_Type::FT_Pro,      0, 0, IDS_STRING_PRODUCT_PRO),
    FCA_TYPE_LIMIT(Fca_Type::FT_Advanced, 0, 0, IDS_STRING_PRODUCT_ADVANCED),
    FCA_TYPE_LIMIT(Fca_Type::FT_Standard, 0, 0, IDS_STRING_PRODUCT_STANDARD),
    FCA_TYPE_LIMIT(Fca_Type::FT_Basic,    0, 0, IDS_STRING_PRODUCT_BASIC),
    FCA_TYPE_LIMIT(Fca_Type::FT_None,     0, 0, IDS_STRING_PRODUCT_NONE),
};

FCA_TYPE_LIMIT g_FcaLimitForCoolBoy[FT_Max] =
{
#ifdef _DEBUG
    FCA_TYPE_LIMIT(Fca_Type::FT_Full,     32 * 1048576, 2048,  IDS_STRING_PRODUCT_FULL),
#else
    FCA_TYPE_LIMIT(Fca_Type::FT_Full,     32 * 1048576, 1024,  IDS_STRING_PRODUCT_FULL),
#endif
    FCA_TYPE_LIMIT(Fca_Type::FT_Pro,      24 * 1048576, 256,  IDS_STRING_PRODUCT_PRO),
    FCA_TYPE_LIMIT(Fca_Type::FT_Advanced, 16 * 1048576, 128,  IDS_STRING_PRODUCT_ADVANCED),
    FCA_TYPE_LIMIT(Fca_Type::FT_Standard, 12 * 1048576, 64,   IDS_STRING_PRODUCT_STANDARD),
    FCA_TYPE_LIMIT(Fca_Type::FT_Basic,     6 * 1048576, 32,   IDS_STRING_PRODUCT_BASIC),
    FCA_TYPE_LIMIT(Fca_Type::FT_None,      2 * 1048576, 8,    IDS_STRING_PRODUCT_NONE),
};

FCA_TYPE_LIMIT g_FcaLimitForCoolGirl[FT_Max] =
{
#ifdef _DEBUG
    FCA_TYPE_LIMIT(Fca_Type::FT_Full,     256 * 1048576, 2048,  IDS_STRING_PRODUCT_FULL),
#else
    FCA_TYPE_LIMIT(Fca_Type::FT_Full,     128 * 1048576, 1024,  IDS_STRING_PRODUCT_FULL),
#endif
    FCA_TYPE_LIMIT(Fca_Type::FT_Pro,      96 * 1048576, 512,  IDS_STRING_PRODUCT_PRO),
    FCA_TYPE_LIMIT(Fca_Type::FT_Advanced, 64 * 1048576, 256,  IDS_STRING_PRODUCT_ADVANCED),
    FCA_TYPE_LIMIT(Fca_Type::FT_Standard, 32 * 1048576, 128,   IDS_STRING_PRODUCT_STANDARD),
    FCA_TYPE_LIMIT(Fca_Type::FT_Basic,    16 * 1048576, 64,   IDS_STRING_PRODUCT_BASIC),
    FCA_TYPE_LIMIT(Fca_Type::FT_None,      4 * 1048576, 12,    IDS_STRING_PRODUCT_NONE),
};

std::map<int, MAPPER_STANDARD> g_CoolGirlMapperTable = {
    std::make_pair(0, MAPPER_STANDARD(0, {1, 2}, {0, 1})),
    std::make_pair(1, MAPPER_STANDARD(1, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(2, MAPPER_STANDARD(2, {1, 2, 4, 8, 16, 32}, {0})),
    std::make_pair(3, MAPPER_STANDARD(3, {1, 2}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(4, MAPPER_STANDARD(4, {1, 2, 4, 8, 16, 32, 64, 128}, {0, 1, 2, 4, 8, 16, 32, 64})),
    std::make_pair(74, MAPPER_STANDARD(74, {1, 2, 4, 8, 16, 32, 64, 128}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(5, MAPPER_STANDARD(5, {1, 2, 4, 8, 16, 32, 64}, {1, 2, 4, 8, 16, 32})),
    std::make_pair(7, MAPPER_STANDARD(7, {1, 2, 4, 8, 16, 32}, {0})),
    std::make_pair(9, MAPPER_STANDARD(9, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(10, MAPPER_STANDARD(10, {1, 2, 4, 8, 16}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(11, MAPPER_STANDARD(11, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(18, MAPPER_STANDARD(18, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(21, MAPPER_STANDARD(21, {1, 2, 4, 8, 16}, {0, 1, 2, 4, 8, 16, 32, 64})),
    std::make_pair(22, MAPPER_STANDARD(22, {1, 2, 4, 8, 16}, {0, 1, 2, 4, 8, 16, 32, 64})),
    std::make_pair(23, MAPPER_STANDARD(23, {1, 2, 4, 8, 16}, {0, 1, 2, 4, 8, 16, 32, 64})),
    std::make_pair(25, MAPPER_STANDARD(25, {1, 2, 4, 8, 16}, {0, 1, 2, 4, 8, 16, 32, 64})),
    std::make_pair(30, MAPPER_STANDARD(30, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4})),
    std::make_pair(32, MAPPER_STANDARD(32, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(33, MAPPER_STANDARD(33, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(34, MAPPER_STANDARD(34, {1, 2, 4, 8, 16, 32}, {0})),
    std::make_pair(36, MAPPER_STANDARD(36, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(38, MAPPER_STANDARD(38, {1, 2, 4, 8}, {0, 1, 2, 4})),
    std::make_pair(48, MAPPER_STANDARD(48, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(42, MAPPER_STANDARD(42, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(65, MAPPER_STANDARD(65, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(66, MAPPER_STANDARD(66, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(67, MAPPER_STANDARD(67, {1, 2, 4, 8, 16}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(69, MAPPER_STANDARD(69, {1, 2, 4, 8, 16, 32, 64, 128}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(70, MAPPER_STANDARD(70, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(71, MAPPER_STANDARD(71, {1, 2, 4, 8, 16}, {0})),
    std::make_pair(73, MAPPER_STANDARD(73, {1, 2, 4, 8}, {0})),
    std::make_pair(75, MAPPER_STANDARD(75, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(78, MAPPER_STANDARD(78, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(79, MAPPER_STANDARD(79, {1, 2, 4}, {0, 1, 2, 4, 8})),
    std::make_pair(83, MAPPER_STANDARD(83, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(87, MAPPER_STANDARD(87, {1, 2}, {0, 1, 2})),
    std::make_pair(89, MAPPER_STANDARD(89, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(90, MAPPER_STANDARD(90, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(93, MAPPER_STANDARD(93, {1, 2, 4, 8, 16}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(97, MAPPER_STANDARD(97, {1, 2, 4, 8, 16}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(112, MAPPER_STANDARD(112, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(118, MAPPER_STANDARD(118, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(133, MAPPER_STANDARD(133, {1, 2, 4}, {0, 1, 2, 4})),
    std::make_pair(146, MAPPER_STANDARD(146, {1, 2, 4}, {0, 1, 2, 4, 8})),
    std::make_pair(152, MAPPER_STANDARD(152, {1, 2, 4, 8}, {0, 1, 2, 4, 8, 16})),
    std::make_pair(163, MAPPER_STANDARD(163, {1, 2, 4, 8, 16, 32, 64, 128}, {0})),
    std::make_pair(184, MAPPER_STANDARD(189, {1, 2}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(189, MAPPER_STANDARD(189, {1, 2, 4, 8, 16, 32}, {0, 1, 2, 4, 8, 16, 32})),
    std::make_pair(206, MAPPER_STANDARD(206, {1, 2, 4, 8}, {0, 1, 2, 4, 8})),
    std::make_pair(228, MAPPER_STANDARD(228, {1, 2, 4, 8, 16, 32, 64, 96, 128}, {0, 1, 2, 4, 8, 16, 32, 64})),
    std::make_pair(241, MAPPER_STANDARD(241, {1, 2, 4, 8, 16, 32, 64, 128}, {0, 1, 2, 4})),
};