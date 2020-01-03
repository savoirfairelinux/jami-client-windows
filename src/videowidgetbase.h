/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

#pragma once

// Qt headers
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <QWindow>
#include <QHBoxLayout>

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

#define D3D11_OBJECT_RELEASE(p) {if(p) p->Release();}
#define D3D11_OBJECT_RESET(p) {if(p) p.Reset();}
#define ERROR_HANDLE_RUN(a,b){ if(FAILED((HRESULT)a)) {qDebug() << b;}}

// The base for video widgets
class VideoWidgetBase : public QWidget {
    Q_OBJECT;

public:
    explicit VideoWidgetBase(QColor bgColor = Qt::transparent,
                             QWidget* parent = 0);
    virtual ~VideoWidgetBase();

    /**
     * Repaints the widget while preventing update/repaint to queue
     * for its parent. This is needed when geometry changes occur,
     * to disable image tearing.
     */
    void forceRepaint();

signals:
    void visibilityChanged(bool visible);

protected:
    virtual void hideEvent(QHideEvent* e) override;
    virtual void showEvent(QShowEvent* e) override;

    virtual void paintBackground(QPainter* painter) = 0;

};

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
        : pos(x,y,z)
        , texCoor(tx , ty)
        , vertexColor(cr, cg, cb, ca){};

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

class D3DVideoWidgetWindow : public QWindow
{
    Q_OBJECT;

public:
    explicit D3DVideoWidgetWindow(int initialWidth, int initialHeight);
    virtual ~D3DVideoWidgetWindow();

    // use this function to update the frame textures when a new frame is inbound
    virtual bool updateTextures(AVFrame* frame);
    // draw function, getting called in paint event function
    virtual void paintD3D();

    // clean the textures when re-initialization is needed
    // TODO: call this function in showing event, hiding event and close event of the widget that holds the video
    void cleanTextures();

protected:
    virtual void resizeEvent(QResizeEvent* ev) override;

    virtual void InitD3D();
    virtual void ResizeD3D(int width, int height);
    virtual void InitScene();
    virtual void CleanUp();

    void initialize();
    void initializeShaderResourceViewWithTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture,
                                                 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
                                                 int index);
    void initializeTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture,
                           Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
                           bool isTextureToBeUpdatedOrRecreated,
                           int width, int height, int index, DXGI_FORMAT textureFormat, D3D11_SUBRESOURCE_DATA resourceData);
    void initializeTextureWithCUDA(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
                                   shaderResourceView, bool isTextureToBeUpdatedOrRecreated,
                                   int width, int height, int lineSize, int index, DXGI_FORMAT textureFormat,
                                   CUdeviceptr startPtr, CUcontext cudaContext);
    bool updateTextureFromCUDA(AVFrame * frame, bool isTextureToBeUpdatedOrRecreated);

    void reInitializeLastFrameParameters();
    void updateLastFrameParameters(AVFrame* frame);
    bool areTexturesToBeUpdatedOrRecreated(AVFrame* frame);

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

class D3DVideoWidgetBase : public QWidget {
    Q_OBJECT;

public:
    explicit D3DVideoWidgetBase(QColor bgColor = Qt::transparent,
        QWidget* parent = 0);
    virtual ~D3DVideoWidgetBase();
    /**
     * Repaints the widget while preventing update/repaint to queue
     * for its parent. This is needed when geometry changes occur,
     * to disable image tearing.
     */
    void forceRepaint();
    bool updateTextures(AVFrame* frame);

signals:
    void visibilityChanged(bool visible);

protected:
    virtual void hideEvent(QHideEvent* e) override;
    virtual void showEvent(QShowEvent* e) override;

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

protected:
    QHBoxLayout* layout_;
    D3DVideoWidgetWindow* d3dWindow_;
    QWidget* containerOfD3DWindow_;
};