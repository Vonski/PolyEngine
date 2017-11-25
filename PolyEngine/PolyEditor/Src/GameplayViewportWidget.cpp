#include "PolyEditorPCH.hpp"
#include "GameplayViewportWidget.hpp"
#include <LibraryLoader.hpp>

#include <windows.h>
#include <windowsx.h>

#include <QGLWidget>

using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (HWND hwnd, RECT rect);
using CreateGameFunc = Poly::IGame* (void);

static Poly::LibraryFunctionHandle<CreateRenderingDeviceFunc> LoadRenderingDevice;
static Poly::LibraryFunctionHandle<CreateGameFunc> LoadGame;

// ---------------------------------------------------------------------------------------------------------
GameplayViewportWidget::GameplayViewportWidget(QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_NativeWindow);
	setMouseTracking(true);

	LoadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libRenderingDevice", "PolyCreateRenderingDevice");
	ASSERTE(LoadRenderingDevice.FunctionValid(), "Error loading rendering device DLL");
	LoadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libGame", "CreateGame");
	ASSERTE(LoadGame.FunctionValid(), "Error loading rendering device DLL");
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::InitializeViewport()
{
	RECT viewportRect;
	viewportRect.top = 0;
	viewportRect.left = 0;
	viewportRect.bottom = height();
	viewportRect.right = width();

	// TODO: catch winId changes (http://doc.qt.io/qt-5/qwidget.html#winId)
	// TODO: something like addviewport to rendering device
	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(LoadGame());
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice((HWND)winId(), viewportRect));

	Poly::gEngine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine loaded successfully");
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::Update()
{
	Poly::gEngine->Update();
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::resizeEvent(QResizeEvent* resizeEvent)
{
	Poly::ScreenSize screenSize;
	screenSize.Width = resizeEvent->size().width();
	screenSize.Height = resizeEvent->size().height();
	Poly::gEngine->ResizeScreen(screenSize);
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	Poly::gEngine->UpdateWheelPos(Poly::Vector(static_cast<float>(wheelEvent->delta()), 0, 0));
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	Poly::gEngine->UpdateMousePos(Poly::Vector(static_cast<float>(mouseEvent->pos().x()), static_cast<float>(mouseEvent->pos().y()), 0));
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(Poly::eKey::MLBUTTON));
		break;

	case Qt::RightButton:
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(Poly::eKey::MRBUTTON));
		break;

	case Qt::MiddleButton:
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(Poly::eKey::MMBUTTON));
		break;

	default:
		// unsupported key
		break;
	}
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(Poly::eKey::MLBUTTON));
		break;

	case Qt::RightButton:
		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(Poly::eKey::MRBUTTON));
		break;

	case Qt::MiddleButton:
		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(Poly::eKey::MMBUTTON));
		break;

	default:
		// unsupported key
		break;
	}
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	Poly::gEngine->KeyDown(static_cast<Poly::eKey>((unsigned int)keyEvent->nativeVirtualKey()));
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
 		Poly::gEngine->KeyUp(static_cast<Poly::eKey>((unsigned int)keyEvent->nativeVirtualKey()));
}