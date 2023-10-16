#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>
#include "InterweaveSDK.h"
#include "opencv2/opencv.hpp"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool initWindow(HINSTANCE hInstance, int nCmdShow, HWND* hWnd) {

    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"DX11WindowClass";
    wcex.hIconSm = nullptr;
    if (!RegisterClassEx(&wcex))
        return false;

    // Create window
    RECT rc = { 0, 0, 7680, 4320 }; //TODO: set window
    std::cout << rc.right - rc.left << " " << rc.bottom - rc.top << std::endl;
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    *hWnd = CreateWindow(L"DX11WindowClass", L"Direct3D 11 Video Decoder Sample",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);
    if (!*hWnd)
        return false;

    ShowWindow(*hWnd, nCmdShow);

    return true;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {

    InterweaveRender app;

    Param param;

    param.render_width = 7680;
    param.render_height = 4320;
    param.r = 6; // there are 6 rows
    param.c = 4; // there are 4 columns
    param.lineNum = 11.983080;
    param.tg_lens = -0.154916;


    HWND hWnd;
    if (!initWindow(hInstance, nCmdShow, &hWnd))
        return 0;
    if (!app.create(hWnd, param))
        return false;

    int width = 7680;
    int height = 4320;

    //opencv import image. BGRA -> RGBA
    //cv::Mat exampleImage = cv::imread("example.png", cv::IMREAD_UNCHANGED);
    cv::Mat exampleImage = cv::imread("example3.png", cv::IMREAD_UNCHANGED);
    cv::cvtColor(exampleImage, exampleImage, cv::COLOR_BGRA2RGBA);


    // Raw Buffer update init
    // RGBA requires 4 bytes per pixel
    const int bytesPerPixel = 4;

    // Create a std::vector to hold the image data
    std::vector<unsigned char> byteBuffer(width * height * bytesPerPixel);

    // Define dimensions for each grid in the 6x4 grid
    const int gridWidth = width / 4;
    const int gridHeight = height / 6;

    // Initialize the buffer
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Determine which grid cell the current pixel falls into
            const int gridX = x / gridWidth;
            const int gridY = y / gridHeight;

            // Calculate the starting index for this pixel
            const int index = (y * width + x) * bytesPerPixel;

            // Set RGBA values based on the grid position
            // We will make the top-left grid white and the rest black
            unsigned char R, G, B, A;
            if (gridX == 0 && gridY == 0) {
                R = 255;
                G = 255;
                B = 255;
                A = 255;
            }
            else {
                R = 0;
                G = 0;
                B = 0;
                A = 255;
            }

            // Add the RGBA values to the buffer
            byteBuffer[index] = R;
            byteBuffer[index + 1] = G;
            byteBuffer[index + 2] = B;
            byteBuffer[index + 3] = A;
        }
    }

    // DirectX 11 update init: TODO: 'device' variable needed to be initialized to use Texture2D update.
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = byteBuffer.data();
    initData.SysMemPitch = width * bytesPerPixel;

    ID3D11Texture2D* pTexture = nullptr;
    //HRESULT hr = device->CreateTexture2D(&texDesc, &initData, &pTexture); 

    // DirectX 11 update call
    //app.update(pTexture);

    // Raw buffer update call
    //app.update(byteBuffer.data(), byteBuffer.size());

    // Opencv update call
    app.update(exampleImage.data, exampleImage.cols*exampleImage.rows*exampleImage.elemSize());

    while (true) {
        app.render();
    }

}