#pragma once
#include <d3d11.h>
#include <stdint.h>

struct InternalData;

// Parameter structure
struct Param {
	uint32_t render_width;
	uint32_t render_height;

	int r;
	int c;

	float lineNum;
	float tg_lens;

	float padding[2];
};

class InterweaveRender {

	// DirectX variables inside
	InternalData* data;

public:
	// initialize internal data
	bool create(HWND hWnd, Param param);


	// CPU byte buffer: expect a pointer to the buffer and the buffer size.
	// Expect a "linear" buffer that store 1 byte RGBA value in the order of R G B A. 
	// Example Usage:
	/*
	    // Create a byte buffer to hold the pixel data
		std::vector<unsigned char> byteBuffer(width * height * 4);

    // Fill the buffer with random RGBA values
		for (int i = 0; i < byteBuffer.size(); i += 4) {
			byteBuffer[i] = randomPixelValue();  // R
			byteBuffer[i + 1] = randomPixelValue();  // G
			byteBuffer[i + 2] = randomPixelValue();  // B
			byteBuffer[i + 3] = randomPixelValue();  // A
		}

		app.update(byteBuffer.data(), byteBuffer.size());*/
	bool update(const uint8_t* new_data, size_t data_size);


	// ID3D11Texture2D: a DirectX texture type 
	// texture description as follows: 
	/*
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
	*/
	void update(ID3D11Texture2D* new_texture); //texture description needed

	// Render to hWnd window.
	void render();

	// Cleanup memory usage
	void destroy();

};