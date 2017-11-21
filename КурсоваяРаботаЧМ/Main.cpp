#include <fstream>
#include <iostream>
#include <list>
#include <iterator>
#define byte char;

using namespace std;

struct Pointd
{
	double r;
	double z;

	Pointd()
	{
		r = 0;
		z = 0;
	}

	Pointd(double R, double Z)
	{
		r = R;
		z = Z;
	}

	Pointd operator +(Pointd a)
	{
		return Pointd(r + a.r, z + a.z);
	}

	Pointd operator -(Pointd a)
	{
		return Pointd(r - a.r, z - a.z);
	}

};

struct Pointi
{
	int r;
	int z;
};

class FileGenerator
{
public:
	//FName - �������� ����� � �����������
	void Save(const string FName)
	{
		string type = FName.substr(FName.length() - 3, 3);
		ofstream out;
		if (type == "bin")
		{
			out = ofstream(FName, ios::binary);
		}
		else if (type == "txt")
		{

		}
		else
		{

		}
	}


};

/*��������� �����

*/

class NetGenerator
{
public :
	//rNum - ���-�� ����� �� r; zNum - ���-�� ����� �� z
	int rNum, zNum;
	//����� ����� �����
	int Num;
	//Min - ����� ������ ������� ���� ���� �������
	Pointd Min;
	//Max - ����� ������ ������� ���� ���� �������
	Pointd Max;
	//GlobalNet - ����� ����� (r,z)
	Pointd *GlobalNet;
	//nvtr - ������ ���������
	int *nvtr[4];
	//nvcat - ������ ����������
	int *nvcat;
	//nvr1 - ������ ������ �������
	list<int> nvr1;
	//nvr2 - ������ ������ �������
	list<int> nvr2;
	//nvr3 - ������ ������� �������
	list<int> nvr3;
	//isReady - ������ �� ������� ������
	bool isReady = false;

	//FName - ���� � �����
	bool Save(const string FName)  
	{
		if (!isReady)
			return false;

		string type = FName.substr(FName.length() - 3, 3);
		ofstream out;
		if (type == "bin")
		{
			out = ofstream(FName, ios::binary);
		}
		else if (type == "txt")
		{
			out = ofstream(FName);
			out << "Global net\n";

			for (int z(zNum - 1); z >= 0; z--)
			{
				for (int r(0); r < rNum; r++)
					out << GlobalNet[z*rNum + r].r << '|' << GlobalNet[z*rNum + r].z << ' ';
				out << endl;
			}

			out << "NVTR\n";
			for (int i(0), n = (rNum - 1)*(zNum - 1); i < n; i++)
				out << nvtr[0][i] << ' ' << nvtr[1][i] << ' ' << nvtr[2][i] << ' ' << nvtr[3][i] << endl;

			out << "NVCAT\n";
			for (int i(0), n = (rNum - 1)*(zNum - 1), k = 1; i < n; i++)
				out << i << ' ' << nvcat[i] << endl;

			out << "NVR1\n";
			int k = 1;
			for (auto it = nvr1.begin(); it != nvr1.end(); it++)
				if (*it == -1)
					k++;
				else
					out << *it << ' ' << k << endl;

			out << "NVR2\n";
			k = 1;
			for (auto it = nvr2.begin(); it != nvr2.end(); it++)
				if (*it == -1)
					k++;
				else
					out << *it << ' ' << k << endl;

			out << "NVR3\n";
			k = 1;
			for (auto it = nvr3.begin(); it != nvr3.end(); it++)
				if (*it == -1)
					k++;
				else
					out << *it << ' ' << k << endl;
		}
		else
			return false;
		
		out.close();
		return true;
	};

	//FName - ���� � �����
	void Load(const char*FName)
	{
		ifstream in(FName, ios::binary);

		in.close();
	}

	//������� ����� ����� (r,z)
	//min - ����� ������ �����; max - ������ �������;
	//rnum - ���-�� �������� �� r; znum - ���-�� �������� �� z;
	bool GenerationGlobalNet(Pointd min, Pointd max, int rnum, int znum)
	{
		if (max.r <= min.r || max.z <= min.z || rnum < 1 || znum < 1)
			return false;

		rNum = rnum + 1;
		zNum = znum + 1;
		Num = rNum + zNum;

		double dr = (max.r - min.r) / rnum, dz = (max.z - min.z) / znum;
		double tmpR = min.r;
		GlobalNet = new Pointd[rNum*zNum];
		for (int z(0); z < zNum; z++)
		{
			double tmpZ = min.z + z*dz;
			for (int r(0); r < rNum; r++)
				GlobalNet[z*rNum + r] = Pointd(tmpR + r*dr, tmpZ);
		}
	}

	//���������� ������ ���������
	void GenerationNVTR()
	{
		int n = (rNum - 1)*(zNum - 1);
		for(int i(0);i<4;i++)
			nvtr[i] = new int[n];
		nvcat = new int[n];

		for (int z(0); z < zNum-1; z++)
			for (int r(0); r < rNum - 1; r++)
			{
				int current = r + z*(rNum - 1), index = r + z*rNum;
				nvtr[0][current] = index;
				nvtr[1][current] = index + 1;

				nvtr[2][current] = index + rNum;
				nvtr[3][current] = index + rNum + 1;
				nvcat[current] = 1;
			}
	}

	//��������� ��������
	//*elements - ������ ������� ���������, ������ � 0
	//n - ���-�� ���������
	//material - ����� ���������
	void AddMaterail(int *elements, int n, int material)
	{
		for (int i(0); i < n; i++)
			nvcat[elements[i]-1] = material;
	}

	//���������� ������� �������
	//a,b - ������ ����� ����� �������� ������ ��������� �������, ������ � 0
	//c - ����� ������� �������
	bool AddBorder(int a, int b, int c)
	{
		if (a < 0 || b < 0 || a == b || a >= rNum*zNum || b >= rNum*zNum || c < 1 || c > 4)
			return false;

		if (a > b)
			swap(a, b);

		list<int> *iterator = &nvr1;

		switch (c)
		{
		case 2:
			iterator = &nvr2;
			break;
		case 3:
			iterator = &nvr3;
		}

		if ((a < rNum && b < rNum) || (a > Num - rNum && b > Num - rNum))
			for (int i(a); i <= b; i++) iterator->push_back(i);

		if (a%rNum == 0 && b%rNum == 0 || a%rNum == rNum - 1 && b%rNum == rNum - 1)
			for (int i(a); i <= b; i += rNum) iterator->push_back(i);

		iterator->push_back(-1);
	}

	//������� �� ������� ���������� ����� �����
	void PrintGlobalNet()
	{
		for (int z(zNum - 1); z >= 0; z--)
		{
			for (int r(0); r < rNum; r++)
				printf_s("%.1f|%.1f ", GlobalNet[z*rNum + r].r, GlobalNet[z*rNum + r].z);
			printf_s("\n");
		}
	}

	//������� �� ������� ������ ���������
	void PrintNVTR()
	{
		int num = (rNum - 1)*(zNum - 1) - 1;
		for (int j(0); j < num; j++)
			printf_s("%d %d %d %d\n", nvtr[0][j], nvtr[1][j], nvtr[2][j], nvtr[3][j]);
	}

	//������� �� ������� ������� �������
	void PrintBorder()
	{
		cout << "������ �������" << endl;
		copy(nvr1.begin(), nvr1.end(), ostream_iterator<int>(cout, "\n"));
		cout <<  endl << "������ �������";
		copy(nvr2.begin(), nvr2.end(), ostream_iterator<int>(cout, "\n"));
		cout  << endl << "������ �������";
		copy(nvr3.begin(), nvr3.end(), ostream_iterator<int>(cout, "\n"));
	}
};

void main()
{
	setlocale(LC_ALL, "Russian");
	NetGenerator gen;
	gen.GenerationGlobalNet({ 0,0 }, { 15,20 }, 5, 4);
	gen.GenerationNVTR();
	int elem[6]{ 1,2,3,5,6,7 };
	gen.AddMaterail(elem, 6, 2);
	gen.AddBorder(0, 4, 1);
	gen.AddBorder(0, 5, 2);
	gen.AddBorder(4, 9, 3);
	gen.isReady = true;
	gen.Save("yt.txt");
	int y = 0;
}