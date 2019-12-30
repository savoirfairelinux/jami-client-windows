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
}

void
D3DVideoWidgetBase::InitD3D()
{
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
}
