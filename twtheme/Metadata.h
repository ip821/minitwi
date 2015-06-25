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
					const static CComBSTR InfoContainer(L"InfoContainer");
					const static CComBSTR InfoImage(L"InfoImage");
					const static CComBSTR MarqueeProgressBox(L"MarqueeProgressBox");

					namespace Images
					{
						const static CComBSTR Home(L"home");
						const static CComBSTR Lists(L"lists");
						const static CComBSTR Search(L"search");
						const static CComBSTR Settings(L"settings");
						const static CComBSTR Error(L"error");
						const static CComBSTR Info(L"info");
					}
				}

				namespace ImageColumn
				{
					const static CComBSTR ImageKey(L"image");
				}

				namespace MarqueeProgressColumn
				{
					const static CComBSTR ItemSize(L"item_size");
					const static CComBSTR ItemDistance(L"item_distance");
					const static CComBSTR ItemCount(L"item_count");
					const static CComBSTR ColorActive(L"color_active");
					const static CComBSTR ColorInactive(L"color_inactive");
					const static CComBSTR Value(L"value");
				}

				namespace TextColumn
				{
					const static CComBSTR Text(L"text");
					const static CComBSTR Font(L"font");
					const static CComBSTR Color(L"color");
					const static CComBSTR ColorSelected(L"color_selected");
				}

				namespace Element
				{
					const static CComBSTR Name(L"name");
					const static CComBSTR Elements(L"elements");
					const static CComBSTR Type(L"type");
					const static CComBSTR Visible(L"visible");
					const static CComBSTR Selected(L"selected");
					const static CComBSTR MarginLeft(L"margin_left");
					const static CComBSTR MarginRight(L"margin_right");
					const static CComBSTR MarginTop(L"margin_top");
					const static CComBSTR MarginBottom(L"margin_bottom");
					const static CComBSTR FitHorizontal(L"fit_horizontal");
					const static CComBSTR FitVertical(L"fit_vertical");
					const static CComBSTR BorderBottom(L"border_bottom");
					const static CComBSTR BorderBottomColor(L"border_bottom_color");
					const static CComBSTR BorderBottomWidth(L"border_bottom_width");
					const static CComBSTR AlignHorizontal(L"align_horizontal");
				}

				namespace AlignTypes
				{
					const static CComBSTR Right(L"right");
				}

				namespace LayoutTypes
				{
					const static CComBSTR HorizontalContainer(L"horizontal_container");
					const static CComBSTR ImageColumn(L"image_column");
					const static CComBSTR TextColumn(L"text_column");
					const static CComBSTR MarqueeProgressColumn(L"marquee_progress");
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