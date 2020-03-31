#include "stdafx.h"
#include "image.h"


image::image()
	: _backBuffer(NULL)
{
}


image::~image()
{
}

HRESULT image::init(int width, int height)
{
	//백버퍼가 존재하면 메모리 해제
	if (_backBuffer != NULL) release();

	HDC hdc = GetDC(_hWnd);

	_backBuffer = new IMAGE_INFO;
	_backBuffer->hMemDC = CreateCompatibleDC(hdc);	// 빈 DC영역 하나를 만든다
	_backBuffer->hBit = (HBITMAP)CreateCompatibleBitmap(hdc, width, height);
	_backBuffer->hOBit = (HBITMAP)SelectObject(_backBuffer->hMemDC, _backBuffer->hBit);
	_backBuffer->width = width;
	_backBuffer->height = height;

	//위에 셋팅이 실패해서 백버퍼가 생성되지 않았다면
	if (_backBuffer == NULL)
	{
		//메모리 해제
		release();

		//그리고 실패했단 메세지를 호출해라
		return E_FAIL;
	}


	ReleaseDC(_hWnd, hdc);

	return S_OK;
}

void image::release()
{
	if (_backBuffer)
	{
		SelectObject(_backBuffer->hMemDC, _backBuffer->hOBit);
		DeleteObject(_backBuffer->hBit);
		DeleteDC(_backBuffer->hMemDC);


		SAFE_DELETE(_backBuffer);
	}

}

void image::render(HDC hdc, int destX, int destY)
{
	//DC영역 간의 고속복사를 담당하는 함쑤
	BitBlt(hdc,				//복사할 DC
		destX,				//복사할 X좌표(left)
		destY,				//복사할 Y좌표(top)
		_backBuffer->width,	//복사할 크기(width)
		_backBuffer->height,//복사할 크기(height)
		_backBuffer->hMemDC,//복사될 DC
		0, 0,				//복사될 X, Y (left, top)
		SRCCOPY);			//변형없이 복사하겠다
}
