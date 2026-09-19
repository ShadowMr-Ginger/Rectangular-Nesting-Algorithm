#include <iostream>
#include <vector>

using namespace std;

class Solution3065 {
public:
    int minOperations(vector<int>& nums, int k) {
        int count = 0;
        // 只需统计严格小于 k 的元素个数，它们就是必须移除的
        for (int num : nums) {
            if (num < k) {
                count++;
            }
        }
        return count;
    }
};

//int main() {
//	Solution3065 solution;
//	vector<int> nums = {2, 11, 10, 1, 3};
//	int k = 10;
//	cout << solution.minOperations(nums, k) << endl;
//	vector<int> nums2 = {1, 1, 2, 4, 9};
//	int k2 = 1;
//	cout << solution.minOperations(nums2, k2) << endl;
//	return 0;
//}
