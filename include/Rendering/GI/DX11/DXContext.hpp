#pragma once

#ifndef NO_EXPERIMENTAL_GRAPHICS
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <d3dx9math.h>

#include "Rendering/GI.hpp"
#include "Utilities/Assertion.hpp"

namespace GI::detail {

    using namespace Stardust_Celeste;

    class DXContext : Singleton {
    public:
        DXContext() = default;
        ~DXContext() = default;

        inline static auto get() -> DXContext& {
            static DXContext dxc;
            return dxc;
        }

        void init(void* window);
        void deinit();

        void clear(D3DXCOLOR color);

        IDXGISwapChain* swapChain;
        ID3D11Device* device;
        ID3D11DeviceContext* deviceContext;
        ID3D11RenderTargetView* renderTargetView;
        ID3D11Texture2D* BackBuffer;

        ID3D11DepthStencilView* depthStencilView;
        ID3D11Texture2D* depthStencilBuffer;

        ID3D11VertexShader* VS;
        ID3D11PixelShader* PS;
        ID3D11InputLayout* vertLayout;
    };
}
#endif