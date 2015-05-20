#include <stdio.h>
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
 * Make new node with data passed
 * @param  _strOderID is oder id.
 * @param  _dPrice   is pricer.
 * @param  _iSize     is size.
 * @return            a new node.
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
 * Add a node into list and sort in an oder asc or desc.
 * @param _list  is the list store.
 * @param _pNode is the node wanna add into list.
 * @param _isAsc to set the oder of sort.
 */
void addList(struct NODE** _list, struct NODE* _pNode, bool _isAsc)
{
	int* x = new int[5];
	if(_pNode == NULL)
		return;

	while(*_list)
	{
		if(((*_list)->dPrice > _pNode->dPrice) == _isAsc)
			break;
		_list = &((*_list)->pNext);
	}
	_pNode->pNext = *_list;
	*_list = _pNode;
}
bool reduceSize(struct NODE** _list, char* _strOderID, int _iSize)
{
	struct NODE** cur = _list;
	while(*cur)
	{
		if(strcmp((*cur)->strOderID, _strOderID)==0)
		{
			(*cur)->iSize -= _iSize;
			if((*cur)->iSize <= 0)
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
void removeList(struct NODE* _list)
{
	while(_list)
	{
		struct NODE* del = _list;
		_list = _list->pNext;
		free(del->strOderID);
		free(del);
	}
}
double getTotal(struct NODE* _list, int _iTargetSize)
{
	int count = 0;
	double dPrice = 0;
	while(_list)
	{
		if(count + _list->iSize < _iTargetSize)
		{
			count += _list->iSize;
			dPrice += _list->iSize * _list->dPrice;
		}
		else
		{
			dPrice += (_list->dPrice * (_iTargetSize - count));
			return dPrice;
		}
		_list = _list->pNext;
	}
	return 0;
}
double toDouble(char* _s)
{
	double ret = 0;
	int m = 1;
	bool isDec = false;
	int i = 0;
	while(_s[i])
	{
		if(_s[i] == '.')
		{
			isDec = true;
		}else
		{
			ret = ret*10 + (_s[i] - '0');
			if(isDec)
				m *= 10;	
		}
		i++;
	}
	return ret/m;
}
bool compareDoubles(double _d1, double _d2)
{
	double diff = _d1 - _d2;
	return  (diff < EPSILON)&&(-diff < EPSILON);
}

int main(int argc, char** argvs)
{
	struct NODE* listBuy = NULL;
	struct NODE* listSell = NULL;
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

	if(argc != 4)
	{
		printf("The Pricer programe use 3 arguments:\n");
		printf("<target-size> <file-in> <file-out>\n");
		return 1;
	}
	fileIn = fopen(argvs[2],"r");
	if(fileIn)
	{
		iTargetSize = atoi(argvs[1]);
		fileOut = fopen(argvs[3],"w");
		while(!feof(fileIn))
		{
			strLine[0] = '\0';//Clear line before get new line.
			fgets(strLine,BUFFER,fileIn);
			if(strlen(strLine)==0)
				break;

			char* context = NULL;
			iTime = atoi(strtok(strLine," "));
			strCommand = strtok(NULL," ");
			strOderID = strtok(NULL," ");

			//Handle for command 'A' add oder to book.
			if (strcmp(strCommand,"A") == 0)
			{
				strSide = strtok(NULL," ");
				dPrice = toDouble(strtok(NULL," "));
				iSize = atoi(strtok(NULL," "));

				if(strcmp(strSide, "B")==0)
				{
					addList(&listBuy,makeNode(strOderID,dPrice,iSize),false);
					double dTotal = getTotal(listBuy, iTargetSize);
					if(dTotal > 0)
					{
						if(!compareDoubles(dTotal, dTotalS))
						{
							dTotalS = dTotal;
							printf("%d S %0.2f\n",iTime, dTotalS);
							fprintf(fileOut, "%d S %0.2f\n",iTime, dTotalS);
						}
					}
				}
				else if(strcmp(strSide, "S")==0)
				{
					addList(&listSell, makeNode(strOderID,dPrice,iSize),true);
					double dTotal = getTotal(listSell, iTargetSize);
					if(dTotal>0)
					{
						if(!compareDoubles(dTotal, dTotalB))
						{
							dTotalB = dTotal;
							printf("%d B %0.2f\n", iTime, dTotalB);
							fprintf(fileOut,"%d B %0.2f\n", iTime, dTotalB);
						}
					}
				}
			}
			else if(strcmp(strCommand, "R")==0)
			{
				iSize = atoi(strtok(NULL," "));
				if(reduceSize(&listBuy, strOderID, iSize))
				{
					double dTotal = getTotal(listBuy, iTargetSize);
					if(dTotal > 0)
					{
						if(!compareDoubles(dTotal,dTotalS))
						{
							dTotalS = dTotal;
							printf("%d S %2.0f\n",iTime, dTotalS );
							fprintf(fileOut,"%d S %2.0f\n", iTime, dTotalS);
						}
					}else if(dTotalS > 0)
					{
						dTotalS = 0;
						printf("%d S NA\n", iTime);
						fprintf(fileOut,"%d S NA\n", iTime);
					}
					if(reduceSize(&listSell, strOderID, iSize))
					{
						double dTotal = getTotal(listSell, iTargetSize);
						if(dTotal > 0)
						{
							if(!compareDoubles(dTotal,dTotalB))
							{
								dTotalB = dTotal;
								printf("%d B %2.0f\n",iTime, dTotalB );
								fprintf(fileOut,"%d B %2.0f\n", iTime, dTotalB);
							}
						}else if(dTotalB > 0)
						{
							dTotalB = 0;
							printf("%d B NA\n", iTime);
							fprintf(fileOut,"%d B NA\n", iTime);
						}
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
		return 1;
	}
	return 0;
}	
