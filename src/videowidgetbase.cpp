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

#include "videowidgetbase.h"

#include "lrcinstance.h"

VideoWidgetBase::VideoWidgetBase(QColor bgColor, QWidget* parent)
    : QWidget(parent)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, bgColor);
    setAutoFillBackground(true);
    setPalette(pal);
}

VideoWidgetBase::~VideoWidgetBase()
{}

void
VideoWidgetBase::forceRepaint()
{
    auto parent = qobject_cast<QWidget*>(this->parent());
    if (parent) parent->setUpdatesEnabled(false);
    repaint();
    if (parent) parent->setUpdatesEnabled(true);
}

void
VideoWidgetBase::hideEvent(QHideEvent* e)
{
    Q_UNUSED(e);
    emit visibilityChanged(false);
}

void
VideoWidgetBase::showEvent(QShowEvent* e)
{
    Q_UNUSED(e);
    emit visibilityChanged(true);
}

D3DVideoWidgetBase::D3DVideoWidgetBase(QColor bgColor, QWidget* parent)
    : QWidget(parent)
    , d3dDevice_(nullptr)
    , d3dDevContext_(nullptr)
    , swapChain_(nullptr)
    , renderTargetView_(nullptr)
    , squareVertexBuffer_(nullptr)
    , squareIndiceBuffer_(nullptr)
    , vertexShader_(nullptr)
    , pixelShader_(nullptr)
    , vertexShaderBlob_(nullptr)
    , pixelShaderBlob_(nullptr)
    , inputLayout_(nullptr)
    , constantBuffer_(nullptr)
{
    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_NativeWindow, true);

    initialize();
}

D3DVideoWidgetBase::~D3DVideoWidgetBase()
{
    CleanUp();
}

void
D3DVideoWidgetBase::forceRepaint()
{
    auto parent = qobject_cast<QWidget*>(this->parent());
    if (parent) parent->setUpdatesEnabled(false);
    repaint();
    if (parent) parent->setUpdatesEnabled(true);
}

void
D3DVideoWidgetBase::initialize()
{
    InitD3D();
    InitScene();
}

void
D3DVideoWidgetBase::hideEvent(QHideEvent* e)
{
    Q_UNUSED(e);
    emit visibilityChanged(false);
}

void
D3DVideoWidgetBase::showEvent(QShowEvent* e)
{
    Q_UNUSED(e);
    emit visibilityChanged(true);
}

void
D3DVideoWidgetBase::resizeEvent(QResizeEvent* event)
{
    ResizeD3D();
}

void
D3DVideoWidgetBase::paintEvent(QPaintEvent* event)
{
    paintD3D();
}

void
D3DVideoWidgetBase::InitD3D()
{
    HRESULT hr;

    //Describe our Buffer
    DXGI_MODE_DESC bufferDesc;

    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = width();
    bufferDesc.Height = height();
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //Describe our SwapChain
    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = (HWND)this->winId();
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
                                       D3D11_SDK_VERSION, &swapChainDesc, &swapChain_, &d3dDevice_, NULL, &d3dDevContext_);
    if(FAILED(hr)) {
        throw std::exception("The creation of device, context and swap chain fails");
    }

    // Create back buffer and render target view
    ID3D11Texture2D* backBuffer;
    hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer);
    hr = d3dDevice_->CreateRenderTargetView(backBuffer,NULL,&renderTargetView_);
    D3D11_OBJECT_RELEASE(backBuffer);
    if (FAILED(hr)) {
        throw std::exception("The creation of render target view fails");
    }

    d3dDevContext_->OMSetRenderTargets(1, &renderTargetView_, NULL);
}

void
D3DVideoWidgetBase::ResizeD3D()
{
    D3D11_OBJECT_RELEASE(renderTargetView_);

    HRESULT hr;
    swapChain_->ResizeBuffers(1, width(), height(), DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    //Create our BackBuffer
    ID3D11Texture2D* backBuffer;
    swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    //Create our Render Target
    hr = d3dDevice_->CreateRenderTargetView(backBuffer, NULL, &renderTargetView_);
    D3D11_OBJECT_RELEASE(backBuffer);

    //Set our Render Target
    d3dDevContext_->OMSetRenderTargets(1, &renderTargetView_, NULL);

    //Create the Viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width();
    viewport.Height = height();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //Set the Viewport
    d3dDevContext_->RSSetViewports(1, &viewport);
}

void
D3DVideoWidgetBase::InitScene()
{

}

void
D3DVideoWidgetBase::paintD3D()
{
}

void
D3DVideoWidgetBase::CleanUp()
{
    D3D11_OBJECT_RELEASE(d3dDevice_);
    D3D11_OBJECT_RELEASE(d3dDevContext_);
    D3D11_OBJECT_RELEASE(swapChain_);
    D3D11_OBJECT_RELEASE(renderTargetView_);
    D3D11_OBJECT_RELEASE(squareVertexBuffer_);
    D3D11_OBJECT_RELEASE(squareIndiceBuffer_);
    D3D11_OBJECT_RELEASE(vertexShader_);
    D3D11_OBJECT_RELEASE(pixelShader_);
    D3D11_OBJECT_RELEASE(vertexShaderBlob_);
    D3D11_OBJECT_RELEASE(pixelShaderBlob_);
    D3D11_OBJECT_RELEASE(inputLayout_);
    D3D11_OBJECT_RELEASE(constantBuffer_);
}
