#pragma once

namespace IP
{
	namespace Twitter
	{
		namespace Themes
		{
			namespace Metadata
			{
				namespace TabContainer
				{
					const static CComBSTR LayoutName(L"MainTabContainerHeaderLayout");

					namespace Images
					{
						const static CComBSTR Home(L"home");
						const static CComBSTR Lists(L"lists");
						const static CComBSTR Search(L"search");
						const static CComBSTR Settings(L"settings");
					}
				}

				namespace ImageColumn
				{
					const static CComBSTR ImageKey(L"image");
				}

				namespace TextColumn
				{
					const static CComBSTR Text(L"text");
					const static CComBSTR Font(L"font");
					const static CComBSTR Color(L"color");
					const static CComBSTR ColorSelected(L"colorSelected");
				}

				namespace Element
				{
					const static CComBSTR Name(L"name");
					const static CComBSTR Elements(L"elements");
					const static CComBSTR Type(L"type");
					const static CComBSTR Selected(L"selected");
					const static CComBSTR PaddingLeft(L"padding_left");
					const static CComBSTR PaddingRight(L"padding_right");
					const static CComBSTR PaddingTop(L"padding_top");
					const static CComBSTR PaddingBottom(L"padding_bottom");
					const static CComBSTR FitHorizontal(L"fit_horizontal");
					const static CComBSTR FitVertical(L"fit_vertical");
					const static CComBSTR BorderBottom(L"border_bottom");
					const static CComBSTR BorderBottomColor(L"border_bottom_color");
					const static CComBSTR BorderBottomWidth(L"border_bottom_width");
				}

				namespace LayoutTypes
				{
					const static CComBSTR HorizontalContainer(L"horizontal_container");
					const static CComBSTR ImageColumn(L"image_column");
					const static CComBSTR TextColumn(L"text_column");
				}

				namespace FitTypes
				{
					const static CComBSTR Content(L"content");
					const static CComBSTR Parent(L"parent");
				}
			}
		}
	}
}