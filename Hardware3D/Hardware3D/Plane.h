/************************************************************************************************************
 * This is essentially a copy of Plane from the DirectX                                                     *
 * tutorial series on youtube at: https://www.youtube.com/playlist?list=PLqCJpWy5Fohd3S7ICFXwUomYW0Wv67pDD  *
 ************************************************************************************************************/
#pragma once
#include <vector>
#include <array>
#include "IndexedTriangleList.h"
#include "HWMath.h"

class Plane
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTessellated(int divisionsX, int divisionsY, float width = 1.0f, float height = 1.0f)
	{
		namespace dx = DirectX;
		assert(divisionsX >= 1);
		assert(divisionsY >= 1);
		
		const int numVertsX = divisionsX + 1;
		const int numVertsY = divisionsY + 1;
		std::vector<V> vertices(numVertsX * numVertsY);

		{
			const float halfSizeX = width / 2.0f;
			const float halfSizeY = height / 2.0f;
			const float vertOffsetX = width / float(divisionsX);
			const float vertOffsetY = height / float(divisionsY);
			const auto bottomLeft = dx::XMVectorSet(-halfSizeX, -halfSizeY, 0.0f, 0.0f);

			for (int y = 0, i = 0; y < numVertsY; y++)
			{
				const float yPos = float(y) * vertOffsetY;
				for (int x = 0; x < numVertsX; x++, i++)
				{
					const auto v = dx::XMVectorAdd(
						bottomLeft,
						dx::XMVectorSet(float(x) * vertOffsetX, yPos, 0.0f, 0.0f)
					);
					dx::XMStoreFloat3(&vertices[i].pos, v);
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve(Sq(divisionsX * divisionsY) * 6);
		{
			const auto VertToIndex = [numVertsX](size_t x, size_t y)
			{
				return (unsigned short)(y * numVertsX + x);
			};
			for (size_t y = 0; y < divisionsY; ++y)
			{
				for (size_t x = 0; x < divisionsX; ++x)
				{
					const std::array<unsigned short, 4> indexArray =
					{ VertToIndex(x,y),VertToIndex(x + 1,y),VertToIndex(x,y + 1),VertToIndex(x + 1,y + 1) };
					indices.push_back(indexArray[0]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[3]);
				}
			}
		}

		return{ std::move(vertices),std::move(indices) };
	}
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTessellated<V>(1, 1);
	}
};