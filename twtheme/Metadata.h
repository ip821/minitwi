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
					const static CComBSTR RootContainerName(L"Header");

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
				}

				namespace LayoutTypes
				{
					const static CComBSTR HorizontalContainer(L"horizontal_container");
					const static CComBSTR ImageColumn(L"image_column");
					const static CComBSTR TextColumn(L"text_column");
				}
			}
		}
	}
}