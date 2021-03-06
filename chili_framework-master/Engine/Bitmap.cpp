#include "Bitmap.h"


Bitmap::Bitmap(const string& filename)
{
	ifstream file(filename, std::ios::binary);
	if (!file.is_open())
		throw filename + " cannot be opened.";

	BITMAPFILEHEADER bmFileHeader;
	file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));

	BITMAPINFOHEADER bmInfoHeader;
	file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));

	assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);
	assert(bmInfoHeader.biCompression == BI_RGB);

	const bool is32b = bmInfoHeader.biBitCount == 32;

	width = bmInfoHeader.biWidth;

	// test for reverse row order and control
	// y loop accordingly
	int yStart;
	int yEnd;
	int dy;
	if (bmInfoHeader.biHeight < 0)
	{
		height = -bmInfoHeader.biHeight;
		yStart = 0;
		yEnd = height;
		dy = 1;
	}
	else
	{
		height = bmInfoHeader.biHeight;
		yStart = height - 1;
		yEnd = -1;
		dy = -1;
	}

	pPixels = new Color[width*height];

	file.seekg(bmFileHeader.bfOffBits);
	// padding is for the case of of 24 bit depth only
	const int padding = (4 - (width * 3) % 4) % 4;

	for (int y = yStart; y != yEnd; y += dy)
	{
		for (int x = 0; x < width; x++)
		{
			PutPixel(x, y, Color(file.get(), file.get(), file.get()));
			if (is32b)
			{
				file.seekg(1, std::ios::cur);
			}
		}
		if (!is32b)
		{
			file.seekg(padding, std::ios::cur);
		}
	}
}

Bitmap::Bitmap(const Bitmap& rhs)
	: width(rhs.width), height(rhs.height)
{
	pPixels = new Color[width * height];
	const int nPixels = width * height;
	for (int i = 0; i < nPixels; i++)
		pPixels[i] = rhs.pPixels[i];
	//memcpy(pPixels, rhs.pPixels, sizeof(Color) * width * height);
}

Bitmap::~Bitmap()
{
	delete[] pPixels;
	pPixels = nullptr;
}

Bitmap& Bitmap::operator=(const Bitmap & rhs)
{
	if (this == &rhs)
		return *this;

	width = rhs.width;
	height = rhs.height;

	delete[] pPixels;
	pPixels = new Color[width*height];

	const int nPixels = width * height;
	for (int i = 0; i < nPixels; i++)
		pPixels[i] = rhs.pPixels[i];
	//memcpy(pPixels, rhs.pPixels, sizeof(Color) * width * height);
	return *this;
}

void Bitmap::PutPixel(int x, int y, const Color & c)
{
	pPixels[y * width + x] = c;
}

Color Bitmap::GetPixel(int x, int y) const
{
	return pPixels[y * width + x];
}

void Bitmap::Draw(Graphics & gfx, int x, int y, const Color & alpha) const
{
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
			if (pPixels[j * width + i] != alpha)
				gfx.PutPixel(x + i, y + j, pPixels[j * width + i]);
}

void Bitmap::DrawPart(Graphics & gfx, int x, int y, int fromx, int fromy, int tox, int toy, const Color & alpha) const
{
	for (int i = fromx; i < tox && i < width; ++i)
		for (int j = fromy; j < toy && j < height; ++j)
			if (pPixels[j*width + i] != alpha)
				gfx.PutPixel(x + i - fromx, y + j - fromy, pPixels[j * width + i]);
}

void Bitmap::leave_only_this_color(const Color & color)
{
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
			if (pPixels[j*width + i] != color)
				pPixels[j*width + i] = Colors::White;
}

void Bitmap::resize(int newwidth, int newheight)
{
	if (newwidth == width && newheight == height)
		return;

	double height_ratio = double(height) / double(newheight);
	double width_ratio  = double(width)  / double(newwidth);

	Color * tmp = new Color[newwidth * newheight];
	if (newwidth > width)
	{
		if (newheight > height)
		{

		}
		else
		{

		}
		delete tmp;
		return;
	}
	else // newwidth < width
	{
		if (newheight > height)
		{
			delete tmp;
			return;
		}
		else // newheight < height
		{
			int xOut = 0; double x = 0.0;
			for (; xOut < newwidth*newheight; x += width_ratio, xOut++)
			{
				Color sample0 = pPixels[int(std::floor(x))];
				Color sample1 = pPixels[int(std::ceil(x))];

				const float t = x - std::floor(x);

				unsigned char sampleR = sample0.GetR() + ((sample1.GetR() - sample0.GetR()) * t);
				unsigned char sampleG = sample0.GetG() + ((sample1.GetG() - sample0.GetG()) * t);
				unsigned char sampleB = sample0.GetB() + ((sample1.GetB() - sample0.GetB()) * t);

				tmp[xOut] = Colors::MakeRGB(sampleR, sampleG, sampleB);

				delete pPixels;
				pPixels = tmp;
				tmp = NULL;
				return;
			}
		}
	}
}
