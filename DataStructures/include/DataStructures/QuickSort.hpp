#pragma once
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
			int QuickSortPartition(T** values, int low, int high, std::function<bool(T&, T&)> comparitor = nullptr)
			{
				T pivot = *values[high];
				int index = low - 1;

				if(!comparitor) // Set default comparitor
					comparitor = [](T& a, T& b) { return a >= b; };

				for (int j = low; j < high; j++)
				{
					if (comparitor(*values[j], pivot))
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