#include <stdio.h>
//#include <conio.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER	100
#define EPSILON	0.0001

//typedef enum{ false, true } bool;

/**
*	struct NODE store data of an oder.
*/
struct NODE
{
	char* strOderID;
	double dPrice;
	int iSize;
	struct NODE* pNext;
};

/**
*	Function makeNode(...) will return a new NODE.
*/
struct NODE* makeNode(char* _strOderID, double _dPrice, int _iSize)
{
	struct NODE* node = malloc(sizeof(struct NODE));
	int size = strlen(_strOderID) + 1;
	node->strOderID = (char*)malloc(size * sizeof(char));
	strcpy(node->strOderID, _strOderID);
	node->dPrice = _dPrice;
	node->iSize = _iSize;
	node->pNext = NULL;

	return node;
}

/**
*	Function addList(...) add a new NODE to _list
*	if _isAsc == true then _pNode will add in ascending order.
*	if _isAsc == false then _pNode will add in descending order.
*/
void addList(struct NODE** _list, struct NODE* _pNode, bool _isAsc)
{
	if (_pNode == NULL)
		return;

	struct NODE** cur = _list;
	while (*cur)
	{
		if (((*cur)->dPrice > _pNode->dPrice) == _isAsc)
		{
			_pNode->pNext = *cur;
			*cur = _pNode;
			return;
		}
		cur = &((*cur)->pNext);
	}
	*cur = _pNode;
}

/**
*	Function reduceSize(...) reduce iSize of NODE have strOderID == _strOderID.
*/
bool reduceSize(struct NODE** _list, char* _strOderID, int _iSize)
{
	struct NODE** cur = _list;
	while (*cur)
	{
		if (strcmp((*cur)->strOderID,_strOderID) == 0)
		{
			(*cur)->iSize -= _iSize;

			if ((*cur)->iSize <= 0)
			{
				struct NODE* del = *cur;
				*cur = (*cur)->pNext;
				free(del->strOderID);
				free(del);
			}
			return true;
		}
		cur = &((*cur)->pNext);
	}
	return false;
}

/**
*	Function removeList(...) will remove all data of _list.
*/
void removeList(struct NODE* _list)
{
	while (_list)
	{
		struct NODE* del = _list;
		_list = _list->pNext;
		free(del->strOderID);
		free(del);
	}
}

/**
*	Function getTotal(...) will return total income or expense on iTarget shares.
*/
double getTotal(struct NODE* _list,int _iTargetSize)
{
	int count = 0;
	double dPrice = 0;
	while (_list)
	{
		if (count + _list->iSize < _iTargetSize)
		{
			count += _list->iSize;
			dPrice += _list->iSize * _list->dPrice;
		}
		else
		{
			dPrice += (_iTargetSize - count) * _list->dPrice;
			return dPrice;
		}
		_list = _list->pNext;
	}
	return 0;
}

/**
*	Function toDouble(...) convert array of char to double.
*/
double toDouble(const char* s) {
	int m = 1;
	double ret = 0;
	bool isDec = false;
	int i = 0;
	while(s[i]) {
		if (s[i] != '.')
		{
			ret = ret * 10 + (s[i] - '0');
			if (isDec)
				m *= 10;
		}
		else
		{
			isDec = true;
		}
		i++;
	}
	return ret / m;
}

/**
*	Function compareDoubles(...) compare doubles with a EPSILON. Because sometime we use _d1 == _d2 is wrong.
*/
bool compareDoubles(double _d1, double _d2)
{
	double diff = _d1 - _d2;
	return (diff < EPSILON) && (-diff < EPSILON);
}


void main(int argc, char** argvs)
{
	struct NODE* listBuy	= NULL;
	struct NODE* listSell	= NULL;
	FILE* fileIn	= NULL;
	FILE* fileOut	= NULL;
	char strLine[BUFFER];
	int iTargetSize = 0;

	char* strCommand;
	char* strOderID;
	char* strSide;
	int iTime;
	double dPrice;
	int iSize;

	double dTotalB = 0;
	double dTotalS = 0;


	/**
	*	Check arguments, if number of arguments is wrong then
	*	print help and return.
	*/
	if (argc != 4)
	{
		printf("The Pricer program using 3 arguments:\n");
		printf("<target-size> <file-in> <file-out>\n");
		return;
	}

	fileIn = fopen(argvs[2], "r");

	if (fileIn)
	{
		iTargetSize = atoi(argvs[1]);
		fileOut = fopen(argvs[3], "w");
		while (!feof(fileIn))
		{
			strLine[0] = NULL;//Clear line before get new line;
			fgets(strLine, BUFFER, fileIn);
			if (strlen(strLine) == 0)
				break;

			char* context = NULL;

			iTime = atoi(strtok(strLine, " "));
			strCommand	= strtok(NULL, " ");
			strOderID	= strtok(NULL, " ");

			//Handle for command 'A' Add Order to Book
			if (strcmp(strCommand, "A")==0)
			{
				strSide	= strtok(NULL, " ");
				dPrice = toDouble(strtok(NULL, " "));
				iSize = atoi(strtok(NULL, " "));
				
				//Add Order to Book by side 'B'
				if (strcmp(strSide, "B") == 0)
				{
					addList(&listBuy, makeNode(strOderID, dPrice, iSize), false);
					double dTotal = getTotal(listBuy, iTargetSize);
					if (dTotal > 0)
					{
						if (!compareDoubles(dTotal, dTotalS))
						{
							dTotalS = dTotal;
							printf("%d S %0.2f\n", iTime, dTotalS);
							fprintf(fileOut, "%d S %0.2f\n", iTime, dTotalS);
						}
					}
				}
				//Add Order to Book by side 'S'
				else if (strcmp(strSide, "S") == 0)
				{
					addList(&listSell, makeNode(strOderID, dPrice, iSize), true);
					double dTotal = getTotal(listSell, iTargetSize);
					if (dTotal > 0)
					{
						if (!compareDoubles(dTotal, dTotalB))
						{
							dTotalB = dTotal;
							printf("%d B %0.2f\n", iTime, dTotalB);
							fprintf(fileOut, "%d B %0.2f\n", iTime, dTotalB);
						}
					}
				}
			}
			//Handle for command 'R' Add Order to Book
			else if (strcmp(strCommand, "R") == 0)
			{
				iSize = atoi(strtok(NULL, " "));
				if (reduceSize(&listBuy, strOderID, iSize))
				{
					double dTotal = getTotal(listBuy, iTargetSize);
					if (dTotal > 0)
					{
						if (!compareDoubles(dTotalS, dTotal))
						{
							dTotalS = dTotal;
							printf("%d S %0.2f\n", iTime, dTotalS);
							fprintf(fileOut, "%d S %0.2f\n", iTime, dTotalS);
						}
					}
					else if (dTotalS > 0)
					{
						printf("%d S NA\n", iTime);
						fprintf(fileOut, "%d S NA\n", iTime);
						dTotalS = 0;
					}
				}

				if (reduceSize(&listSell, strOderID, iSize))
				{
					double dTotal = getTotal(listSell, iTargetSize);
					if (dTotal > 0)
					{
						if (!compareDoubles(dTotalB, dTotal))
						{
							dTotalB = dTotal;
							printf("%d B %0.2f\n", iTime, dTotalB);
							fprintf(fileOut, "%d B %0.2f\n", iTime, dTotalB);
						}
					}
					else if (dTotalB > 0)
					{
						printf("%d B NA\n", iTime);
						fprintf(fileOut,"%d B NA\n", iTime);
						dTotalB = 0;
					}

				}
			}
		}
		fclose(fileIn);
		fclose(fileOut);

		removeList(listBuy);
		removeList(listSell);
	}
	else
	{
		printf("Could not find file: %s\n", argvs[2]);
		return;
	}
}