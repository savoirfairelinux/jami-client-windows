#include "d3d11qtquickvideowidgetbae.h"

D3D11QtQuickVideoWidgetBae::D3D11QtQuickVideoWidgetBae()
{

}

D3D11Renderer::D3D11Renderer(QObject* parent)
    : QObject(parent)
    , d3dDevice_(0)
    , d3dDevContext_(0)
    , swapChain_(0)
    , renderTargetView_(0)
    , rastState_(0)
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

}

D3D11Renderer::~D3D11Renderer()
{
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

    if (m_vert.isEmpty())
        prepareShader(VertexStage);
    if (m_frag.isEmpty())
        prepareShader(FragmentStage);

    if (!m_initialized)
        init();
}

void
D3D11Renderer::mainPassRecordingStart()
{
    // rendering function
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
    }
    else {
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
        target = "vs_4_0";
        break;
    case FragmentStage:
        target = "ps_4_0";
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
}
