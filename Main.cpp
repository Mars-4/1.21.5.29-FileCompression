#include<iostream>
#include<fstream>
#include<deque>
#include<algorithm>
#include<math.h>
#define Offset(num,bits,l,r) (((((unsigned)num<<(bits-l)))>>(bits-1-l))>>r)
#define ShowBits(num,size) for (int i = size; i > 0; i--){cout << ((int)Offset(num, size, i, i));}

#define FILE_EXIST 1

using namespace std;

struct Node;
class HuffmanTree;
bool operator>(const Node &a, const Node &b);
bool smaller(const Node *a, const Node *b);
int Char_size = sizeof(char) * 8;

struct Node
{
	char data;
	int index = 0;
	Node* l = NULL;
	Node* r = NULL;
	Node* parent = NULL;
};

class HuffmanTree
{
	int valid = 0;
	int repeat[256];
	int code[256];
	Node *root = NULL;
	deque<Node*> index;
	deque<Node*> _copy;
public:
	long long AfterSize = 0;
	long long BeforeSize = 0;
	~HuffmanTree()
	{
		int i = _copy.size();
		for (int n = 0; n < i; n++)
		{
			delete _copy[n];
		}
	}
	void _ReadFile(const char *str);
	void Show();
	int SaveFile(const char* source, const char* target)
	{
		//统计目标文件部分的比特位的数目
		for (int i = 0; i < 256; i++)
		{
			if (code[i] > 1)
			{
				AfterSize += (int)log(code[i])*repeat[i];
			}
		}
		//无错误返回0
		unsigned char count = 0;
		ofstream out(target, ios::binary | ios::out);

		if (!out.is_open()) return FILE_EXIST;

		out.seekp(1);
		//保存code
		for (int i = 0; i < 256; i++)
		{
			if (code[i] > 1)
			{
				count++;
				out << (char)i;
				out << code[i];
			}
		}
		//保存code组数
		out.seekp(0);
		out << count;

		//复位文件指针
		out.seekp(1 + count * (sizeof(char) + sizeof(int)));
		//****************************************************************************
		//开始写入
		ifstream in(source, ios::in | ios::binary);
		
		//输出缓冲区，一次写入1KB内容，预留4字节防止溢出
		unsigned char outBuffer[1028];
		//读取缓冲区，一次读取1KB内容
		char inbuffer[1024];
		//当前输出缓冲区写入的bit的位置
		int pos = 0;

		in.read(inbuffer, 1024);
		while (!in.eof())
		{
			for (int i = 0; i < 1024; i++)
			{
				int processingByte = code[(unsigned char)inbuffer[i]];
				while (processingByte > 1)
				{
					if (processingByte % 2)
					{
						outBuffer[pos / Char_size] |= ((1 << (Char_size - (pos % Char_size) - 1)));
					}
					else
					{
						outBuffer[pos / Char_size] &= (~(1 << (Char_size - (pos % Char_size) - 1)));
					}
					processingByte /= 2;
					pos++;
				}
				if (pos > 1024 * 8)
				{
					WriteIn(out, (char*)outBuffer);
					pos -= 1024 * 8;
					for (int i = 0; i < pos / 8; i++)
					{
						outBuffer[i] = outBuffer[1024 + i];
					}
				}
			}
			in.read(inbuffer, 1024);
		}
		for (int i = 0; i < in.gcount(); i++)
		{
			int processingByte = code[((unsigned char)inbuffer[i])];
			while (processingByte > 1)
			{
				if (processingByte % 2)
				{
					outBuffer[pos / Char_size] |= ((1 << (Char_size - (pos % Char_size) - 1)));
				}
				else
				{
					outBuffer[pos / Char_size] &= (~(1 << (Char_size - (pos % Char_size) - 1)));
				}
				processingByte /= 2;
				pos++;
			}
			if (pos > 1024 * 8)
			{
				WriteIn(out, (char*)outBuffer);
				pos -= 1024 * 8;
				for (int i = 0; i < pos / 8; i++)
				{
					outBuffer[i] = outBuffer[1024 + i];
				}
			}
		}
		WriteIn(out, (char*)outBuffer, pos / 8 + 1);
		out.seekp(0,ios::end);
		//在文件末尾添加压缩大小信息
		out << AfterSize;
		in.close();
		out.close();
		//保存完毕
		return 0;
	}
	void WriteIn(ofstream& out,const char *outBuffer,int count = 1024)
	{
		out.write(outBuffer,count);
	}
	void CompressFile(const char* source, const char* target)
	{
		_ReadFile(source);
		SaveFile(source, target);
	}
};

bool smaller(const Node *a, const Node *b)
{
	return a->index < b->index;
}

bool operator>(const Node &a, const Node &b)
{
	return a.index > b.index;
}

int main(void)
{
	HuffmanTree a;
	a.CompressFile("C:\\Users\\admin\\Desktop\\zip1.bin", "C:\\Users\\admin\\Desktop\\zip2.bin");
	a.Show();
	return 0;
}



void HuffmanTree::Show()
{
	unsigned int a = 5;
	for (int i = 0; i < 256; i++)
	{
		if (code[i] > 1)
		{
			//AfterSize += (int)log(code[i])*repeat[i];
			a = code[i];
			cout << (char)i << "\t:";
			ShowBits(a, 32);
			cout << "\t" << repeat[i] << endl;
		}
	}
	cout << endl << valid << endl;
	cout << "源文件：\t" << BeforeSize / 1024.0 << "\tKB" << endl;
	cout << "目标文件：\t" << AfterSize / 8.0 / 1024.0 << "\tKB" << endl;
}
void HuffmanTree::_ReadFile(const char *str)
{
	for (int i = 0; i < 256; i++) { repeat[i] = 0; }

	ifstream in(str, ios::in | ios::binary);
	char buffer[1024];
	in.read(buffer, 1024);
	//统计
	int i;
	while (!in.eof())
	{
		BeforeSize += 1024;
		for (i = 0; i < 1024; i++)
		{
			repeat[(unsigned char)buffer[i]]++;
		}
		in.read(buffer, 1024);
	}
	BeforeSize += in.gcount();
	for (i = 0; i < in.gcount(); i++)
	{
		repeat[(unsigned char)buffer[i]]++;
	}
	in.close();

	//存储统计值
	for (i = 0; i < 256; i++)
	{
		if (repeat[i])
		{
			valid++;
			index.push_back(new Node({ (char)i, repeat[i] }));
		}
	}
	for (int n = 0, num = index.size(); n < num; n++)
	{
		_copy.push_back(index[n]);
	}
	sort(index.begin(), index.end(), smaller);
	//建树
	deque<Node*>::iterator it;
	while (index.size() > 1)
	{
		static Node* temp;
		it = index.begin();
		temp = new Node({ 0,(*it++)->index + (*it--)->index });
		//备份所有new的对象，供结束释放空间
		_copy.push_back(temp);

		temp->l = (*it++);
		temp->r = (*it++);
		temp->l->parent = temp;
		temp->r->parent = temp;
		index.erase(index.begin());
		index.erase(index.begin());
		it = index.begin();
		while (it != index.end() && index.size() > 1 && *temp > **it)
		{
			it++;
		}
		index.insert(it, temp);
	}

	//求码
	root = *index.begin();

	for (i = 0; i < 256; i++) { code[i] = 1; }

	for (int i = 0; i < valid; i++)
	{
		Node* cur = _copy[i];
		int data = (unsigned char)_copy[i]->data;
		while (cur->parent)
		{
			code[data] <<= 1;
			if (cur->parent->l == cur)
			{
				code[data]++;
			}
			cur = cur->parent;
		}
	}
}