#pragma once
#include <cstdint>
#include <functional>

namespace LCDS
{
	namespace Sorting
	{
		namespace __internal
		{
			template<typename T>
			void QuickSortSwapValues(T* a, T* b)
			{
				T temp = *a;
				*a = *b;
				*b = temp;
			}

			template<typename T>
			bool QuickSortDefaultComparitor(T& a, T& b) { return a >= b; }

			template<typename T>
			int QuickSortPartition(T** values, int low, int high, std::function<bool(T&, T&)> comparitor = nullptr)
			{
				T pivot = *values[high];
				int index = low - 1;

				for (int j = low; j < high; j++)
				{

					if (comparitor && comparitor(*values[j], pivot))
						continue;
					if (!comparitor && QuickSortDefaultComparitor(*values[j], pivot))
						continue;

					index++;
					QuickSortSwapValues(values[index], values[j]);
				}

				QuickSortSwapValues(values[index + 1], values[high]);
				return index + 1;
			}
		}

		template<typename T>
		void QuickSort(T** values, int low, int high, std::function<bool(T&, T&)> comparitor = nullptr)
		{
			if (low >= high)
				return;
			const int pivotIndex = __internal::QuickSortPartition(values, low, high, comparitor);
			QuickSort(values, low, pivotIndex - 1, comparitor);
			QuickSort(values, pivotIndex + 1, high, comparitor);
		}
	}
}