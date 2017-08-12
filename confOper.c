/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-01 18:15
* Last modified: 2017-08-12 15:34
* Email        : 374648064@qq.com
* Filename     : confOper.c
* Description  : 
******************************************************/

#include "confOper.h"

static char subStr2[2][MAX_SUBSTR];
static char lineData[LINE_CHAR_MAX_NUM];

bool getConfValueByLabelAndKey(char *label, char *key, char *value)
{
	int fd = OpenFile(CONFIG_PATH, O_RDONLY);
	if(fd == -1)
	{
		return false;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	bool point = false;
	char *temp_str = NULL;
	while(ReadLine(fd, lineData) == -1)
	{
		removeBeginSpace(lineData);
		if(lineData[0] != CONFIG_NOTESYMBOL)
		{
			if(lineData[0] == '[' && lineData[strlen(lineData)-1] == ']')
			{
				if(point)
					break;
				//判断是label
				lineData[strlen(lineData)-1] = '\0';
				temp_str = &(lineData[1]);
				if(strcasecmp(temp_str, label) == 0)
				{
					memset(lineData, 0, LINE_CHAR_MAX_NUM);
					point = true;
					continue;
				}
			}
			if(point)
			{
				//是要查找的label域
				//判断不是注释行，则提取key与value
				cutStrByLabel(lineData, '=', subStr2, 2);
				removeChar(subStr2[0], ' ');
				//判断是否为要被提取的配置项
				if(strcasecmp(key, subStr2[0]) == 0)
				{
					removeChar(subStr2[1], ' ');
					if(strlen(subStr2[1]) >= CONFIG_VALUE_MAX_NUM)
					{
						CloseFile(fd);
						return false;
					}
					strcpy(value, subStr2[1]);
					CloseFile(fd);
					return true;
				}
			}
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	CloseFile(fd);
	return false;
}

