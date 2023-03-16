def lev_distance(str1, str2) -> int:
    """
    Minimum number of ops (delete, insert, replace) to transform s1 to s2.
    """
    if len(str1) == 0:
        return len(str2)
    if len(str2) == 0:
        return len(str1)

    dp = [[0 for i in range(len(str2) + 1)] for j in range(len(str1) + 1)]

    for i in range(len(str2) + 1):
        dp[0][i] = i

    for j in range(len(str1) + 1):
        dp[j][0] = j

    for i in range(1, len(str1) + 1):
        for j in range(1, len(str2) + 1):
            if str1[i - 1] == str2[j - 1]:
                dp[i][j] = dp[i - 1][j - 1]
            else:
                deletion = 1 + dp[i - 1][j]
                insertion = 1 + dp[i][j - 1]
                replace = 1 + dp[i - 1][j - 1]
                dp[i][j] = min(deletion, min(insertion, replace))

    return dp[len(str1)][len(str2)]


print(lev_distance("funk", "splunk"))
print(lev_distance("status", "stage"))
print(lev_distance("stash", "status"))
print(lev_distance("knife", "wife"))
