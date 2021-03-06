// Copyright 2015 XLGAMES Inc.
//
// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#pragma once

#include "IOverlayContext.h"
#include "../RenderCore/Metal/DeviceContext.h"
#include "../RenderCore/Metal/InputLayout.h"        // for ConstantBufferPacket
#include "../RenderCore/Techniques/TechniqueUtils.h"
#include "../Math/Matrix.h"
#include "Font.h"
#include <vector>

#pragma warning(disable:4324)

namespace RenderOverlays
{
    class ImmediateOverlayContext : public IOverlayContext
    {
    public:
        void    DrawPoint      (ProjectionMode::Enum proj, const Float3& v,     const ColorB& col,      uint8 size);
        void    DrawPoints     (ProjectionMode::Enum proj, const Float3 v[],    uint32 numPoints,       const ColorB& col,    uint8 size);
        void    DrawPoints     (ProjectionMode::Enum proj, const Float3 v[],    uint32 numPoints,       const ColorB col[],   uint8 size);

        void    DrawLine       (ProjectionMode::Enum proj, const Float3& v0,    const ColorB& colV0,    const Float3& v1,     const ColorB& colV1, float thickness);
        void    DrawLines      (ProjectionMode::Enum proj, const Float3 v[],    uint32 numPoints,       const ColorB& col,    float thickness);
        void    DrawLines      (ProjectionMode::Enum proj, const Float3 v[],    uint32 numPoints,       const ColorB col[],   float thickness);

        void    DrawTriangles  (ProjectionMode::Enum proj, const Float3 v[],    uint32 numPoints,       const ColorB& col);
        void    DrawTriangles  (ProjectionMode::Enum proj, const Float3 v[],    uint32 numPoints,       const ColorB col[]);

        void    DrawTriangle   (ProjectionMode::Enum proj, const Float3& v0,    const ColorB& colV0,    const Float3& v1,     
                                const ColorB& colV1, const Float3& v2,       const ColorB& colV2);

        void    DrawQuad       (ProjectionMode::Enum proj, 
                                const Float3& mins, const Float3& maxs, 
                                ColorB color0, ColorB color1,
                                const Float2& minTex0, const Float2& maxTex0, 
                                const Float2& minTex1, const Float2& maxTex1,
                                const std::string& pixelShader);

        void    DrawQuad(
            ProjectionMode::Enum proj, 
            const Float3& mins, const Float3& maxs, 
            ColorB color,
            const std::string& pixelShader);

        void    DrawTexturedQuad(
            ProjectionMode::Enum proj, 
            const Float3& mins, const Float3& maxs, 
            const std::string& texture,
            ColorB color, const Float2& minTex0, const Float2& maxTex0);

        float   DrawText       (const std::tuple<Float3, Float3>& quad, TextStyle* textStyle, ColorB col, TextAlignment::Enum alignment, const char text[], va_list args);
        float   StringWidth    (float scale, TextStyle* textStyle, const char text[], va_list args);
        float   TextHeight     (TextStyle* textStyle);

        void CaptureState();
        void ReleaseState();
        void SetState(const OverlayState& state);

        RenderCore::IThreadContext*                 GetDeviceContext();
        RenderCore::Techniques::ProjectionDesc      GetProjectionDesc() const;
        const RenderCore::Metal::UniformsStream&    GetGlobalUniformsStream() const;

        ImmediateOverlayContext(
            RenderCore::IThreadContext* threadContext, 
            const RenderCore::Techniques::ProjectionDesc& projDesc = RenderCore::Techniques::ProjectionDesc());
        ~ImmediateOverlayContext();

        class ShaderBox;

    private:
        RenderCore::IThreadContext*                         _deviceContext;
        std::shared_ptr<RenderCore::Metal::DeviceContext>   _metalContext;
        std::unique_ptr<uint8[]>	_workingBuffer;
		unsigned					_workingBufferSize;
		unsigned					_writePointer;

        intrusive_ptr<Font>     _font;
        TextStyle               _defaultTextStyle;

        RenderCore::Metal::ConstantBufferPacket _viewportConstantBuffer;
        RenderCore::Metal::ConstantBufferPacket _globalTransformConstantBuffer;
        RenderCore::Metal::UniformsStream _globalUniformsStream;

        RenderCore::Techniques::ProjectionDesc _projDesc;

        enum VertexFormat { PC, PCT, PCR, PCCTT };
        class DrawCall
        {
        public:
            unsigned        _topology;
            unsigned        _vertexOffset;
            unsigned        _vertexCount;
            VertexFormat    _vertexFormat;
            std::string     _pixelShaderName;
            std::string     _textureName;
            ProjectionMode::Enum  _projMode;

            DrawCall(   unsigned topology, unsigned vertexOffset, 
                        unsigned vertexCount, VertexFormat format, ProjectionMode::Enum projMode, 
                        const std::string& pixelShaderName = std::string(),
                        const std::string& textureName = std::string()) 
                : _topology(topology), _vertexOffset(vertexOffset), _vertexCount(vertexCount)
                , _vertexFormat(format), _pixelShaderName(pixelShaderName), _projMode(projMode)
                , _textureName(textureName) {}
        };

        std::vector<DrawCall>   _drawCalls;
        void                    Flush();
        void                    SetShader(unsigned topology, VertexFormat format, ProjectionMode::Enum projMode, const std::string& pixelShaderName);

        template<typename Type> VertexFormat AsVertexFormat() const;
        unsigned                VertexSize(VertexFormat format);
        void                    PushDrawCall(const DrawCall& drawCall);
    };
}



