#include <Rendering/GI/DX11/DXContext.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace GI::detail {

    HRESULT CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut) {
        HRESULT hr = S_OK;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifndef NDEBUG
        dwShaderFlags |= D3DCOMPILE_DEBUG;
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        ID3DBlob* pErrorBlob = nullptr;
        hr = D3DCompileFromFile( fileName, nullptr, nullptr, entryPoint, shaderModel,
                                 dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
        if( FAILED(hr) )
        {
            if( pErrorBlob )
            {
                OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
                pErrorBlob->Release();
            }
            return hr;
        }
        if( pErrorBlob ) pErrorBlob->Release();

        return S_OK;
    }

    void DXContext::init(void* window) {
        DXGI_MODE_DESC bufferDescription;

        ZeroMemory(&bufferDescription, sizeof(DXGI_MODE_DESC));

        int width, height;
        glfwGetWindowSize((GLFWwindow*)window, &width, &height);

        bufferDescription.Width = width;
        bufferDescription.Height = height;
        bufferDescription.RefreshRate.Numerator = 60;
        bufferDescription.RefreshRate.Denominator = 1;
        bufferDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        bufferDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        bufferDescription.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

        swapChainDesc.BufferDesc = bufferDescription;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 1;
        swapChainDesc.OutputWindow = glfwGetWin32Window((GLFWwindow*)window);
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);

        swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
        device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);

        D3D11_TEXTURE2D_DESC depthStencilDesc;
        depthStencilDesc.Width = width;
        depthStencilDesc.Height = height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count   = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;

        device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
        device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

        deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

        /// Initialize Shaders
        ID3DBlob* VS_Buffer = nullptr;
        ID3DBlob* PS_Buffer = nullptr;
        auto hr = CompileShader(L"shaders/shader.fx", "VS", "vs_5_0", &VS_Buffer);

        if(FAILED(hr)) {
            throw std::runtime_error("Failed to compile Vertex Shader!");
        }

        hr = CompileShader(L"shaders/shader.fx", "PS", "ps_5_0", &PS_Buffer);

        if(FAILED(hr)) {
            throw std::runtime_error("Failed to compile Fragment Shader!");
        }

        device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
        device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

        deviceContext->VSSetShader(VS, 0, 0);
        deviceContext->PSSetShader(PS, 0, 0);
    }

    void DXContext::deinit() {
        BackBuffer->Release();
        depthStencilView->Release();
        depthStencilBuffer->Release();
        swapChain->Release();
        device->Release();
        deviceContext->Release();
    }

    void DXContext::clear(D3DXCOLOR color) {
        deviceContext->ClearRenderTargetView(renderTargetView, color);
        deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
    }

}