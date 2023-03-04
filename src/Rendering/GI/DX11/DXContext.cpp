#ifndef NO_EXPERIMENTAL_GRAPHICS
#include <Rendering/GI/DX11/DXContext.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace GI::detail {

    struct XVertex {
        XVertex(float x, float y, float z,
               float cr, float cg, float cb)
                : pos(x,y,z), color(cr, cg, cb){}

        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 color;
    };

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
                std::cout << (const char*)pErrorBlob->GetBufferPointer() << std::endl;
                OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
                pErrorBlob->Release();
            }
            return hr;
        }
        if( pErrorBlob ) pErrorBlob->Release();

        return S_OK;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
            {
                    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
    UINT numElements = ARRAYSIZE(layout);

    ID3D11Buffer* triangleVertBuffer;


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
        auto hr = CompileShader(L"shaders/shader.fx", "VS", "vs_4_0", &VS_Buffer);

        if(FAILED(hr)) {
            throw std::runtime_error("Failed to compile Vertex Shader!");
        }

        hr = CompileShader(L"shaders/shader.fx", "PS", "ps_4_0", &PS_Buffer);

        if(FAILED(hr)) {
            throw std::runtime_error("Failed to compile Fragment Shader!");
        }

        device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
        device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

        deviceContext->VSSetShader(VS, 0, 0);
        deviceContext->PSSetShader(PS, 0, 0);

        /// Vertex Buffers
        XVertex v[] = {
            XVertex( 0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f ),
            XVertex( 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f ),
            XVertex( -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f ),
        };

        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 3;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA vertexBufferData;

        ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
        vertexBufferData.pSysMem = v;
        device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &triangleVertBuffer);

        //Create the Input Layout
        device->CreateInputLayout( layout, numElements, VS_Buffer->GetBufferPointer(),
                                        VS_Buffer->GetBufferSize(), &vertLayout );

        //Set the Input Layout
        deviceContext->IASetInputLayout( vertLayout );

        //Set the vertex buffer
        UINT stride = sizeof( XVertex );
        UINT offset = 0;
        deviceContext->IASetVertexBuffers( 0, 1, &triangleVertBuffer, &stride, &offset );

        //Set Primitive Topology
        deviceContext->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

        D3D11_VIEWPORT viewport;
        ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = width;
        viewport.Height = height;

        //Set the Viewport
        deviceContext->RSSetViewports(1, &viewport);
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

        deviceContext->Draw( 3, 0 );
    }

}
#endif