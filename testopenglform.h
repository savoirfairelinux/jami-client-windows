#ifndef TESTOPENGLFORM_H
#define TESTOPENGLFORM_H

#include <QWidget>
#include <QtOpenGL/QtOpenGL>
#include  <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>
#include <QOffscreenSurface>

namespace Ui {
class TestOpenGLForm;
}

class TestOpenGLForm;

class GLSharedContextObject : public QObject
{
    Q_OBJECT

public:
    explicit GLSharedContextObject(TestOpenGLForm* parent = nullptr);
    ~GLSharedContextObject();

    void draw();

private:
    bool initializeShader();
    void initializeTexture();
    void setUpBuffers();

public:
    QOpenGLContext* context;
    QOpenGLTexture* texture_;

private:
    QOpenGLShaderProgram* shaderProgram_;

    QOffscreenSurface* offscreenSurface_;
    QOpenGLBuffer* vbo_;
    QOpenGLBuffer* vbo_Color;
    QOpenGLBuffer* vbo_Tex;
    QOpenGLVertexArrayObject* vao;
    QOpenGLBuffer* ibo_;

};

class TestOpenGLForm : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    int indices[3] = {
    0,1,2 };

public:
    explicit TestOpenGLForm(QWidget *parent = nullptr);
    ~TestOpenGLForm();

    void draw();

protected:
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initializeGL() override;

private:
    bool initializeShader();
    void initializeTexture();
    void setUpBuffers();

private:
    Ui::TestOpenGLForm *ui;

    QOpenGLTexture* texture_;
    QOpenGLShaderProgram* shaderProgram_;

    QOpenGLBuffer* vbo_;
    QOpenGLBuffer* ibo_;
    GLSharedContextObject* GLShare_;
};




#endif // TESTOPENGLFORM_H
