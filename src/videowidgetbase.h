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

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define D3D11_OBJECT_RELEASE(p) {if(p){(p)->Release(); p=0;}}

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

signals:
    void visibilityChanged(bool visible);

private:
    void initialize();

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

protected:
    ID3D11Device* d3dDevice_;
    ID3D11DeviceContext* d3dDevContext_;
    IDXGISwapChain* swapChain_;
    ID3D11DepthStencilView* depthStencilView_;
    ID3D11RenderTargetView* renderTargetView_;

    ID3D11Buffer* squareVertexBuffer_;
    ID3D11Buffer* squareIndiceBuffer_;
    ID3D11VertexShader* vertexShader_;
    ID3D11PixelShader* pixelShader;
    ID3D10Blob* vertexShaderBlob_;
    ID3D10Blob* pixelShader_;
    ID3D11InputLayout* inputLayout_;
    ID3D11Buffer* vbo_;

};