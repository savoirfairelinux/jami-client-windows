#pragma once

#include <QObject>
#include <QMutex>
#include <QtQuick/QQuickItem>
#include <QSGRendererInterface>
#include <QtQuick/QQuickWindow>

// D3D headers
#ifndef interface struct
#define interface struct
#endif
#include <wrl/client.h>
#include <DirectXMath.h>

#include <d3d11.h>
#include <d3d10.h>
#include <d3dcompiler.h>

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

#define D3D11_OBJECT_RELEASE(p) {if(p) p->Release();}
#define D3D11_OBJECT_RESET(p) {if(p) p.Reset();}
#define ERROR_HANDLE_RUN(a,b){ if(FAILED((HRESULT)a)) {qDebug() << b;}}

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

class D3D11QtQuickVideoWidgetBase;

class D3D11Renderer : public QObject {
    Q_OBJECT
public:
    D3D11Renderer(QObject* parent = nullptr);
    virtual ~D3D11Renderer();

    void setViewportSize(const QRectF& rect) { m_viewportRect = rect; }
    void setWindow(QQuickWindow* window) { m_window = window; }
    void setParantingQuickItem(D3D11QtQuickVideoWidgetBase* parent);

    // use this function to update the frame textures when a new frame is inbound
    virtual bool updateTextures(AVFrame* frame);
    void cleanTextures();

public:
    QMutex mutexUpdateTextures_;

signals:
    void frameStartSlotFinished();
    void mainPassRecordingStartSlotFinished();

protected:
    void initializeShaderResourceViewWithTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture,
                                                 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
                                                 int index);
    void initializeTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture,
                           Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
                           bool isTextureToBeUpdatedOrRecreated,
                           int width, int height, int index, DXGI_FORMAT textureFormat, D3D11_SUBRESOURCE_DATA resourceData);
    void initializeTextureWithCUDA(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
                                   bool isTextureToBeUpdatedOrRecreated,int width, int height, int lineSize, int index, DXGI_FORMAT textureFormat,
                                   CUdeviceptr startPtr, CUcontext cudaContext);
    bool updateTextureFromCUDA(AVFrame* frame, bool isTextureToBeUpdatedOrRecreated);

    void reInitializeLastFrameParameters();
    void updateLastFrameParameters(AVFrame* frame);
    bool areTexturesToBeUpdatedOrRecreated(AVFrame* frame);

public slots:
    // what to do when a frame starts
    void frameStart();
    // Drawing function call
    void mainPassRecordingStart();

protected:
    enum Stage {
        VertexStage,
        FragmentStage
    };
    void prepareShader(Stage stage);
    QByteArray compileShader(Stage stage,
        const QByteArray& source,
        const QByteArray& entryPoint);
    void init();

    void cleanUp();

    QRectF m_viewportRect;
    QQuickWindow* m_window;
    D3D11QtQuickVideoWidgetBase* m_parantingItem;

    QByteArray m_vert;
    QByteArray m_vertEntryPoint;
    QByteArray m_frag;
    QByteArray m_fragEntryPoint;

    bool m_initialized = false;

    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dDevContext_;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState_;

    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencileState_;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indiceBuffer_;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;

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

    const int texYIndex_ = 2;

    const int texUIndex_ = texYIndex_ + 1;
    const int texVIndex_ = texUIndex_ + 1;
    const int texUV_NV12_Index_ = texVIndex_ + 1;
};

class D3D11QtQuickVideoWidgetBase : public QQuickItem
{
    Q_OBJECT
public:
    D3D11QtQuickVideoWidgetBase();
    virtual ~D3D11QtQuickVideoWidgetBase();

public slots:
    void sync();
    void cleanupRenderer();

protected slots:
    void handleWindowChanged(QQuickWindow* win);

protected:
    void releaseResources() override;

    D3D11Renderer* renderer_;
};

class D3D11QtQuickVideoPreviewWidget : public D3D11QtQuickVideoWidgetBase
{
    Q_OBJECT

public:
    D3D11QtQuickVideoPreviewWidget();
    virtual ~D3D11QtQuickVideoPreviewWidget();

private:
    bool isReadyToRender = false;
};

class D3D11QtQuickVideoDistantwWidget : public D3D11QtQuickVideoWidgetBase
{
    Q_OBJECT

public:
    D3D11QtQuickVideoDistantwWidget();
    virtual ~D3D11QtQuickVideoDistantwWidget();
};