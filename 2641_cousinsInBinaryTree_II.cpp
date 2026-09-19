#include <queue>
using namespace std;

class Solution2641 {
public:
	TreeNode* replaceValueInTree(TreeNode* root) {
		if (root == nullptr) return root;
		queue<TreeNode*> q;
		q.push(root);
		int level_sum = root->val;
		root->val = 0; // 根节点没有堂兄弟，直接置 0
		while (!q.empty()) {
			int size = q.size();
			int next_level_sum = 0; // 下一层的原始节点值总和
			for (int i = 0;i < size;i++) {
				TreeNode* node = q.front();
				q.pop();
				// 1. 计算当前节点的孩子之和（即亲兄弟之和）
				int children_sum = 0;
				if (node->left != nullptr) children_sum += node->left->val;
				if (node->right != nullptr) children_sum += node->right->val;
				// 2. 堂兄弟之和 = 本层总和 - 亲兄弟之和
				if (node->left != nullptr) {
					node->left->val = level_sum - children_sum;
					q.push(node->left);
				}
				if (node->right != nullptr) {
					node->right->val = level_sum - children_sum;
					q.push(node->right);
				}
				next_level_sum += children_sum;
			}
			level_sum = next_level_sum;
		}
		return root;
	}
};

//int main() {
//	// 示例：root = [5,4,9,1,10,null,7]
//	TreeNode* root = new TreeNode(5);
//	root->left = new TreeNode(4);
//	root->right = new TreeNode(9);
//	root->left->left = new TreeNode(1);
//	root->left->right = new TreeNode(10);
//	root->right->right = new TreeNode(7);
//	Solution2641 s;
//	TreeNode* res = s.replaceValueInTree(root);
//	// 层序输出结果
//	queue<TreeNode*> q;
//	q.push(res);
//	while (!q.empty()) {
//		TreeNode* node = q.front();
//		q.pop();
//		cout << node->val << " ";
//		if (node->left != nullptr) q.push(node->left);
//		if (node->right != nullptr) q.push(node->right);
//	}
//	// 预期输出：0 0 0 7 7 7
//	return 0;
//}
