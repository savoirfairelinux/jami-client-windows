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
    , d3dDevice_(0)
    , d3dDevContext_(0)
    , swapChain_(0)
    , renderTargetView_(0)
    , vertexBuffer_(0)
    , indiceBuffer_(0)
    , vertexShader_(0)
    , pixelShader_(0)
    , vertexShaderBlob_(0)
    , pixelShaderBlob_(0)
    , inputLayout_(0)
    , constantBuffer_(0)
    , sampler_(0)
    , shaderResourceViewY_(0)
    , shaderResourceViewU_(0)
    , shaderResourceViewV_(0)
    , texY_(0)
    , texU_(0)
    , texV_(0)
{
    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_NativeWindow, true);

    // initialize the constant buffer's value
    constantBufferDataStruct_.angleToRotate = 0.0f;
    constantBufferDataStruct_.isUsingD3D11HW = false;
    constantBufferDataStruct_.aWidthAndHeight.x = 1920;
    constantBufferDataStruct_.aWidthAndHeight.y = 1080;
    constantBufferDataStruct_.aViewPortWidthAndHeight.x = (float)width();
    constantBufferDataStruct_.aViewPortWidthAndHeight.y = (float)height();

    // initialize D3D11 entities
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

bool
D3DVideoWidgetBase::updateTextures(AVFrame* frame)
{
    if (!frame || !frame->width || !frame->height || !frame->linesize[0] || !frame->linesize[1] || !frame->linesize[2]) {
        qDebug() << "The size of this frame is not correct and frame update is abondoned!";
        return false;
    }

    double rotation = 0;
    if (auto matrix = av_frame_get_side_data(frame, AV_FRAME_DATA_DISPLAYMATRIX)) {
        const int32_t* data = reinterpret_cast<int32_t*>(matrix->data);
        rotation = av_display_rotation_get(data);
    }

    constantBufferDataStruct_.angleToRotate = rotation;
    constantBufferDataStruct_.isUsingD3D11HW = false;
    constantBufferDataStruct_.aWidthAndHeight.x = (float)frame->width;
    constantBufferDataStruct_.aWidthAndHeight.y = (float)frame->height;

    D3D11_SUBRESOURCE_DATA resourceDataTexY;
    ZeroMemory(&resourceDataTexY, sizeof(D3D11_SUBRESOURCE_DATA));
    D3D11_SUBRESOURCE_DATA resourceDataTexU;
    ZeroMemory(&resourceDataTexU, sizeof(D3D11_SUBRESOURCE_DATA));
    D3D11_SUBRESOURCE_DATA resourceDataTexV;
    ZeroMemory(&resourceDataTexV, sizeof(D3D11_SUBRESOURCE_DATA));

    if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV420P) {
        resourceDataTexY.pSysMem = frame->data[0];
        resourceDataTexY.SysMemPitch = frame->linesize[0];
        initializeTextures(texY_, shaderResourceViewY_, frame->width, frame->height, 0, resourceDataTexY);

        resourceDataTexU.pSysMem = frame->data[1];
        resourceDataTexU.SysMemPitch = frame->linesize[1];
        initializeTextures(texU_, shaderResourceViewU_, frame->width / 2, frame->height / 2, 1, resourceDataTexU);

        resourceDataTexV.pSysMem = frame->data[2];
        resourceDataTexV.SysMemPitch = frame->linesize[2];
        initializeTextures(texV_, shaderResourceViewV_, frame->width / 2, frame->height / 2, 2, resourceDataTexV);
    } else if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV422P) {
        resourceDataTexY.pSysMem = frame->data[0];
        resourceDataTexY.SysMemPitch = frame->linesize[0];
        initializeTextures(texY_, shaderResourceViewY_, frame->width, frame->height, 0, resourceDataTexY);

        resourceDataTexU.pSysMem = frame->data[1];
        resourceDataTexU.SysMemPitch = frame->linesize[1];
        initializeTextures(texU_, shaderResourceViewU_, frame->width / 2, frame->height, 1, resourceDataTexU);

        resourceDataTexV.pSysMem = frame->data[2];
        resourceDataTexV.SysMemPitch = frame->linesize[2];
        initializeTextures(texV_, shaderResourceViewV_, frame->width / 2, frame->height, 2, resourceDataTexV);
    } else if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV444P) {
        resourceDataTexY.pSysMem = frame->data[0];
        resourceDataTexY.SysMemPitch = frame->linesize[0];
        initializeTextures(texY_, shaderResourceViewY_, frame->width, frame->height, 0, resourceDataTexY);

        resourceDataTexU.pSysMem = frame->data[1];
        resourceDataTexU.SysMemPitch = frame->linesize[1];
        initializeTextures(texU_, shaderResourceViewU_, frame->width, frame->height, 1, resourceDataTexU);

        resourceDataTexV.pSysMem = frame->data[2];
        resourceDataTexV.SysMemPitch = frame->linesize[2];
        initializeTextures(texV_, shaderResourceViewV_, frame->width, frame->height, 2, resourceDataTexV);
    } else if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_D3D11VA_VLD) {
        qDebug() << "Application is now using D3D11 to accelerate but it;s not yet implemented!";
    } else if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_QSV) {
        qDebug() << "Application is now using Intel libmfx to accelerate but it;s not yet implemented!";
    } else if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_CUDA) {
        qDebug() << "Application is now using CUDA to accelerate but it;s not yet implemented!";
    }
    else if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_DXVA2_VLD) {
        qDebug() << "Application is now using D3D9 to accelerate but it;s not yet implemented!";
    } else {
        qWarning() << "The format is not supportted, please set it to YUV format in YUV420P, YUV422P or YUV444P!";
        return false;
    }

    return true;
}

void
D3DVideoWidgetBase::initialize()
{
    InitD3D();
    InitScene();
}

void
D3DVideoWidgetBase::initializeTextures(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
                                       int width, int height, int index, D3D11_SUBRESOURCE_DATA resourceData)
{
    D3D11_OBJECT_RELEASE(shaderResourceView);
    D3D11_OBJECT_RELEASE(texture);
    d3dDevContext_->Flush();

    CD3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(CD3D11_TEXTURE2D_DESC));
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.ArraySize = 1;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.MiscFlags = 0;
    textureDesc.Format = DXGI_FORMAT_R8_UNORM;

    d3dDevice_->CreateTexture2D(&textureDesc, &resourceData, texture.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

    d3dDevice_->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, shaderResourceView.GetAddressOf());
    d3dDevContext_->PSSetShaderResources(index, 1, shaderResourceView.GetAddressOf());
    d3dDevContext_->PSSetSamplers(0, 1, sampler_.GetAddressOf());
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
    Q_UNUSED(event);
    ResizeD3D();
}

void
D3DVideoWidgetBase::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
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

    ERROR_HANDLE_RUN(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL,
        D3D11_SDK_VERSION, &swapChainDesc, swapChain_.GetAddressOf(), d3dDevice_.GetAddressOf(), NULL, d3dDevContext_.GetAddressOf()), "The creation of device, context and swap chain fails");

    // Create back buffer and render target view
    ID3D11Texture2D* backBuffer;
    ERROR_HANDLE_RUN(swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer), "The creation of back buffer fails");
    ERROR_HANDLE_RUN(d3dDevice_->CreateRenderTargetView(backBuffer,NULL,renderTargetView_.GetAddressOf()), "The creation of render target view fails");
    D3D11_OBJECT_RELEASE(backBuffer);
    d3dDevContext_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), NULL);
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

    ERROR_HANDLE_RUN(d3dDevice_->CreateRenderTargetView(backBuffer, NULL, renderTargetView_.GetAddressOf()),"The creation of render target view fails when resizing");
    D3D11_OBJECT_RELEASE(backBuffer);

    //Set our Render Target
    d3dDevContext_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), NULL);

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
    constantBufferDataStruct_.aViewPortWidthAndHeight.x = (float)width();
    constantBufferDataStruct_.aViewPortWidthAndHeight.y = (float)height();
}

void
D3DVideoWidgetBase::InitScene()
{
    // compile shaders and set shader byte code blob
    Microsoft::WRL::ComPtr<ID3DBlob> VS_ErrorMsg;
    Microsoft::WRL::ComPtr<ID3DBlob> PS_ErrorMsg;

    ERROR_HANDLE_RUN(D3DCompileFromFile(L"shaders/ShaderJami.hlsl", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_4_0", 0, 0, vertexShaderBlob_.GetAddressOf(), VS_ErrorMsg.GetAddressOf()),"Vertex shader compile fails!");
    if(VS_ErrorMsg) {
        std::string error;
        error.assign((char*)VS_ErrorMsg->GetBufferPointer(), VS_ErrorMsg->GetBufferSize());
        qDebug() << QString::fromStdString(error);
        D3D11_OBJECT_RELEASE(VS_ErrorMsg);
    }
    ERROR_HANDLE_RUN(D3DCompileFromFile(L"shaders/ShaderJami.hlsl", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_4_0", 0, 0, pixelShaderBlob_.GetAddressOf(), PS_ErrorMsg.GetAddressOf()),"Pixel shader compile fails!");
    if (PS_ErrorMsg) {
        std::string error;
        error.assign((char*)PS_ErrorMsg->GetBufferPointer(), VS_ErrorMsg->GetBufferSize());
        qDebug() << QString::fromStdString(error);
        D3D11_OBJECT_RELEASE(PS_ErrorMsg);
    }

    ERROR_HANDLE_RUN(d3dDevice_->CreateVertexShader(vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize(), NULL, vertexShader_.GetAddressOf()), "Vertex Shader creation fails");
    ERROR_HANDLE_RUN(d3dDevice_->CreatePixelShader(pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize(), NULL, pixelShader_.GetAddressOf()), "Pixel Shader creation fails");

    d3dDevContext_->VSSetShader(vertexShader_.Get(), 0, 0);
    d3dDevContext_->PSSetShader(pixelShader_.Get(), 0, 0);
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

    ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&indexBufferDesc, &indexData, indiceBuffer_.GetAddressOf()), "The creation of index buffer fails");
    d3dDevContext_->IASetIndexBuffer(indiceBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

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

    ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer_.GetAddressOf()), "The creation of vertex buffer fails");

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3dDevContext_->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

    // set input layout
    d3dDevice_->CreateInputLayout(&input_layout_description.front(), input_layout_description.size(),
                                  vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize(), inputLayout_.GetAddressOf());
    d3dDevContext_->IASetInputLayout(inputLayout_.Get());

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

    // initialize sampler
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    d3dDevice_->CreateSamplerState(&samplerDesc, sampler_.GetAddressOf());

    // set constant buffer
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.ByteWidth = sizeof(ConstantBufferDataStruct) % 16 == 0 ? sizeof(ConstantBufferDataStruct) : 16 * (1 + (UINT)(sizeof(ConstantBufferDataStruct) / 16));
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&constantBufferDesc, NULL, constantBuffer_.GetAddressOf()), "The creation of constant buffer fails!");;

    // clear the background
    float bgColor[4] = { (0.0f,0.0f,0.0f,1.0f) };
    d3dDevContext_->ClearRenderTargetView(renderTargetView_.Get(), bgColor);
}

void
D3DVideoWidgetBase::paintD3D()
{
    //Clear our backbuffer
    float bgColor[4] = { (0.0f,0.0f,0.0f,0.0f) };
    try {
        d3dDevContext_->ClearRenderTargetView(renderTargetView_.Get(), bgColor);
        d3dDevContext_->UpdateSubresource(constantBuffer_.Get(), 0, NULL, &constantBufferDataStruct_, 0, 0);
        d3dDevContext_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
    }
    catch(...) {
        qDebug() << "The constant buffer is not set right or does not exist!";
    };

    d3dDevContext_->DrawIndexed(6, 0, 0);
    swapChain_->Present(0, 0),"Swap chain swap buffer fails!";
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
    D3D11_OBJECT_RELEASE(sampler_);
    D3D11_OBJECT_RELEASE(shaderResourceViewY_);
    D3D11_OBJECT_RELEASE(shaderResourceViewU_);
    D3D11_OBJECT_RELEASE(shaderResourceViewV_);
    D3D11_OBJECT_RELEASE(texY_);
    D3D11_OBJECT_RELEASE(texU_);
    D3D11_OBJECT_RELEASE(texV_);
}
