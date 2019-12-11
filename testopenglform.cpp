#include "testopenglform.h"
#include "ui_testopenglform.h"

GLfloat verticesCorTexCor[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f

};

const int g_indices[] = { 0,1,2,2,3,0 };

TestOpenGLForm::TestOpenGLForm(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::TestOpenGLForm)
{
    ui->setupUi(this);
}

TestOpenGLForm::~TestOpenGLForm()
{
    makeCurrent();
    vbo_->destroy();
    ibo_->destroy();
    texture_->destroy();
    delete shaderProgram_;
    shaderProgram_ = nullptr;
    doneCurrent();
    delete ui;
}

void
TestOpenGLForm::draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderProgram_->bind();
    vbo_->bind();
    ibo_->bind();
    texture_->bind(0);

    glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_INT,0);

    texture_->release();
    ibo_->release();
    vbo_->release();
    shaderProgram_->release();
}

void
TestOpenGLForm::resizeGL(int w, int h)
{
    glViewport(0, 0, w,h);
}

void
TestOpenGLForm::paintGL()
{
    qDebug() << "Drawing!";
    draw();
}

void
TestOpenGLForm::initializeGL()
{
    initializeOpenGLFunctions();
    glViewport(0, 0, this->width(), this->height());

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "GL Initialize";

    initializeShader();
    initializeTexture();
    setUpBuffers();
}

bool
TestOpenGLForm::initializeShader()
{
    bool success = true;
    shaderProgram_ = new QOpenGLShaderProgram(this);
    QDir currentPath = QDir(QCoreApplication::applicationDirPath() + "/../../shader");
    auto vsFile = currentPath.filePath("TestTriangle.vert");
    auto fsFile = currentPath.filePath("TestTriangle.frag");
    success &= shaderProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, vsFile);
    success &= shaderProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, fsFile);
    success &= shaderProgram_->link();
    if (!success) {
        qDebug() << "adding shaders fails";
    }
    return success;
}

void
TestOpenGLForm::initializeTexture()
{
    QImage img("C:/Users/wxdns/Desktop/test1.bmp");

    texture_ = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture_->setMinificationFilter(QOpenGLTexture::Filter::Nearest);
    texture_->setMagnificationFilter(QOpenGLTexture::Filter::Linear);
    texture_->create();
    //texture_->allocateStorage();
    texture_->setData(img.mirrored());
    texture_->bind(0);
    GLuint m_Tex = shaderProgram_->uniformLocation("Tex");
    shaderProgram_->setUniformValue(m_Tex, 0);
    texture_->release();
}

void
TestOpenGLForm::setUpBuffers()
{
    vbo_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ibo_ = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    shaderProgram_->bind();
    vbo_->create();
    vbo_->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_->bind();
    vbo_->allocate(verticesCorTexCor, sizeof(verticesCorTexCor));
    GLuint m_posAttr = shaderProgram_->attributeLocation("posAttr");
    shaderProgram_->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_posAttr);

    GLuint m_textureCor = shaderProgram_->attributeLocation("textureCoordinate");
    shaderProgram_->setAttributeBuffer(m_textureCor, GL_FLOAT, 3 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_textureCor);

    GLuint m_colAttr = shaderProgram_->attributeLocation("colAttr");
    shaderProgram_->setAttributeBuffer(m_colAttr, GL_FLOAT, 5 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_colAttr);
    vbo_->release();

    ibo_->create();
    ibo_->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_->bind();
    ibo_->allocate(g_indices, sizeof(g_indices));
    ibo_->release();

    shaderProgram_->release();
}

GLSharedContextObject::GLSharedContextObject(TestOpenGLForm* parent):
                       QObject(qobject_cast<QObject*>(parent))
{
    offscreenSurface_ = new QOffscreenSurface();
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(0);

    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setSamples(0);
    offscreenSurface_->setFormat(format);
    offscreenSurface_->create();

    context = new QOpenGLContext(this);
    context->setFormat(format);
    context->create();
    context->makeCurrent(offscreenSurface_);
}

GLSharedContextObject::~GLSharedContextObject()
{
}

