

def two_sum(nums, target):
    freq = {} 
    for i in range(len(nums)):
        complement = target - nums[i]
        if complement in freq:
            return [freq[complement], i]
        else:
            freq[nums[i]] = i
    return []

# abcdeabba

def longest_substr(s):
    if len(s) == 0: return 0

    p1 = 0
    p2 = 0
    c_map = {}
    max_len = 0
    while p2 < len(s):
        if s[p2] in c_map and c_map[s[p2]] >= p1:
            p1 = c_map[s[p2]] + 1
            
            
        c_map[s[p2]] = p2
        max_len = max(max_len, p2 - p1 + 1)
        p2 += 1

    return max_len

def main():
    nums = [2, 7, 11, 15]
    target = 9

    s = "abcabcbb"

    # print(two_sum(nums, target))

    print(longest_substr(s))
if __name__ == "__main__":
    main()