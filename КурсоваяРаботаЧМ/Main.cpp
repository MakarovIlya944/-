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
	//FName - название файла с расширением
	/*bool Save(const string FName)
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
	*/

};

/*
===============================================Структура файла net.txt===============================================
Nr
r1 r2 r3 ... rNr
Nz
z1 z2 z3 ...
step
hk1 kr1 hk2 kr2 ... hk(Nr-1) kr(Nr-1)
hk1 kz1 hk2 kz2 ...
Nw
W r_left z_left r_right z_right
...
=============================================Структура файла border.txt==============================================
N
type NumBorder r_left z_left r_right z_right
...
*/

class NetGenerator
{
public:
	//rNum - кол-во узлов по r; zNum - кол-во узлов по z
	int rNum, zNum;
	//Общее число узлов
	int Num;
	//Min - точка левого нижнего узла всей области
	Pointd Min;
	//Max - точка правая верхняя узла всей области
	Pointd Max;
	//GlobalNet - общая сетка (r,z)
	Pointd *GlobalNet;
	//nvtr - массив элементов
	int *nvtr[4];
	//nvcat - массив материалов
	int *nvcat;
	//nvr1 - массив первых краевых
	list<int> nvr1;
	//nvr2 - массив вторых краевых
	list<int> nvr2;
	//nvr3 - массив третьих краевых
	list<int> nvr3;
	//isReady - готовы ли массивы данных
	bool isReady = false;

	//FNameN - файл net.txt , FNameB - файл border.txt
	void Load(const char*FNameN, const char*FNameB)
	{
		//ifstream in(FName, ios::binary);
		ifstream in(FNameN);

		int nr, nz;
		in >> nr;
		double *R = new double[nr];
		for (int i(0); i < nr; i++)
			in >> R[i];

		in >> nz;
		double *Z = new double[nz];
		for (int i(0); i < nz; i++)
			in >> Z[i];


		double step;
		in >> step;
		double *kr = new double[nr - 1], *kz = new double[nz - 1];
		int *hr = new int[nr - 1], nhr(0), *hz = new int[nz - 1], nhz(0);
		for (int i(0); i < nr - 1; i++)
		{
			in >> hr[i];
			in >> kr[i];
			nhr += hr[i];
		}
		for (int i(0); i < nz - 1; i++)
		{
			in >> hz[i];
			in >> kz[i];
			nhz += hz[i];
		}

		double *Rh = new double[nhr + 1], *Zh = new double[nhz + 1];
		for (int i(0), k(0); i < nr - 1; i++)
		{
			for (int j(0); j < hr[i]; j++, k++)
				Rh[k] = R[i] + j*step*kr[i];
			hr[i] += i != 0 ? hr[i - 1] : 0;
		}
		Rh[nhr] = R[nr - 1];

		for (int i(0), k(0); i < nz - 1; i++)
		{
			for (int j(0); j < hz[i]; j++, k++)
				Zh[k] = Z[i] + j*step*kz[i];
			hz[i] += i != 0 ? hz[i - 1] : 0;
		}
		Zh[nhz] = Z[nz - 1];

		delete R;
		delete Z;

		rNum = nhr + 1;
		zNum = nhz + 1;
		Num = rNum*zNum;
		GlobalNet = new Pointd[Num];
		for (int i(0); i < zNum; i++)
			for (int j(0); j < rNum; j++)
				GlobalNet[i*rNum + j] = Pointd(Rh[j], Zh[i]);

		GenerationNVTR();

		int nw;
		in >> nw;
		int *W = new int[nw];
		Pointi *area = new Pointi[nw * 2]{};
		for (int i(0), j(0); i < nw; i++)
		{
			in >> W[i];
			in >> area[j].r;
			in >> area[j++].z;
			in >> area[j].r;
			in >> area[j++].z;
		}

		in.close();

		in.open(FNameB);

		int nb;
		in >> nb;
		int *B = new int[nb];
		Pointi *border = new Pointi[nb * 2]{};
		for (int i(0), j(0); i < nb; i++)
		{
			in >> B[i];
			in >> border[j].r;
			in >> border[j].z;
			border[j].r -= 1;
			border[j++].z -= 1;
			in >> border[j].r;
			in >> border[j].z;
			border[j].r -= 1;
			border[j++].z -= 1;
		}

		// Добавление краевых работает
		for (int i(0), j(0); i < nw; i++, j++)
		{
			list<int> *iterator = &nvr1;

			switch (B[i])
			{
			case 2:
				iterator = &nvr2;
				break;
			case 3:
				iterator = &nvr3;
			}

			if (border[j].r == border[j+1].r)
			{
				int l = border[j].z != 0 ? hz[border[j].z-1] : 0;
				int k = border[j].r != 0 ? hr[border[j].r-1] : 0;
				int nl(hz[border[++j].z-1]);
				for(;l<=nl;l++)
					iterator->push_back(l*rNum + k);
			}
			else
			{
				int l = border[j].r != 0 ? hr[border[j].r-1] : 0;
				int k = border[j].z != 0 ? hz[border[j].z-1] : 0;
				int nl(hr[border[++j].r-1]);
				for (; l<=nl; l++)
					iterator->push_back(k*rNum + l);
			}
			iterator->push_back(-1);
		}

		delete Rh;
		delete Zh;
		delete hz;
		delete hr;
		delete kr;
		delete kz;
		delete W;
		delete border;

		in.close();
	}

	//Генерирует массив элементов
	void GenerationNVTR()
	{
		int n = (rNum - 1)*(zNum - 1);
		for (int i(0); i<4; i++)
			nvtr[i] = new int[n];
		nvcat = new int[n] {};

		for (int z(0); z < zNum - 1; z++)
			for (int r(0); r < rNum - 1; r++)
			{
				int current = r + z*(rNum - 1), index = r + z*rNum;
				nvtr[0][current] = index;
				nvtr[1][current] = index + 1;

				nvtr[2][current] = index + rNum;
				nvtr[3][current] = index + rNum + 1;
				//nvcat[current] = 1;
			}
	}

	//Добовляет материал
	//*elements - массив номеров элементов, отсчет с 0
	//n - кол-во элементов
	//material - номер материала
	void AddMaterail(int *elements, int n, int material)
	{
		for (int i(0); i < n; i++)
			nvcat[elements[i] - 1] = material;
	}

	//Добавление краевых условий
	//a,b - номера узлов между которымы заданы начальные условия, отсчет с 0
	//c - номер краевых условий
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

	//Выводит на консоль глобальную сетку узлов
	void PrintGlobalNet()
	{
		for (int z(zNum - 1); z >= 0; z--)
		{
			for (int r(0); r < rNum; r++)
				printf_s("%.1f|%.1f ", GlobalNet[z*rNum + r].r, GlobalNet[z*rNum + r].z);
			printf_s("\n");
		}
	}

	//Выводит на консоль массив элементов
	void PrintNVTR()
	{
		int num = (rNum - 1)*(zNum - 1) - 1;
		for (int j(0); j < num; j++)
			printf_s("%d %d %d %d\n", nvtr[0][j], nvtr[1][j], nvtr[2][j], nvtr[3][j]);
	}

	//Выводит на консоль краевые условия
	void PrintBorder()
	{
		cout << "Первые краевые" << endl;
		copy(nvr1.begin(), nvr1.end(), ostream_iterator<int>(cout, "\n"));
		cout << endl << "Вторые краевые";
		copy(nvr2.begin(), nvr2.end(), ostream_iterator<int>(cout, "\n"));
		cout << endl << "Третьи краевые";
		copy(nvr3.begin(), nvr3.end(), ostream_iterator<int>(cout, "\n"));
	}
};

void main()
{
	setlocale(LC_ALL, "Russian");
	NetGenerator gen;
	gen.Load("net.txt", "border.txt");
	int y = 0;
}