def remove_dup(arr):
    idx = 0
    for i in range(1, len(arr)):
        if arr[i] != arr[idx]:
            idx += 1
            arr[idx], arr[i] = arr[i], arr[idx]

    return idx + 1


arr = [1, 1, 1, 2, 3, 4, 4, 5, 6, 6, 6, 7, 8, 9]
print(remove_dup(arr))
print(arr)

arr = [1, 1, 1, 1, 2, 2, 2]
print(remove_dup(arr))
print(arr)
