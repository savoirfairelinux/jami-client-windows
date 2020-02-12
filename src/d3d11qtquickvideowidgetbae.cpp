#include <QRunnable>

#include "lrcinstance.h"

#include "d3d11qtquickvideowidgetbae.h"

D3D11Renderer::D3D11Renderer(QObject* parent)
    : //QObject(parent)
    //,
    d3dDevice_(0)
    , d3dDevContext_(0)
    , rasterState_(0)
    , blendState_(0)
    , depthStencileState_(0)
    , vertexBuffer_(0)
    , indiceBuffer_(0)
    , vertexShader_(0)
    , pixelShader_(0)
    , sampler_(0)
    , shaderResourceViewY_(0)
    , shaderResourceViewU_(0)
    , shaderResourceViewV_(0)
    , shaderResourceViewNV12_(0)
    , texY_(0)
    , texU_(0)
    , texV_(0)
    , texUV_NV12_(0)
    , inputLayout_(0)
    , constantBuffer_(0)
{
    D3D11QtQuickVideoWidgetBase* parentItem = qobject_cast<D3D11QtQuickVideoWidgetBase*>(parent);
    if (parentItem) {
        setParantingQuickItem(parentItem);
        setWindow(parentItem->window());
    }
    constantBufferDataStruct_.boarderRadiusNormalizedShortSide = 0.0f;
}

D3D11Renderer*
D3D11Renderer::createD3d11RendererFactory(D3D11QtQuickVideoWidgetBase * parent, float borderRadiusboarderRadiusNormalizedShortSide, int texYIndex)
{
    D3D11Renderer * renderer = new D3D11Renderer(parent);
    renderer->texYIndex_ = texYIndex;

    return renderer;
}

D3D11Renderer::~D3D11Renderer()
{
    cleanUp();
}

void
D3D11Renderer::frameStart()
{
    QSGRendererInterface* rif = m_window->rendererInterface();

    // D3D11's RHI backend is the only backend that we use
    Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::Direct3D11Rhi);

    // set up the device and device context when the frame starts according to the window provided
    d3dDevice_ = reinterpret_cast<ID3D11Device*>(rif->getResource(m_window, QSGRendererInterface::DeviceResource));
    Q_ASSERT(d3dDevice_);
    d3dDevContext_ = reinterpret_cast<ID3D11DeviceContext*>(rif->getResource(m_window, QSGRendererInterface::DeviceContextResource));
    Q_ASSERT(d3dDevContext_);

    // prepare shaders and store the code into QByteArray if it is empty
    if (m_vert.isEmpty())
        prepareShader(VertexStage);
    if (m_frag.isEmpty())
        prepareShader(FragmentStage);

    // if it's not yet initialized, call the initialize function
    if (!m_initialized)
        init();

    emit frameStartSlotFinished();
}

void
D3D11Renderer::setParantingQuickItem(D3D11QtQuickVideoWidgetBase* parent)
{
    m_parantingItem = parent;
}

bool
D3D11Renderer::updateTextures(AVFrame* frame)
{
    bool result = false;
    if (!frame || !frame->width || !frame->height) {
        qDebug() << "The size of this frame is not correct and frame update is abondoned!";
        return false;
    }

    constantBufferDataStruct_.angleToRotate = getAngleToRotate(frame);
    constantBufferDataStruct_.aWidthAndHeight.x = (float)frame->width;
    constantBufferDataStruct_.aWidthAndHeight.y = (float)frame->height;

    D3D11_SUBRESOURCE_DATA resourceDataTexY;
    memset(&resourceDataTexY, 0, sizeof(D3D11_SUBRESOURCE_DATA));
    D3D11_SUBRESOURCE_DATA resourceDataTexU;
    memset(&resourceDataTexU, 0, sizeof(D3D11_SUBRESOURCE_DATA));
    D3D11_SUBRESOURCE_DATA resourceDataTexV;
    memset(&resourceDataTexV, 0, sizeof(D3D11_SUBRESOURCE_DATA));

    // set a bool variable. If it's true, there's no need to recreate the texture. If it's false, the textures needs to be recreated
    bool updateOrRecreate = areTexturesToBeUpdatedOrRecreated(frame);
    updateLastFrameParameters(frame);

    if (frame->linesize[0] && frame->linesize[1] && frame->linesize[2]) {
        constantBufferDataStruct_.isNV12 = false;
        // when not using hardware acceleration, it's planer YUV, and the linesize of all three channels must not be zero
        if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV420P) {
            resourceDataTexY.pSysMem = frame->data[0];
            resourceDataTexY.SysMemPitch = frame->linesize[0];
            initializeTexture(texY_, shaderResourceViewY_, updateOrRecreate, frame->width, frame->height, texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexY);

            resourceDataTexU.pSysMem = frame->data[1];
            resourceDataTexU.SysMemPitch = frame->linesize[1];
            initializeTexture(texU_, shaderResourceViewU_, updateOrRecreate, frame->width / 2, frame->height / 2, offset_TexUIndex_ + texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexU);

            resourceDataTexV.pSysMem = frame->data[2];
            resourceDataTexV.SysMemPitch = frame->linesize[2];
            initializeTexture(texV_, shaderResourceViewV_, updateOrRecreate, frame->width / 2, frame->height / 2, offset_TexVIndex_ + texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexV);
            result = true;
        } else if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV422P) {
            resourceDataTexY.pSysMem = frame->data[0];
            resourceDataTexY.SysMemPitch = frame->linesize[0];
            initializeTexture(texY_, shaderResourceViewY_, updateOrRecreate, frame->width, frame->height, texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexY);

            resourceDataTexU.pSysMem = frame->data[1];
            resourceDataTexU.SysMemPitch = frame->linesize[1];
            initializeTexture(texU_, shaderResourceViewU_, updateOrRecreate, frame->width / 2, frame->height, offset_TexUIndex_ + texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexU);

            resourceDataTexV.pSysMem = frame->data[2];
            resourceDataTexV.SysMemPitch = frame->linesize[2];
            initializeTexture(texV_, shaderResourceViewV_, updateOrRecreate, frame->width / 2, frame->height, offset_TexVIndex_ + texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexV);
            result = true;
        } else if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV444P) {
            resourceDataTexY.pSysMem = frame->data[0];
            resourceDataTexY.SysMemPitch = frame->linesize[0];
            initializeTexture(texY_, shaderResourceViewY_, updateOrRecreate, frame->width, frame->height, texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexY);

            resourceDataTexU.pSysMem = frame->data[1];
            resourceDataTexU.SysMemPitch = frame->linesize[1];
            initializeTexture(texU_, shaderResourceViewU_, updateOrRecreate, frame->width, frame->height, offset_TexUIndex_ + texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexU);

            resourceDataTexV.pSysMem = frame->data[2];
            resourceDataTexV.SysMemPitch = frame->linesize[2];
            initializeTexture(texV_, shaderResourceViewV_, updateOrRecreate, frame->width, frame->height, offset_TexVIndex_ + texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexV);
            result = true;
        }
    }

    if (frame->linesize[0] && frame->linesize[1]) {
        // the format is NV12
        if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_NV12) {
            resourceDataTexY.pSysMem = frame->data[0];
            resourceDataTexY.SysMemPitch = frame->linesize[0];
            initializeTexture(texY_, shaderResourceViewY_, updateOrRecreate, frame->width, frame->height, texYIndex_, DXGI_FORMAT_R8_UNORM, resourceDataTexY);

            D3D11_SUBRESOURCE_DATA resourceDataTexUV;
            memset(&resourceDataTexUV, 0, sizeof(D3D11_SUBRESOURCE_DATA));
            resourceDataTexUV.pSysMem = frame->data[1];
            resourceDataTexUV.SysMemPitch = frame->linesize[1];
            initializeTexture(texUV_NV12_, shaderResourceViewNV12_, updateOrRecreate, frame->width, frame->height / 2, offset_TexUV_NV12_Index_ + texYIndex_, DXGI_FORMAT_R8G8_UNORM, resourceDataTexUV);

            constantBufferDataStruct_.isNV12 = true;
            result = true;
        }
    }

    if (frame->linesize[0] && frame->linesize[1]) {
        // all hardware accelerated formats are NV12
        if (AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_CUDA) {
            // support the CUDA hardware accel frame
            result = updateTextureFromCUDA(frame, updateOrRecreate);
        } else {
            result = false;
        }
    }

    return result;
}

void
D3D11Renderer::cleanTextures()
{
    D3D11_OBJECT_RESET(shaderResourceViewY_);
    D3D11_OBJECT_RESET(shaderResourceViewU_);
    D3D11_OBJECT_RESET(shaderResourceViewV_);
    D3D11_OBJECT_RESET(shaderResourceViewNV12_);

    D3D11_OBJECT_RESET(texY_);
    D3D11_OBJECT_RESET(texU_);
    D3D11_OBJECT_RESET(texV_);
    D3D11_OBJECT_RESET(texUV_NV12_);

    if (d3dDevContext_) {
        d3dDevContext_->Flush();
    }

    reInitializeLastFrameParameters();
}

double
D3D11Renderer::getAngleToRotate(AVFrame * frame)
{
    double rotation = 0;
    try {
        if ( auto matrix = av_frame_get_side_data(frame, AV_FRAME_DATA_DISPLAYMATRIX) ) {
            const int32_t * data = reinterpret_cast<int32_t *>(matrix->data);
            rotation = av_display_rotation_get(data);
        }
    } catch ( ... ) {}

    return -rotation;
}

void
D3D11Renderer::initializeShaderResourceViewWithTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView, int index)
{
    D3D11_OBJECT_RESET(shaderResourceView);

    CD3D11_TEXTURE2D_DESC textureDesc;
    if(texture) texture->GetDesc(&textureDesc);
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    memset(&shaderResourceViewDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

    if(d3dDevice_) d3dDevice_->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, shaderResourceView.GetAddressOf());
    if (d3dDevContext_) d3dDevContext_->PSSetShaderResources(index, 1, shaderResourceView.GetAddressOf());
}

void
D3D11Renderer::initializeTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
                                 bool isTextureToBeUpdatedOrRecreated, int width, int height, int index, DXGI_FORMAT textureFormat, D3D11_SUBRESOURCE_DATA resourceData)
{
    if (isTextureToBeUpdatedOrRecreated && (texture.Get() != nullptr)) {
        if (d3dDevContext_) d3dDevContext_->UpdateSubresource(texture.Get(), D3D11CalcSubresource(0, 0, 1), NULL, resourceData.pSysMem, resourceData.SysMemPitch, 0);
    } else {
        D3D11_OBJECT_RESET(texture);

        if (d3dDevContext_) {
            d3dDevContext_->Flush();
        }

        CD3D11_TEXTURE2D_DESC textureDesc;
        memset(&textureDesc, 0, sizeof(CD3D11_TEXTURE2D_DESC));
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.ArraySize = 1;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.MiscFlags = 0;
        textureDesc.Format = textureFormat;

        if (d3dDevice_) d3dDevice_->CreateTexture2D(&textureDesc, &resourceData, texture.GetAddressOf());
    }
    initializeShaderResourceViewWithTexture(texture, shaderResourceView, index);

    if (d3dDevContext_) d3dDevContext_->PSSetSamplers(0, 1, sampler_.GetAddressOf());
}

void D3D11Renderer::initializeTextureWithCUDA(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
                                              bool isTextureToBeUpdatedOrRecreated, int width, int height, int lineSize, int index, DXGI_FORMAT textureFormat,
                                              CUdeviceptr startPtr, CUcontext cudaContext)
{
    D3D11_OBJECT_RESET(texture);

    if (d3dDevContext_) {
        d3dDevContext_->Flush();
    }

    CD3D11_TEXTURE2D_DESC textureDesc;
    memset(&textureDesc, 0, sizeof(CD3D11_TEXTURE2D_DESC));
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
    textureDesc.Format = textureFormat;

    if (d3dDevice_) d3dDevice_->CreateTexture2D(&textureDesc, NULL, texture.GetAddressOf());

    CUcontext pctx;
    cuCtxGetCurrent(&pctx);
    if (pctx != cudaContext) cuCtxPushCurrent(cudaContext);
    // copy the frame to cuda graphic resource
    CUgraphicsResource cudaResourceForCurrentTexture;
    cuGraphicsD3D11RegisterResource(&cudaResourceForCurrentTexture, texture.Get(), CU_GRAPHICS_REGISTER_FLAGS_NONE);
    cuGraphicsResourceSetMapFlags(cudaResourceForCurrentTexture, CU_GRAPHICS_MAP_RESOURCE_FLAGS_WRITE_DISCARD);

    cuGraphicsMapResources(1, &cudaResourceForCurrentTexture, 0);
    CUarray dstArray;
    cuGraphicsSubResourceGetMappedArray(&dstArray, cudaResourceForCurrentTexture, 0, 0);
    CUDA_MEMCPY2D memCopyParameter;
    memset(&memCopyParameter, 0, sizeof(memCopyParameter));
    memCopyParameter.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    memCopyParameter.srcDevice = startPtr;
    memCopyParameter.srcPitch = lineSize * sizeof(uint8_t);
    memCopyParameter.dstMemoryType = CU_MEMORYTYPE_ARRAY;
    memCopyParameter.dstArray = dstArray;
    memCopyParameter.WidthInBytes = width * sizeof(uint8_t);
    memCopyParameter.Height = height;
    cuMemcpy2D(&memCopyParameter);

    cuGraphicsUnmapResources(1, &cudaResourceForCurrentTexture, 0);
    cuGraphicsUnregisterResource(cudaResourceForCurrentTexture);

    initializeShaderResourceViewWithTexture(texture, shaderResourceView, index);
    if (d3dDevContext_) d3dDevContext_->PSSetSamplers(0, 1, sampler_.GetAddressOf());
}

bool
D3D11Renderer::updateTextureFromCUDA(AVFrame* frame, bool isTextureToBeUpdatedOrRecreated)
{
    bool result = false;
    if ((AVPixelFormat)frame->format != AVPixelFormat::AV_PIX_FMT_CUDA) return false;
    // get the cuda context
    CUcontext cudaContext;

    AVHWFramesContext* avHWFramesContext = (AVHWFramesContext*)(frame->hw_frames_ctx->data);
    if (!avHWFramesContext) return false;
    auto device_ctx = static_cast<AVHWDeviceContext*>(avHWFramesContext->device_ctx);
    if (!device_ctx || !static_cast<AVCUDADeviceContext*>(device_ctx->hwctx)) return false;
    AVCUDADeviceContext* avCUDADeviceContext = static_cast<AVCUDADeviceContext*>(device_ctx->hwctx);
    if (!avCUDADeviceContext) return false;
    cudaContext = avCUDADeviceContext->cuda_ctx;
    if (!cudaContext) return false;

    if ((AVPixelFormat)avHWFramesContext->sw_format == AVPixelFormat::AV_PIX_FMT_NV12) {
        CUdeviceptr pointerToDataBufferY = (CUdeviceptr)frame->data[0];
        initializeTextureWithCUDA(texY_, shaderResourceViewY_, isTextureToBeUpdatedOrRecreated, frame->width, frame->height, frame->linesize[0],
            texYIndex_, DXGI_FORMAT_R8_UNORM, pointerToDataBufferY, cudaContext);
        CUdeviceptr pointerToDataBufferUV = (CUdeviceptr)frame->data[1];
        initializeTextureWithCUDA(texUV_NV12_, shaderResourceViewNV12_, isTextureToBeUpdatedOrRecreated, frame->width, frame->height / 2, frame->linesize[1],
            texYIndex_ + offset_TexUV_NV12_Index_, DXGI_FORMAT_R8G8_UNORM, pointerToDataBufferUV, cudaContext);

        constantBufferDataStruct_.isNV12 = true;
        result = true;
    }

    return result;
}

void
D3D11Renderer::reInitializeLastFrameParameters()
{
    lastFrameParameters_.LastFormat = AV_PIX_FMT_NONE;
    lastFrameParameters_.LastWidth = 0;
    lastFrameParameters_.LastHeight = 0;
}

void
D3D11Renderer::updateLastFrameParameters(AVFrame* frame)
{
    lastFrameParameters_.LastFormat = (AVPixelFormat)frame->format;
    lastFrameParameters_.LastWidth = frame->width;
    lastFrameParameters_.LastHeight = frame->height;
}

bool
D3D11Renderer::areTexturesToBeUpdatedOrRecreated(AVFrame* frame)
{
    bool result = true;
    result &= (frame->format == lastFrameParameters_.LastFormat);
    result &= (frame->width == lastFrameParameters_.LastWidth);
    result &= (frame->height == lastFrameParameters_.LastHeight);

    return result;
}

void
D3D11Renderer::mainPassRecordingStart()
{
    // rendering function
    m_window->beginExternalCommands();

    // update viewport
    D3D11_VIEWPORT viewPort;
    memset(&viewPort, 0, sizeof(D3D11_VIEWPORT));

    viewPort.TopLeftX = m_parantingItem->x();
    viewPort.TopLeftY = m_parantingItem->y();
    viewPort.Width = m_parantingItem->width() * m_window->devicePixelRatio();
    viewPort.Height = m_parantingItem->height() * m_window->devicePixelRatio();
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;

    if (d3dDevContext_) {
    constantBufferDataStruct_.aViewPortWidthAndHeight.x = m_parantingItem->width();
    constantBufferDataStruct_.aViewPortWidthAndHeight.y = m_parantingItem->height();

    try {
        // update the data in constant buffer
        d3dDevContext_->UpdateSubresource(constantBuffer_.Get(), 0, NULL, &constantBufferDataStruct_, 0, 0);
        d3dDevContext_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());

    } catch (...) {
        qFatal("The constant buffer is not set right or does not exist!");
    };

    d3dDevContext_->RSSetViewports(1, &viewPort);

    // add the related resources to the pipline, including the shaders, primitive topology, input layout,and textures
    d3dDevContext_->VSSetShader(vertexShader_.Get(), 0, 0);
    d3dDevContext_->PSSetShader(pixelShader_.Get(), 0, 0);
    d3dDevContext_->IASetIndexBuffer(indiceBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3dDevContext_->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    d3dDevContext_->IASetInputLayout(inputLayout_.Get());
    d3dDevContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    d3dDevContext_->OMSetDepthStencilState(depthStencileState_.Get(), 0);
    float blendConstants[] = { 1, 1, 1, 1 };
    d3dDevContext_->OMSetBlendState(blendState_.Get(), blendConstants, 0xFFFFFFFF);
    d3dDevContext_->RSSetState(rasterState_.Get());

    d3dDevContext_->DrawIndexed(6, 0, 0);
    }

    m_window->endExternalCommands();

    emit mainPassRecordingStartSlotFinished();
}

void
D3D11Renderer::prepareShader(Stage stage)
{
    QString filename;

    filename = QLatin1String(":/src-gpu/ShaderJami.hlsl");

    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
        qFatal("Failed to read shader %s", qPrintable(filename));

    const QByteArray contents = f.readAll();

    if (stage == VertexStage) {
        m_vert = contents;
        Q_ASSERT(!m_vert.isEmpty());
        m_vertEntryPoint = QByteArrayLiteral("VS_main");
    } else {
        m_frag = contents;
        Q_ASSERT(!m_frag.isEmpty());
        m_fragEntryPoint = QByteArrayLiteral("PS_main");
    }
}

QByteArray
D3D11Renderer::compileShader(Stage stage, const QByteArray& source, const QByteArray& entryPoint)
{
    const char* target;
    switch (stage) {
    case VertexStage:
        target = "vs_5_0";
        break;
    case FragmentStage:
        target = "ps_5_0";
        break;
    default:
        qFatal("Unknown shader stage %d", stage);
        return QByteArray();
    }

    ID3DBlob* bytecode = nullptr;
    ID3DBlob* errors = nullptr;
    HRESULT hr = D3DCompile(source.constData(), source.size(),
        nullptr, nullptr, nullptr,
        entryPoint.constData(), target, 0, 0, &bytecode, &errors);
    if (FAILED(hr) || !bytecode) {
        qWarning("HLSL shader compilation failed: 0x%x", uint(hr));
        if (errors) {
            const QByteArray msg(static_cast<const char*>(errors->GetBufferPointer()),
                errors->GetBufferSize());
            errors->Release();
            qWarning("%s", msg.constData());
        }
        return QByteArray();
    }

    QByteArray result;
    result.resize(bytecode->GetBufferSize());
    memcpy(result.data(), bytecode->GetBufferPointer(), result.size());
    bytecode->Release();

    return result;
}

void
D3D11Renderer::init()
{
    qDebug("D3D Initilize");
    m_initialized = true;

    const QByteArray vs = compileShader(VertexStage, m_vert, m_vertEntryPoint);
    const QByteArray ps = compileShader(FragmentStage, m_frag, m_fragEntryPoint);

    if (d3dDevice_) {
        HRESULT hr = d3dDevice_->CreateVertexShader(vs.constData(), vs.size(), nullptr, vertexShader_.GetAddressOf());
        if (FAILED(hr)) qFatal("Failed to create vertex shader: 0x%x", hr);

        hr = d3dDevice_->CreatePixelShader(ps.constData(), ps.size(), nullptr, pixelShader_.GetAddressOf());
        if (FAILED(hr)) qFatal("Failed to create pixel shader: 0x%x", hr);
    } else {
        qWarning() << "The device context does not exist and cannot compile shaders";
    }

    // create index buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    memset(&indexBufferDesc, 0,sizeof(D3D11_BUFFER_DESC));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 6;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData;
    memset(&indexData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
    indexData.pSysMem = indices;

    if (d3dDevice_) ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&indexBufferDesc, &indexData, indiceBuffer_.GetAddressOf()), "The creation of index buffer fails");

    // create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    memset(&vertexBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData;
    memset(&vertexData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
    vertexData.pSysMem = vertexArray;

    if (d3dDevice_) ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer_.GetAddressOf()), "The creation of vertex buffer fails");

    // create input layout
    if (d3dDevice_) d3dDevice_->CreateInputLayout(&input_layout_description.front(), input_layout_description.size(),
                                  vs.constData(), vs.size(), inputLayout_.GetAddressOf());

    // create constant buffer
    D3D11_BUFFER_DESC constantBufferDesc;
    memset(&constantBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.ByteWidth = sizeof(ConstantBufferDataStruct) % 16 == 0 ? sizeof(ConstantBufferDataStruct) : 16 * (1 + (UINT)(sizeof(ConstantBufferDataStruct) / 16));
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    if (d3dDevice_) ERROR_HANDLE_RUN(d3dDevice_->CreateBuffer(&constantBufferDesc, NULL, constantBuffer_.GetAddressOf()), "The creation of constant buffer fails!");

    // create sampler
    D3D11_SAMPLER_DESC samplerDesc;
    memset(&samplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
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

    if (d3dDevice_) d3dDevice_->CreateSamplerState(&samplerDesc, sampler_.GetAddressOf());

    //create rasterizer state
    D3D11_RASTERIZER_DESC rastDesc;
    memset(&rastDesc, 0, sizeof(rastDesc));
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_NONE;
    HRESULT hr = d3dDevice_->CreateRasterizerState(&rastDesc, rasterState_.GetAddressOf());
    if (FAILED(hr))
        qFatal("Failed to create rasterizer state: 0x%x", hr);

    // create depth stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    memset(&dsDesc, 0, sizeof(dsDesc));
    hr = d3dDevice_->CreateDepthStencilState(&dsDesc, depthStencileState_.GetAddressOf());
    if (FAILED(hr))
        qFatal("Failed to create depth/stencil state: 0x%x", hr);

    // create blend state
    D3D11_RENDER_TARGET_BLEND_DESC blend;
    memset(&blend, 0, sizeof(blend));
    blend.BlendEnable = TRUE;
    blend.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend.BlendOp = D3D11_BLEND_OP_ADD;
    blend.SrcBlendAlpha = D3D11_BLEND_ONE;
    blend.DestBlendAlpha = D3D11_BLEND_ZERO;
    blend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    D3D11_BLEND_DESC blendDesc;
    memset(&blendDesc, 0, sizeof(blendDesc));
    blendDesc.IndependentBlendEnable = true;
    blendDesc.RenderTarget[0] = blend;
    hr = d3dDevice_->CreateBlendState(&blendDesc, blendState_.GetAddressOf());
    if (FAILED(hr))
        qFatal("Failed to create blend state: 0x%x", hr);
}

void
D3D11Renderer::cleanUp()
{
    D3D11_OBJECT_RELEASE(rasterState_);
    D3D11_OBJECT_RELEASE(blendState_);
    D3D11_OBJECT_RELEASE(vertexBuffer_);
    D3D11_OBJECT_RELEASE(indiceBuffer_);
    D3D11_OBJECT_RELEASE(vertexShader_);
    D3D11_OBJECT_RELEASE(pixelShader_);
    D3D11_OBJECT_RELEASE(inputLayout_);
    D3D11_OBJECT_RELEASE(constantBuffer_);
    D3D11_OBJECT_RELEASE(sampler_);
    D3D11_OBJECT_RELEASE(shaderResourceViewY_);
    D3D11_OBJECT_RELEASE(shaderResourceViewU_);
    D3D11_OBJECT_RELEASE(shaderResourceViewV_);
    D3D11_OBJECT_RELEASE(shaderResourceViewNV12_);
    D3D11_OBJECT_RELEASE(texY_);
    D3D11_OBJECT_RELEASE(texU_);
    D3D11_OBJECT_RELEASE(texV_);
    D3D11_OBJECT_RELEASE(texUV_NV12_);

    if (d3dDevContext_) d3dDevContext_->Flush();
}

D3D11QtQuickVideoWidgetBase::D3D11QtQuickVideoWidgetBase()
    : renderer_(nullptr)
{
    connect(this,&QQuickItem::windowChanged,this, &D3D11QtQuickVideoWidgetBase::handleWindowChanged);
}

void
D3D11QtQuickVideoWidgetBase::sync()
{
    syncFunction();
}

class CleanupJob : public QRunnable
{
private:
    D3D11Renderer* m_renderer;

public:
    CleanupJob(D3D11Renderer* renderer) : m_renderer(renderer) { }
    void run() override {if(m_renderer) delete m_renderer; }

};

D3D11QtQuickVideoWidgetBase::~D3D11QtQuickVideoWidgetBase()
{
}

void
D3D11QtQuickVideoWidgetBase::syncFunction()
{
    if ( !renderer_ ) {
        renderer_ = D3D11Renderer::createD3d11RendererFactory(this, roundedCornerRadius_);
        connect(window(), &QQuickWindow::beforeRendering, renderer_, &D3D11Renderer::frameStart, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, renderer_, &D3D11Renderer::mainPassRecordingStart, Qt::DirectConnection);
    }
    renderer_->setParantingQuickItem(this);
    renderer_->setWindow(window());
}

void D3D11QtQuickVideoWidgetBase::cleanupRenderer()
{
    delete renderer_;
    renderer_ = nullptr;
}

void
D3D11QtQuickVideoWidgetBase::handleWindowChanged(QQuickWindow* win)
{
    qDebug() << "The window of d3d11 qml widget has changed";
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &D3D11QtQuickVideoWidgetBase::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &D3D11QtQuickVideoWidgetBase::cleanupRenderer, Qt::DirectConnection);

        if(isDistant_) {
            win->setColor(Qt::black);
        } else {
            win->setColor(Qt::transparent);
        }
    }
}

void
D3D11QtQuickVideoWidgetBase::releaseResources()
{
    // release the d3d11 renderer resources
    window()->scheduleRenderJob(new CleanupJob(renderer_), QQuickWindow::BeforeSynchronizingStage);

    QQuickItem::releaseResources();
}

D3D11QtQuickVideoPreviewWidget::D3D11QtQuickVideoPreviewWidget()
    : D3D11QtQuickVideoWidgetBase()
{
    roundedCornerRadius_ = 0.2f;

    connect(LRCInstance::renderer(), &RenderManager::previewD3DFrameUpdated,
        [this]() {
            if(!renderer_) {
                return;
            }
            updatePreviewItemAspectRatio(LRCInstance::renderer()->getPreviewAVFrame());
            renderer_->updateTextures(LRCInstance::renderer()->getPreviewAVFrame());
           if (window()) window()->update();
        });
    connect(LRCInstance::renderer(), &RenderManager::previewRenderingStopped,
        [this]() {
            if(!renderer_) {
                return;
            }
            updatePreviewItemAspectRatio(LRCInstance::renderer()->getPreviewAVFrame());
            renderer_->updateTextures(LRCInstance::renderer()->getPreviewAVFrame());
            if (window()) window()->update();
        });
}

void
D3D11QtQuickVideoPreviewWidget::sync()
{
    if(renderer_) {
        renderer_->constantBufferDataStruct_.boarderRadiusNormalizedShortSide = roundedCornerRadius_;
    }
    updatePreviewWindowSizeAccordingToAspectRatio();
    D3D11QtQuickVideoWidgetBase::sync();
}

D3D11QtQuickVideoPreviewWidget::~D3D11QtQuickVideoPreviewWidget()
{
}

void
D3D11QtQuickVideoPreviewWidget::updatePreviewItemAspectRatio(AVFrame * frame)
{
    aspectRatio = 16 / 9;
    try {
        int angleToRotate = (int)renderer_->getAngleToRotate(frame);
        if ( (angleToRotate % 360 >= 45 && angleToRotate % 360 <= 135)
           ||(angleToRotate % 360 >= 225 && angleToRotate % 360 <= 315) ) {
            aspectRatio = frame->height / frame->width;
        } else {
            aspectRatio = frame->width / frame->height;
        }
    } catch(...) {
        return;
    }
}

void
D3D11QtQuickVideoPreviewWidget::updatePreviewWindowSizeAccordingToAspectRatio()
{
    if ( aspectRatio > 1 ) {
        qreal currentHeight = height();
        window()->setHeight(currentHeight / aspectRatio);
    } else if( aspectRatio > 0 && aspectRatio < 1 ) {
        qreal currentWidth = width();
        window()->setWidth(currentWidth * aspectRatio);
    }
}

D3D11QtQuickVideoDistantWidget::D3D11QtQuickVideoDistantWidget()
    : D3D11QtQuickVideoWidgetBase()
{
    isDistant_ = true;
    connect(LRCInstance::renderer(), &RenderManager::previewD3DFrameUpdated,
        [this]() {
            if(!renderer_) {
                return;
            }
            renderer_->updateTextures(LRCInstance::renderer()->getPreviewAVFrame());
            if(window()) window()->update();

        });
    connect(LRCInstance::renderer(), &RenderManager::previewRenderingStopped,
        [this]() {
            if(!renderer_) {
                return;
            }
            renderer_->updateTextures(LRCInstance::renderer()->getPreviewAVFrame());
            if(window()) window()->update();
        });
}

D3D11QtQuickVideoDistantWidget::~D3D11QtQuickVideoDistantWidget()
{
}
