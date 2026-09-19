#include <iostream>
#include <string>
using namespace std;

class Solution1138 {
public:
    string alphabetBoardPath(string target) {
        string path;
        int x = 0;
        int y = 0;
        int n = target.size();
        for (int i = 0;i < n;i++) {
            int idx = target[i] - 'a';
            int r = idx / 5;
            int c = idx % 5;
            if (x == 5) { // 当前在 z，只能先向上
                while (x > r) { path += 'U'; x--; }
                while (x < r) { path += 'D'; x++; }
                while (y > c) { path += 'L'; y--; }
                while (y < c) { path += 'R'; y++; }
            } else if (r == 5) { // 目标 z，先移到第 0 列再向下
                while (y > c) { path += 'L'; y--; }
                while (y < c) { path += 'R'; y++; }
                while (x > r) { path += 'U'; x--; }
                while (x < r) { path += 'D'; x++; }
            } else { // 普通位置：先上下后左右
                while (x > r) { path += 'U'; x--; }
                while (x < r) { path += 'D'; x++; }
                while (y > c) { path += 'L'; y--; }
                while (y < c) { path += 'R'; y++; }
            }
            path += '!';
        }
        return path;
    }
};

//int main() {
//	Solution1138 sol;
//	cout << sol.alphabetBoardPath("leet") << endl;
//	cout << sol.alphabetBoardPath("code") << endl;
//	return 0;
//}
