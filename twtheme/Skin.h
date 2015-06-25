#pragma once

#include <gdiplus.h>

using namespace GdiPlus;

typedef Color SkinColor;

struct SkinFont
{

};

enum class SkinHorizontalAlignment
{
	None,
	Left,
	Right,
	Center
};

enum class SkinVerticalAlignment
{
	None,
	Left,
	Right,
	Center
};

const int COORD_AUTO = -1;

struct SkinElement
{
	int Left = 0;
	int Right = 0;
	int Width = COORD_AUTO;
	int Height = COORD_AUTO;
	HorizontalAlignment HorizontalAlignment = HorizontalAlignment::None;
	VerticalAlignment VerticalAlignment = VerticalAlignment::None;
	SkinColor BackgroundColor = SkinColor::Transparent;
	SkinFont Font = {0};
};

enum class SkinContainerType
{
	None,
	Horizontal,
	Vertical
};

struct SkinContainer : SkinElement
{
	SkinContainerType SkinContainerType = SkinContainerType::None;
	SkinElement[] Elements;
};

struct SkinColumn : SkinElement
{

};

struct TextColumn : SkinColumn
{
	CComBSTR Name;
	CComBSTR Text;
	bool TextWrap = false;
};

struct ImageColumn : SkinColumn
{
	CComBSTR Name;
	CComBSTR ImageKey;
};