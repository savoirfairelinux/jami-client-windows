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
    , vertexBuffer_(nullptr)
    , indiceBuffer_(nullptr)
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
    update();
}

void
D3DVideoWidgetBase::paintEvent(QPaintEvent* event)
{
    paintD3D();
}

void
D3DVideoWidgetBase::InitD3D()
{
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

    ERROR_HANDLE_RUN(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
        D3D11_SDK_VERSION, &swapChainDesc, &swapChain_, &d3dDevice_, NULL, &d3dDevContext_), "The creation of device, context and swap chain fails");

    // Create back buffer and render target view
    ID3D11Texture2D* backBuffer;
    ERROR_HANDLE_RUN(swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer), "The creation of back buffer fails");
    ERROR_HANDLE_RUN(d3dDevice_->CreateRenderTargetView(backBuffer,NULL,&renderTargetView_), "The creation of render target view fails");
    D3D11_OBJECT_RELEASE(backBuffer);
    d3dDevContext_->OMSetRenderTargets(1, &renderTargetView_, NULL);
}

void
D3DVideoWidgetBase::ResizeD3D()
{
    D3D11_OBJECT_RELEASE(renderTargetView_);

    swapChain_->ResizeBuffers(1, width(), height(), DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    //Create our BackBuffer
    ID3D11Texture2D* backBuffer;
    swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    //Create our Render Target
    ERROR_HANDLE_RUN(d3dDevice_->CreateRenderTargetView(backBuffer, NULL, &renderTargetView_),"The creation of render target view fails when resizing");
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
    // compile shaders and set shader byte code blob
    ID3DBlob* VS_ErrorMsg;
    ID3DBlob* PS_ErrorMsg;

    ERROR_HANDLE_RUN(D3DCompileFromFile(L"Effects.fx", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_4_0", 0, 0, &vertexShaderBlob_, &VS_ErrorMsg),"Vertex shader compile fails!");
    if(VS_ErrorMsg) {
        qDebug() << QString::fromRawData((QChar*)VS_ErrorMsg->GetBufferPointer(), VS_ErrorMsg->GetBufferSize());
    }
    ERROR_HANDLE_RUN(D3DCompileFromFile(L"Effects.fx", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_4_0", 0, 0, &pixelShaderBlob_, &PS_ErrorMsg),"Pixel shader compile fails!");
    if (PS_ErrorMsg) {
        qDebug() << QString::fromRawData((QChar*)VS_ErrorMsg->GetBufferPointer(), VS_ErrorMsg->GetBufferSize());
    }
    ERROR_HANDLE_RUN(d3dDevice_->CreateVertexShader(vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize(), NULL, &vertexShader_), "Vertex Shader creation fails");
    ERROR_HANDLE_RUN(d3dDevice_->CreatePixelShader(pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize(), NULL, &pixelShader_), "Pixel Shader creation fails");

    d3dDevContext_->VSSetShader(vertexShader_, 0, 0);
    d3dDevContext_->PSSetShader(pixelShader_, 0, 0);
    // set index buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 6;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData;
    ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
    indexData.pSysMem = indices;

    ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&indexBufferDesc, &indexData, &indiceBuffer_), "The creation of index buffer fails");
    d3dDevContext_->IASetIndexBuffer(indiceBuffer_, DXGI_FORMAT_R32_UINT, 0);

    // set vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData;
    ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
    vertexData.pSysMem = vertexArray;

    ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer_), "The creation of vertex buffer fails");

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3dDevContext_->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);

    // set input layout
    d3dDevice_->CreateInputLayout(&input_layout_description.front(), input_layout_description.size(),
                                  vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize(), &inputLayout_);
    d3dDevContext_->IASetInputLayout(inputLayout_);

    // set primitive Topology
    d3dDevContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // set viewport
    D3D11_VIEWPORT viewPort;
    ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));

    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = width();
    viewPort.Height = height();
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;

    d3dDevContext_->RSSetViewports(1, &viewPort);

    // set constant buffer
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.ByteWidth = sizeof(ConstantBufferDataStruct);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.MiscFlags = 0;

    ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&constantBufferDesc, NULL, &constantBuffer_), "The creation of constant buffer fails!");
}

void
D3DVideoWidgetBase::paintD3D()
{
    float rotation = 90.f;
    constantBufferDataStruct_.angleToRotate = rotation;
    d3dDevContext_->UpdateSubresource(constantBuffer_, 0, NULL, &constantBufferDataStruct_, 0, 0);
    d3dDevContext_->CSSetConstantBuffers(0, 1, &constantBuffer_);

    d3dDevContext_->DrawIndexed(6, 0, 0);
    ERROR_HANDLE_RUN(swapChain_->Present(0, 0),"Swap chain swap buffer fails!");
}

void
D3DVideoWidgetBase::CleanUp()
{
    D3D11_OBJECT_RELEASE(d3dDevice_);
    D3D11_OBJECT_RELEASE(d3dDevContext_);
    D3D11_OBJECT_RELEASE(swapChain_);
    D3D11_OBJECT_RELEASE(renderTargetView_);
    D3D11_OBJECT_RELEASE(vertexBuffer_);
    D3D11_OBJECT_RELEASE(indiceBuffer_);
    D3D11_OBJECT_RELEASE(vertexShader_);
    D3D11_OBJECT_RELEASE(pixelShader_);
    D3D11_OBJECT_RELEASE(vertexShaderBlob_);
    D3D11_OBJECT_RELEASE(pixelShaderBlob_);
    D3D11_OBJECT_RELEASE(inputLayout_);
    D3D11_OBJECT_RELEASE(constantBuffer_);
}
