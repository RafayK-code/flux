#pragma once

namespace flux
{
    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SrcAlphaSaturate
    };

    enum class BlendOp
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    struct BlendState
    {
        bool enabled = true;
        BlendFactor srcColorBlendFactor = BlendFactor::One;
        BlendFactor dstColorBlendFactor = BlendFactor::Zero;
        BlendFactor srcAlphaBlendFactor = BlendFactor::SrcAlpha;
        BlendFactor dstAlphaBlendFactor = BlendFactor::OneMinusSrcAlpha;

        BlendOp colorBlendOp = BlendOp::Add;
        BlendOp alphaBlendOp = BlendOp::Add;
    };

    enum class DepthCompareOp
    {
        Never,
        NotEqual,
        Less,
        LessOrEqual,
        Greater,
        GreaterOrEqual,
        Equal,
        Always,
    };

    struct DepthState
    {
        bool enabled = true;
        DepthCompareOp depthCompareOp = DepthCompareOp::Less;
    };
}