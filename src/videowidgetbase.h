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

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

#define interface struct
#include <d3dcompiler.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define D3D11_OBJECT_RELEASE(p) {if(p){(p)->Release(); p=0;}}

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
    float angleToRotate = 0.0f;
    bool isUsingD3D11HW = false;
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

class D3DVideoWidgetBase : public QWidget {
    Q_OBJECT;

public:
    explicit D3DVideoWidgetBase(QColor bgColor = Qt::transparent,
        QWidget* parent = 0);
    virtual ~D3DVideoWidgetBase();

    virtual QPaintEngine* paintEngine() const override
    {
        return NULL;
    }

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

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

    virtual void InitD3D();
    virtual void ResizeD3D();
    virtual void InitScene();
    virtual void paintD3D();
    virtual void CleanUp();

    void initialize();

protected:
    ID3D11Device* d3dDevice_;
    ID3D11DeviceContext* d3dDevContext_;
    IDXGISwapChain* swapChain_;
    ID3D11RenderTargetView* renderTargetView_;

    ID3D11Buffer* vertexBuffer_;
    ID3D11Buffer* indiceBuffer_;
    ID3D11VertexShader* vertexShader_;
    ID3D11PixelShader* pixelShader_;
    ID3D10Blob* vertexShaderBlob_;
    ID3D10Blob* pixelShaderBlob_;

    ID3D11InputLayout* inputLayout_;
    ID3D11Buffer* constantBuffer_;
    ID3D11SamplerState* sampler_;

    ConstantBufferDataStruct constantBufferDataStruct_;
};