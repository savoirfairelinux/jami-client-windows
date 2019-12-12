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
    explicit GLSharedContextObject(QObject* parent = nullptr);
    ~GLSharedContextObject();

    void draw();

signals:
    void drawSignal();

private:
    void initializeAll();
    bool initializeShader();
    void initializeTexture();
    void setUpBuffers();

public:
    QOpenGLContext* context_;
    QOpenGLTexture* texture_;
    QOpenGLBuffer* vbo_;
    QOpenGLBuffer* ibo_;
    QOpenGLShaderProgram* shaderProgram_;

private:
    QOffscreenSurface* offscreenSurface_;
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
    void setContextObj(GLSharedContextObject* contextObj);

protected:
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initializeGL() override;

private:
    //bool initializeShader();
    //void initializeTexture();
    //void setUpBuffers();

private:
    Ui::TestOpenGLForm *ui;

    //QOpenGLTexture* texture_;
    //QOpenGLShaderProgram* shaderProgram_;

    //QOpenGLBuffer* vbo_;
    //QOpenGLBuffer* ibo_;
    GLSharedContextObject* GLShare_;
};
#endif // TESTOPENGLFORM_H
