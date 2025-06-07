#pragma once

#include <flux/renderer/GraphicsContext.h>
#include <flux/renderer/RendererAPI.h>
#include <flux/renderer/OrthographicCamera.h>

namespace flux
{
    struct Renderer2DSpecification
    {
        uint32_t maxQuads = 1000;
        Ref<GraphicsContext> graphicsContext;
    };

    class Renderer2D
    {
    public:
        Renderer2D(const Renderer2DSpecification& spec);
        ~Renderer2D();

        const Ref<OrthographicCamera>& ActiveCamera() const { return camera_; }
        void SetActiveCamera(const Ref<OrthographicCamera>& camera);

        void BeginFrame();
        void EndFrame();

        void StartBatch();
        void EndBatch();

        void FlushCurrentBatch();

        void DrawQuad(const glm::mat4& transform, const Ref<Image>& image);

    private:
        struct QuadVertex
        {
            glm::vec3 position;
            glm::vec4 color;
            glm::vec2 texCoord;
        };

        struct BatchData
        {
            ~BatchData() { delete[] quadVertexBufferBase; }

            Ref<Framebuffer> framebuffer_;

            std::vector<Ref<VertexBuffer>> quadVertexBuffers;
            std::vector<Ref<VertexArray>> quadVertexArrays;
            Ref<Pipeline> quadPipeline;
            Ref<RenderPass> quadRenderPass;

            uint32_t quadIndexCount = 0;
            QuadVertex* quadVertexBufferBase = nullptr;
            QuadVertex* quadVertexBufferPtr = nullptr;

            glm::vec4 quadVertexPositions[4];
        };

        Scope<BatchData> batchData_;

        const uint32_t maxQuads_;
        const uint32_t maxVertices_;
        const uint32_t maxIndices_;

        Ref<RendererAPI> renderer_;
        Ref<GraphicsContext> context_;

        Ref<OrthographicCamera> camera_;
        Ref<UniformBuffer> viewProjectionUB_;
    };
}