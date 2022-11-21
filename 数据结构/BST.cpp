//BST
#include <iostream>
#include <functional>
#include <stack>
#include <queue>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

template<typename T, typename Comp = less<T>>
class BSTree
{
public:
	//初始化根节点和函数对象+lambda表达式
	BSTree(Comp comp = Comp())
		:root_(nullptr)
		, comp_(comp)
	{}
	//层序遍历思想释放BST树所有节点资源
	~BSTree()
	{
		if (root_ != nullptr)
		{
			queue<Node*>q;
			q.push(root_);
			while (q.size())
			{
				Node* front = q.front();
				q.pop();
				if (front->left != nullptr) {
					q.push(front->left);
				}
				if (front->right != nullptr) {
					q.push(front->right);
				}
				delete front;
			}
		}
	}
	//非递归插入操作
	void n_insert(const T& val)
	{
		if (root_ == nullptr)
		{
			root_ = new Node(val);
			return;
		}
		//寻找合适的插入位置
		Node* parent = nullptr;
		Node* cur = root_;
		while (cur != nullptr)
		{
			if (cur->data_ == val) {
				return;
			} 
			else if (comp_(cur->val, val)) {
				parent = cur;
				cur = cur->right_;
			}
			else {
				parent = cur;
				cur = cur->left_;
			}
		}

		if (comp_(val, parent->data_))
		{
			parent->left = new Node(val);
		}
		else
		{
			parent->right = new Node(val);
		}
	}
	//递归插入操作 用户调用接口 后面会有实现接口
	void insert(const T& val)
	{
		root_ = insert(root_, val);
	}
	//非递归删除操作
	void n_remove(const T& val)
	{
		if (root_ == nullptr)
		{
			return;
		}
		Node* parent = nullptr;
		Node* cur = root_;
		while (cur != nullptr)
		{
			if (cur->data == val)
			{
				break; //找到了待删除点
			}
			else if (comp_(cur->data_, val))
			{
				parent = cur;
				cur = cur->righ_t;
			}
			else
			{
				parent = cur;
				cur = cur->left_;
			}
		}
		//没找到待删除的节点
		if (cur == nullptr) return;
		//情况三 左右孩子都有
		if (cur->left_ != nullptr && cur->right_ != nullptr)
		{
			parent = cur;
			Node* pre = cur->left_;
			while (pre->right != nullptr)
			{
				parent = pre;
				pre = pre->right_;
			}
			cur->data_ = pre->data_;
			cur = pre;
		}
		//cur此时是待删除节点 parent是其父节点
		Node* child = cur->left_;
		if (child == nullptr)
		{
			child = cur->right_;
		}
		//特殊情况 删除的是根节点
		if (parent == nullptr)
		{
			root_ = child;
		}
		else
		{
			if (parent->left_ == cur)
			{
				parent->left_ = child;
			}
			else
			{
				parent->right_ = child;
			}
		}
		delete cur;
	}
	//递归删除
	void remove(const T& val)
	{
		root_ = remove(root_, val);
	}
	//非递归查询
	bool n_query(const T& val)
	{
		Node* cur = root_;
		while (cur != nullptr)
		{
			if (cur->data == val)
			{
				return true;
			}
			else if (comp_(cur->data_, val))
			{
				cur = cur->right_;
			}
			else
			{
				cur = cur->left_;
			}
		}
		return false;
	}
	//递归查询
	bool query(const T& val)
	{
		return nullptr != query(root_, val);
	}
	// 递归前序遍历操作
	void preOrder()
	{
		cout << "[递归]前序遍历:";
		preOrder(root_);
		cout << endl;
	}
	// 非递归前序遍历操作
	void n_preOrder()
	{
		cout << "非递归前序遍历操作: ";
		if (root_ == nullptr)
		{
			return;
		}
		//先入右节点再入左节点
		stack<Node*>s;
		s.push(root_);
		while (!s.empty())
		{
			Node* top = s.top();
			s.pop();
			cout << top->data_ << " ";
			if (top->right_ != nullptr)
			{
				s.push(top->right_);
			}
			if (top->left_ != nullptr)
			{
				s.push(top->left_);
			}
		}
		cout << endl;
	}
	// 递归中序遍历操作
	void inOrder()
	{
		cout << "[递归]中序遍历:";
		inOrder(root_);
		cout << endl;
	}
	// 非递归中序遍历操作
	void n_inOrder()
	{
		cout << "[非递归]中序遍历:";
		if (root_ == nullptr)
		{
			return;
		}
		stack<Node*>s;
		Node* cur = root_;
		while (!s.empty() || cur != nullptr)
		{
			if (cur != nullptr)
			{
				s.push(cur);
				cur = cur->left_;
			}
			else
			{
				Node* top = s.top();
				s.pop();
				cout << top->data_ << " ";
				cur = top->right_;
			}
		}
		cout << endl;
	}
	// 递归后序遍历操作
	void postOrder()
	{
		cout << "[递归]后序遍历:";
		postOrder(root_);
		cout << endl;
	}
	// 非递归后序遍历操作
	void n_postOrder()
	{
		cout << "[非递归]后序遍历:";
		if (root_ == nullptr)
		{
			return;
		}
		stack<Node*>s1, s2;
		s1.push(root_);
		//先左后右入栈 -> 先右后左出栈
		while (!s1.empty())
		{
			Node* top = s1.top();
			s1.pop();
			s2.push(top);
			if (top->left_ != nullptr)
			{
				s1.push(top->left_);
			}
			if (top->right_ != nullptr)
			{
				s1.push(top->right);
			}
		}
		while (!s2.empty())
		{
			cout << s2.top()->data_ << " ";
			s2.pop();
		}
		cout << endl;
	}
	//递归层序遍历
	void levelOrder()
	{
		cout << "[递归]层序遍历:";
		int h = high(); // 树的层数
		for (int i = 0; i < h; ++i)
		{
			levelOrder(root_, i); // 递归调用树的层数次
		}
		cout << endl;
	}
	// 非递归层序遍历操作
	void n_levelOrder()
	{
		cout << "[非递归]层序遍历:";
		if (root_ == nullptr)
		{
			return;
		}
		queue<Node*> que;
		que.push(root_);
		while (!que.empty())
		{
			Node* front = que.front();
			que.pop();
			cout << front->data_ << " ";
			if (front->left != nullptr)
			{
				que.push(front->left_);
			}
			if (front->right != nullptr);
			{
				que.push(front->right_);
			}
		}
		cout << endl;
	}
	// 递归求二叉树层数
	int high()
	{
		return high(root_);
	}
	// 递归求二叉树节点个数
	int number()
	{
		return number(root_);
	}
	// 求满足区间的元素值[i, j]
	void findValues(vector<T>& vec, int i, int j)
	{
		findValues(root_, vec, i, j);
	}
	// 判断二叉树是否是BST树
	bool isBSTree()
	{
		Node* pre = nullptr;
		return isBSTree(root_, pre);
	}
	// 判断子树问题
	bool isChildTree(BSTree<T, Comp>& child)
	{
		// 在当前二叉树上找child的根节点
		if (child.root_ == nullptr)
		{
			return true;
		}
		Node* cur = root_;
		while (cur != nullptr)
		{
			if (cur->data_ == child.root_->data)
			{
				break;
			}
			else if (comp_(cur->data_, child.root_->data_))
			{
				cur = cur->right_;
			}
			else
			{
				cur = cur->left_;
			}
		}
		if (cur == nullptr)
		{
			return false;
		}
		return isChildTree(cur, child.root_); //其它地方实现的
	}
	// 最近公共祖先节点
	int getLCA(int val1, int val2)
	{
		Node* node = getLCA(root_, val1, val2);
		if (node == nullptr)
		{
			throw "no LCA!";
		}
		else
		{
			return node->data_;
		}
	}
	// 镜像翻转
	void mirror01()
	{
		mirror01(root_);
	}
	// 镜像对称
	bool mirror02()
	{
		if (root_ == nullptr)
			return true;
		return mirror02(root_->left_, root_->right_);
	}
	// 重建二叉树
	void rebuild(int pre[], int i, int j, int in[], int m, int n)
	{
		root_ = _rebuild(pre, i, j, in, m, n);
	}
	// 判断平衡树
	bool isBalance()
	{
		int l = 0;
		bool flag = true;
		isBalance02(root_, l, flag);
		return flag;
	}
	// 求中序倒数第K个节点
	int getVal(int k)
	{
		Node* node = getVal(root_, k);
		if (node == nullptr)
		{
			string err = "no No.";
			err += k;
			throw err;
		}
		else
		{
			return node->data_;
		}
	}

private:
	struct Node
	{
		Node(T data = T())
			:data_(data)
			, left_(nullptr)
			, right_(nullptr)
		{}
		T data_;
		Node* left_;
		Node* right_;
	};
	Node* root_;
	Comp comp_;

	// 递归前序遍历的实现 VLR
	void preOrder(Node* node)
	{
		if (node != nullptr)
		{
			cout << node->data_ << " ";
			preOrder(node->left_);
			preOrder(node->right_);
		}
	}
	// 递归中序遍历的实现 LVR
	void inOrder(Node* node)
	{
		if (node != nullptr)
		{
			inOrder(node->left);
			cout << node->data_ << " ";
			inOrder(node->right);
		}
	}
	// 递归后序遍历的实现 LRV
	void postOrder(Node* node)
	{
		if (node != nullptr)
		{
			postOrder(node->left);
			postOrder(node->right);
			cout << node->data << " ";
		}
	}
	// 递归层序遍历的实现
	void levelOrder(Node* node, int i)
	{
		if (node == nullptr)
			return;
		if (i == 0)
		{
			cout << node->data_ << " ";
			return;
		}
		levelOrder(node->left_, i - 1);
		levelOrder(node->right_, i - 1);
	}
	// 递归实现求二叉树层数 求以node为根节点的子树的高度并返回高度值
	int high(Node* node)
	{
		if (node == nullptr)
		{
			return 0;
		}
		int left = high(node->left);
		int right = high(node->right);
		return left > right ? left + 1 : right + 1;
	}
	// 递归求二叉树节点个数的实现 求以node为根节点的树的节点总数，并返回
	int number(Node* node)
	{
		if (node == nullptr)
			return 0;
		int left = number(node->left_);
		int right = number(node->right_);
		return left + right + 1;
	}
	// 递归插入操作实现
	Node* insert(Node* node, const T& val)
	{
		if (node == nullptr)
		{
			// 递归结束，找到插入val的位置，生成新节点并返回其节点地址
			return new Node(val);
		}
		if (node->data_ == val)
		{
			return node;
		}
		else if (comp_(node->data_, val))
		{
			node->right_ = insert(node->right_, val);
		}
		else
		{
			node->left_ = insert(node->left_, val);
		}
		return node;
	}
	// 递归查询操作实现
	Node* query(Node* node, const T& val)
	{
		if (node == nullptr)
			return nullptr;
		if (node->data_ == val)
		{
			return node;
		}
		else if (comp_(node->data_, val))
		{
			return query(node->right_, val);
		}
		else
		{
			return query(node->left_, val);
		}
	}
	// 递归删除操作实现
	Node* remove(Node* node, const T& val)
	{
		if (node == nullptr)
			return nullptr;

		if (node->data_ == val) // 找到待删除节点
		{
			// 情况3
			if (node->left_ != nullptr && node->right_ != nullptr)
			{
				// 找前驱节点
				Node* pre = node->left_;
				while (pre->right_ != nullptr)
				{
					pre = pre->right_;
				}
				node->data_ = pre->data_;
				// 通过递归直接删除前驱节点 
				node->left_ = remove(node->left_, pre->data_);
			}
			else // 情况1和情况2
			{
				if (node->left_ != nullptr)
				{
					// 删除节点以后，把非空的左孩子返回，回溯时更新其父节点地址域
					Node* left = node->left_;
					delete node;
					return left;
				}
				else if (node->right_ != nullptr)
				{
					// 删除节点以后，把非空的右孩子返回，回溯时更新其父节点地址域
					Node* right = node->right_;
					delete node;
					return right;
				}
				else // 删除的是没有孩子的节点  叶子节点
				{
					delete node;
					return nullptr; // 回溯时更新其父节点地址域为nullptr
				}
			}
		}
		else if (comp_(node->data_, val))
		{
			node->right_ = remove(node->right_, val);
		}
		else
		{
			node->left_ = remove(node->left_, val);
		}
		return node; // 把当前节点返回给父节点，更新父节点相应的地址域
	}
	// 求满足区间的元素值[i, j]实现
	void findValues(Node* node, vector<T>& vec, int i, int j)
	{
		if (node != nullptr)
		{
			// 在当前节点的左子树中搜索
			if (node->data_ > i)
			{
				findValues(node->left_, vec, i, j); // L
			}

			if (node->data_ >= i && node->data_ <= j)// V
			{
				vec.push_back(node->data_); // 存储满足区间元素的值
			}

			// 在当前节点的右子树中搜索
			if (node->data_ < j)
			{
				findValues(node->right_, vec, i, j); // R
			}
		}
	}
	// 判断二叉树是否是BST树的实现 利用BST树中序遍历是一个升序的特点
	bool isBSTree(Node* node, Node*& pre)
	{
		if (node == nullptr)
		{
			return true;
		}

		if (!isBSTree(node->left_, pre)) // L 主要判断使递归结束的条件
		{
			return false;
		}
		// V
		if (pre != nullptr)
		{
			if (comp_(node->data_, pre->data_)) // 主要判断使递归结束的条件
			{
				return false;
			}
		}
		pre = node; // 更新中序遍历的前驱节点

		return isBSTree(node->right_, pre); // R
	}
	// 判断子树问题实现
	bool isChildTree(Node* father, Node* child)
	{
		if (father == nullptr && child == nullptr)
		{
			return true;
		}

		if (father == nullptr) // 子树里面有的节点，当前二叉树没有
		{
			return false;
		}

		if (child == nullptr)
		{
			return true;
		}

		// 判断值不相同
		if (father->data_ != child->data_)  // V
		{
			return false;
		}

		return isChildTree(father->left_, child->left_) // L
			&& isChildTree(father->right_, child->right_); // R
	}
	// 最近公共祖先节点实现
	Node* getLCA(Node* node, int val1, int val2)
	{
		if (node == nullptr)
		{
			return nullptr;
		}

		if (comp_(node->data_, val1) && comp_(node->data_, val2))
		{
			return getLCA(node->right_, val1, val2);
		}
		else if (comp_(val1, node->data_) && comp_(val2, node->data_))
		{
			return getLCA(node->left_, val1, val2);
		}
		else
		{
			return node;
		}
	}
	// 镜像翻转
	void mirror01(Node* node)
	{
		if (node == nullptr)
			return;

		// V
		Node* tmp = node->left_;
		node->left_ = node->right_;
		node->right_ = tmp;

		mirror01(node->left_);  // L
		mirror01(node->right_); // R
	}
	// 镜像对称
	bool mirror02(Node* node1, Node* node2)
	{
		if (node1 == nullptr && node2 == nullptr)
		{
			return true;
		}
		if (node1 == nullptr || node2 == nullptr)
		{
			return false;
		}
		if (node1->data_ != node2->data_)
		{
			return false;
		}
		return mirror02(node1->left_, node2->right_)
			&& mirror02(node1->right_, node2->left_);
	}
	// 重建二叉树递归实现
	Node* _rebuild(int pre[], int i, int j, int in[], int m, int n)
	{
		if (i > j || m > n)
		{
			return nullptr;
		}

		// 创建当前子树的根节点
		Node* node = new Node(pre[i]); // 拿前序的第一个数字创建子树根节点  V
		for (int k = m; k <= n; ++k)
		{
			if (pre[i] == in[k]) // 在中序遍历中找子树根节点的下标k
			{
				node->left_ = _rebuild(pre, i + 1, i + (k - m), in, m, k - 1); // L
				node->right_ = _rebuild(pre, i + (k - m) + 1, j, in, k + 1, n); // R
				return node;
			}
		}
		return node;
	}
	// 判断平衡树 效率比较低
	bool isBalance(Node* node)
	{
		if (node == nullptr)
			return true;
		if (!isBalance(node->left_))  // L
			return false;
		if (!isBalance(node->right_))  // R
			return false;

		int left = high(node->left_);
		int right = high(node->right_);
		return abs(left - right) <= 1;   // V
	}
	// 判断平衡树 效率高 递归过程中，记录了节点的高度值  返回节点高度值
	int isBalance02(Node* node, int l, bool& flag)
	{
		if (node == nullptr)
		{
			return l;
		}

		int left = isBalance02(node->left_, l + 1, flag); // L
		if (!flag)
			return left;
		int right = isBalance02(node->right_, l + 1, flag); // R
		if (!flag)
			return right;

		// V
		if (abs(left - right) > 1) // 节点失衡了
		{
			flag = false;
		}
		return max(left, right);
	}
	// 求中序倒数第K个节点
	int i = 1;
	Node* getVal(Node* node, int k)
	{
		if (node == nullptr)
			return nullptr;

		Node* left = getVal(node->right_, k); // R
		if (left != nullptr)
			return left;
		// V
		if (i++ == k) // 在VRL的顺序下，找到正数第k个元素
		{
			return node;
		}
		return getVal(node->left_, k); // L
	}
};

int main()
{
	return 0;
}