#ifndef D3D11QTQUICKVIDEOWIDGETBAE_H
#define D3D11QTQUICKVIDEOWIDGETBAE_H

#include <QObject>
#include <QQuickItem>

// D3D headers
#ifndef interface struct
#define interface struct
#endif
#include <wrl/client.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <d3d10.h>

extern "C" {
    // FFMPEG headers
#include <libavutil/frame.h>
#include <libavutil/display.h>
#include <libavutil/hwcontext.h>
#include <libavutil/hwcontext_cuda.h>

// CUDA headers
#include <cuda.h>
#include <cudaD3D11.h>

// Libmfx and Media SDK headers
#include "mfxvideo.h"
}
#include "mfxvideo++.h"

struct ConstantBufferDataStruct
{
    DirectX::XMFLOAT2 aViewPortWidthAndHeight;
    DirectX::XMFLOAT2 aWidthAndHeight;
    float angleToRotate = 0.0f;
    bool isNV12 = false;
};

struct Vertex {
    Vertex();
    Vertex(float x, float y, float z,
        float tx, float ty,
        float cr, float cg, float cb, float ca)
        : pos(x, y, z)
        , texCoor(tx, ty)
        , vertexColor(cr, cg, cb, ca) {};

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 texCoor;
    DirectX::XMFLOAT4 vertexColor;
};

const std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout_description = {
    {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"Texcoordinate",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,20,D3D11_INPUT_PER_VERTEX_DATA,0},
};

Vertex vertexArray[] =
{
    Vertex(-1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,1.0f),
    Vertex(1.0f, 1.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,1.0f),
    Vertex(1.0f,-1.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f),
    Vertex(-1.0f,-1.0f,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,1.0f),
};

DWORD indices[] = {
    // front face
    0,1,2,
    2,3,0
};

class D3D11Renderer : public QObject {
public:
    D3D11Renderer(QObject* parent = nullptr);
    ~D3D11Renderer();

private:
    enum Stage {
        VertexStage,
        FragmentStage
    };
    void prepareShader(Stage stage);
    QByteArray compileShader(Stage stage,
        const QByteArray& source,
        const QByteArray& entryPoint);
    void init();

    QSize m_viewportSize;
    qreal m_t;
    QQuickWindow* m_window;

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    QByteArray m_vert;
    QByteArray m_vertEntryPoint;
    QByteArray m_frag;
    QByteArray m_fragEntryPoint;

    bool m_initialized = false;
    ID3D11Buffer* m_vbuf = nullptr;
    ID3D11Buffer* m_cbuf = nullptr;
    ID3D11VertexShader* m_vs = nullptr;
    ID3D11PixelShader* m_ps = nullptr;
    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11RasterizerState* m_rastState = nullptr;
    ID3D11DepthStencilState* m_dsState = nullptr;
    ID3D11BlendState* m_blendState = nullptr;

protected:
    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dDevContext_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indiceBuffer_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<ID3D10Blob> pixelShaderBlob_;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewY_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewU_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewV_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewNV12_;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texY_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texU_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texV_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texUV_NV12_;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;
    ConstantBufferDataStruct constantBufferDataStruct_;

    struct {
        AVPixelFormat LastFormat = AV_PIX_FMT_NONE;
        int LastWidth = 0;
        int LastHeight = 0;
    } lastFrameParameters_;
};

class D3D11QtQuickVideoWidgetBae : public QQuickItem
{
    Q_OBJECT
public:
    D3D11QtQuickVideoWidgetBae();

signals:

};

#endif // D3D11QTQUICKVIDEOWIDGETBAE_H
