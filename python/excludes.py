def exclude_nums(arr1, arr2):
    arr = [None for i in range(len(arr1))]
    i, j, k = 0, 0, 0
    while i < len(arr1) and j < len(arr2):
        if arr1[i] < arr2[j]:
            arr[k] = arr1[i]
            k += 1
            i += 1
        elif arr1[i] > arr2[j]:
            j += 1
        else:
            i += 1
            j += 1

    while i < len(arr1):
        arr[k] = arr1[i]
        k += 1
        i += 1

    return arr


arr1 = [1, 2, 3, 4, 5]
arr2 = [2, 4, 7, 8]
arr = exclude_nums(arr1, arr2)
print(arr)
