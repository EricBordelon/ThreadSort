//Eric Bordelon,Group 3			 Team Members: Nicholas Mahony, Quintin Donnelly
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <functional>
#include <string>

using namespace std;

//int swap;
//mutex swapLock;

int swapCounter;
mutex countLock;

void insSort(vector<int>::iterator lhs, vector<int>::iterator rhs);
void srtMerge(vector<int>::iterator lhs, vector<int>::iterator mid, vector<int>::iterator rhs);

int main()
 {
	vector<int> data;
	vector<thread> threads;
	
	int input;

	cout << "This program sorts a file of integers using multiple threads." << endl;

	string dest;
	cout << ">  ";
	cin >> dest;

	ifstream fin;
	fin.open(dest);
	
	fin >> input;
	while (fin)
	{
		data.push_back(input);

		fin >> input;
	}

	fin.close();

	for (size_t i = 0; i < data.size(); i = i + 1024)
	{
		if (i + 1023 >= data.size())
			threads.emplace_back(insSort, data.begin() + i, data.begin() + (data.size() - 1));
		else
			threads.emplace_back(insSort, data.begin() + i, data.begin() + (i + 1023));
	}
	/*for (vector<int>::iterator i = data.begin(); i != data.end(); i = i + 1024)
	{
		if (i + 1023 >= data.end())
			threads.emplace_back(insSort, i, data.begin() + (data.size() - 1));
		else
			threads.emplace_back(insSort, i, i + 1023);
	}*/

	for (size_t i = 0; i < threads.size(); ++i)
	{
		threads[i].join();
	}
	
	int j = 1024;
	int sz = static_cast<int>(data.size());
	while (sz - j > 0)
	{
		threads.clear();

		for (int i = 0; i < sz; i = i + (2*j))
		{
			if (i + (2 * j) >= sz)
			{
				if (i + (j-1) >= sz)
					threads.emplace_back(srtMerge, data.begin() + i, data.begin() + ((i + sz) / 2), data.begin() + (sz-1));
				else
					threads.emplace_back(srtMerge, data.begin() + i, data.begin() + (i + (j - 1)), data.begin() + (sz-1));
			}
			else
				threads.emplace_back(srtMerge, data.begin() + i, data.begin() + (i + (j - 1)), data.begin() + (i + ((2 * j) - 1)));
		}

		for (size_t i = 0; i < threads.size(); ++i)
		{
			threads[i].join();
		}

		j = j * 2;
	}

	cout << "Sorting finished.";
	cout << "  The number of swaps used is: " << swapCounter << endl;
	
	ofstream fout;
	string output = "ans_" + dest;
	fout.open(output);

	for (size_t i = 0; i < data.size(); ++i)
	{
		fout << data[i] << " ";
	}

	fout << endl << "The number of swaps used to sort the array: " << swapCounter;

	fout.close();

	return 0;
}

void insSort(vector<int>::iterator lhs, vector<int>::iterator rhs)
{
	for (auto i = lhs + 1; i <= rhs; i++)
	{
		for (auto j = i; j > lhs; j--)
		{
			if (*j < *(j - 1))
			{
				std::swap(*j, *(j-1));
				
				countLock.lock();
				++swapCounter;
				countLock.unlock();
			}
			else
			{
				break;
			}
		}
	}
}


void srtMerge(vector<int>::iterator lhs, vector<int>::iterator mid, vector<int>::iterator rhs)
{
	auto lStart = lhs;
	auto lEnd = mid;

	auto rStart = mid + 1;
	auto rEnd = rhs;

	while (lStart <= lEnd && rStart <= rEnd)
	{
		if (*lStart <= *rStart)
		{
			++lStart;
		}
		else
		{
			for (auto i = rStart; i > lStart; --i)
			{
				std::swap(*i, *(i - 1));

				countLock.lock();
				++swapCounter;
				countLock.unlock();
			}

			++rStart;

			++lStart;
			++lEnd;
		}
	}
}