// ConvertingCharset.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ConvertingCharset.h"



// 按字符分割字符串,无个数限制 
void wstring_split(const wstring &strSrc, vector<wstring>&arrStrDst, wstring::value_type separator)
{
	arrStrDst.clear();
	wstring::const_iterator i = strSrc.begin(), last = i;
	for (; i != strSrc.end(); i++)
	{
		if (*i == separator)
		{
			arrStrDst.push_back(wstring(last, i));
			last = i + 1;
		}
	}
	if (last != strSrc.end())	//	加上最后的空串
	{
		arrStrDst.push_back(wstring(last, i));
	}
}



KFindingFileNames::KFindingFileNames(const TCHAR *szPath, const TCHAR* szExNames)
{
	m_strPathName = szPath;

	wstring_split(szExNames, m_vExNames, _T('|'));


	m_vFileResults.reserve(3000);
 
}



KFindingFileNames::~KFindingFileNames()
{
}
 



void KFindingFileNames::FindFileList(vector<wstring>& vFiles)
{

	m_vFileResults.clear();

	//FindFiles(m_strPathName.c_str());

	FindFilesNonRecursion(m_strPathName.c_str());


	vFiles = m_vFileResults;
}


//  测试扩展名, 小写 ".cpp"
bool KFindingFileNames::CheckFileExName(const TCHAR * szFileName, const TCHAR * pszExName)
{
 
	int len = _tcslen(szFileName);

	int iExLen = _tcslen(pszExName);

	if(len - iExLen > 0)
	{
		//	取后位扩展名
		TCHAR* pExFileName =  (TCHAR*)szFileName + len - iExLen;

		TCHAR szRealExName[32] = {0};

		// 全部转小写
		for(int i = 0; i < iExLen; i++)
		{
			szRealExName[i] =  _tolower (pExFileName[i]);
		}
 

		if(_tcsncmp (pExFileName, pszExName, iExLen) == 0)
		{
			return true;
		}
	}

	return false;

}


bool KFindingFileNames::CheckFileExNameList(const TCHAR * szFileName, vector<wstring>& vExNames)
{
	if(vExNames.empty())
	{
		return true;
	}


	bool ret = false;

	for(int i = 0; i < vExNames.size(); i++)
	{
		if(CheckFileExName(szFileName, vExNames[i].c_str()))
		{
			ret = true;
			break;
		}
	}

	return ret;

}


 

// 递归查找文件
void KFindingFileNames::FindFiles(const TCHAR *szPath)
{
	TCHAR szFind[MAX_PATH] = {0};
	

	WIN32_FIND_DATA findData;
	_tcscpy(szFind, szPath);
 	_tcscat(szFind, _T("\\*.*"));

	HANDLE hFind =  FindFirstFile(szFind,&findData);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}
	
	while(true)
	{
		if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(findData.cFileName[0] != _T('.'))
			{
				TCHAR szSubDir[MAX_PATH] = {0};

				_tcscpy(szSubDir,szPath);
				_tcscat(szSubDir, _T("\\"));
				_tcscat(szSubDir, findData.cFileName); 
				FindFiles(szSubDir);
			}
		}
		else
		{
			if(CheckFileExNameList(findData.cFileName, m_vExNames))
			{
				wstring strName = szPath;
				strName += _T("\\");
				strName += findData.cFileName;

				m_vFileResults.push_back(strName);
				//printf("FileName %S\\%S\n", szPath, findData.cFileName);
			}
			
		}

		if(!FindNextFile(hFind,&findData))
		{
			break;
		}
	}

	FindClose(hFind);
}




// 非递归查找文件
void KFindingFileNames::FindFilesNonRecursion(const TCHAR *szPath)
{

	queue<wstring> queueDir;	//	待查找的目录列表, 先进先出
	queueDir.push(szPath);


	while(!queueDir.empty())
	{
		
		wstring strCurrentPath = queueDir.front();
		queueDir.pop();

		//	遍历一个目录的一层
		TCHAR szFind[MAX_PATH] = {0};

		WIN32_FIND_DATA findData;
		_tcscpy(szFind, strCurrentPath.c_str());
		_tcscat(szFind, _T("\\*.*"));

		HANDLE hFind =  FindFirstFile(szFind,&findData);

		if (INVALID_HANDLE_VALUE == hFind)
		{
			ASSERT(0);
			continue;
		}

		while(true)
		{
			if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(findData.cFileName[0] != _T('.'))
				{
					TCHAR szSubDir[MAX_PATH] = {0};

					_tcscpy(szSubDir,strCurrentPath.c_str());
					_tcscat(szSubDir, _T("\\"));
					_tcscat(szSubDir, findData.cFileName); 
					//FindFiles(szSubDir);

					queueDir.push(szSubDir);
				}
			}
			else
			{
				if(CheckFileExNameList(findData.cFileName, m_vExNames))
				{
					wstring strName = strCurrentPath;
					strName += _T("\\");
					strName += findData.cFileName;

					m_vFileResults.push_back(strName);
					//printf("FileName %S\\%S\n", szPath, findData.cFileName);
				}

			}

			if(!FindNextFile(hFind,&findData))
			{
				break;
			}
		}

		FindClose(hFind);



	}
}
 

/***********************************************************************
* Function :	读取文件全部内容
*
* Parameter: const TCHAR * pFileName	文件名
* Parameter: int & iLen					文件长度
*
* Returns:   BYTE*	file buffer
***********************************************************************/
BYTE* GetFileBuffer(const TCHAR* pFileName, long& iLen)
{    
 
	FILE* fs = _tfopen(pFileName, _T("rb"));
 
	if (fs == NULL)
	{
		ASSERT(0);
		printf("GetFileBuffer Error %S\n", pFileName);
		return NULL;
	}

	fseek(fs, 0, SEEK_END);
	long lFileSize = ftell(fs);

	fseek(fs, 0, SEEK_SET);

	BYTE* pBuffer = new BYTE[lFileSize + 4];

	long iReadNum = fread (pBuffer, sizeof(BYTE), lFileSize, fs);

	fclose(fs);

	if(iReadNum != lFileSize)
	{
		printf ("fread %S error, iReadNum = %d, lFileSize = %d\n", pFileName, iReadNum, lFileSize);
		delete [] pBuffer;
		pBuffer = NULL;
	}
	else
	{
		// OK
		iLen = lFileSize;
		pBuffer[lFileSize] = 0;
	}

	return pBuffer;

}



// 判断buffer是否是GBK
bool CheckBufferGBK(const BYTE* pBuf, int Len)
{
	int iPos = 0;
	bool ret = true;
	while(iPos < Len)
	{
		if(pBuf[iPos] < 128)
		{
			iPos++;
		}		
		else if(pBuf[iPos] >= 0x81 && pBuf[iPos] <= 0xFE)
		{
			// 首字节在 81-FE 之间，尾字节在 40-FE 之间
			if(pBuf[iPos + 1] >= 0x40 && pBuf[iPos + 1] <= 0xFE)
			{
				iPos+=2;
			}
			else
			{
				ret = false;
				break;
			}
			
		}
		else
		{
			ret = false;
			break;
		}
		
	}

	return ret;
}

enum
{
	CHARSET_GBK,
	CHARSET_UTF8,
	CHARSET_UCS16_LE,
	CHARSET_UCS16_BE,
	CHARSET_UNKNOEN,
	CHARSET_SIZE
};

char CHARSET_NAME[CHARSET_SIZE][32] = 
{
	"GBK", "UTF8_BOM ", "UCS16_LE", "UCS16_BE", "UNKNOEN"

};

int CheckFileContentCharset(const BYTE* pBuf, int Len)
{
	if (Len >= 3)
	{
		//	UTF-8: EF BB BF
		if(pBuf[0] == 0xEF && pBuf[1] == 0xBB && pBuf[2] == 0xBF)
		{
			return CHARSET_UTF8;
		}

		// UTF16 FF FE
		if(pBuf[0] == 0xFF && pBuf[1] == 0xFE)
		{
			return CHARSET_UCS16_LE;
		}
		// UTF16 : Big Endian FF FE
		if(pBuf[0] == 0xFE && pBuf[1] == 0xFF)
		{
			return CHARSET_UCS16_BE;
		}
		else
		{
			bool ret = CheckBufferGBK(pBuf, Len);
			if(ret)
			{
				return CHARSET_GBK;
			}
		}

	}
	//	其他都无法判断

	return CHARSET_UNKNOEN;
}

int SaveUTF8File(const TCHAR* pszFile, const char *pUTF8Buf, int iLen)
{

	ASSERT(pszFile != NULL);
	ASSERT(pUTF8Buf != NULL);

	FILE* fs = _tfopen(pszFile, _T("wb"));

	if (fs == NULL)
	{
		printf("SaveUTF8File Error %S\n", pszFile);
		return -1;
	}

	unsigned char szBOM[3] = {0xEF,  0xBB,  0xBF};

	fwrite(szBOM, sizeof(unsigned char), sizeof(szBOM), fs);
	fwrite(pUTF8Buf, sizeof(char), iLen, fs);
	fclose(fs);

	return (0);
}


int SaveNormalFile(const TCHAR* pszFile, const char *pBuf, int iLen)
{
	FILE* fs = _tfopen(pszFile, _T("wb"));

	if (fs == NULL)
	{
		printf("SaveNormalFile Error %S\n", pszFile);
		return -1;
	}

	 
	fwrite(pBuf, sizeof(char), iLen, fs);
	fclose(fs);

	return 0;
}

// void fillArray(char array[], int size)
// {
// 	int i;
// 	for (i = 0; i <= size; i++) { // iterate from 0 to 100
// 		array[i] = 0; // VIOLATION (accessing array out of bounds: "array[i]" where (i == 100))
// 	}
// }

int ConvertFile2UTF8(const TCHAR* pszFile)
{
	int ret = CHARSET_UNKNOEN;
	long  iLen = 0;
	BYTE* pInBuf = GetFileBuffer(pszFile, iLen);

	if(pInBuf != NULL && iLen >= 3)
	{
		int iFileCharset =  CheckFileContentCharset(pInBuf, iLen);

		if(iFileCharset == CHARSET_GBK)
		{
			// GBK -> Unicode

			wchar_t *pUnicodeBuf = new wchar_t[iLen * 2];
			int iUnicodeSize = MultiByteToWideChar(936, 0,(char*) pInBuf, iLen, pUnicodeBuf, iLen * 2);

			

			//	Unicode  -> UTF8
			int iUtf8Size = iUnicodeSize * 3;
			char *pOutBuf = new char[iUtf8Size];

			//fillArray(pOutBuf,  iUtf8Size);

			int iOutLen = WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuf, iUnicodeSize, pOutBuf, iUtf8Size, NULL, NULL);

			SaveUTF8File(pszFile, pOutBuf, iOutLen);

			printf("ConvertFile2UTF8 ANSI 2 UTF8 %S\n", pszFile);
			delete [] pUnicodeBuf;
			delete [] pOutBuf;

		}
		else if(iFileCharset == CHARSET_UCS16_LE)
		{
			// Unicode -> UTF8
			char *pOutBuf = new char[iLen * 2];

			int iOutLen = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)(pInBuf + 2), iLen/2 - 1, pOutBuf, iLen * 2, NULL, NULL);

			SaveUTF8File(pszFile, pOutBuf, iOutLen);

			delete [] pOutBuf;

			printf("ConvertFile2UTF8 Unicode 2 UTF8 %S\n", pszFile);

		}
		else if(iFileCharset == CHARSET_UCS16_BE)
		{
			printf("ConvertFile2UTF8 iFileCharset == CHARSET_UCS16_BE %S\n", pszFile);
			//	
			ASSERT(0);
		}
		else if(iFileCharset == CHARSET_UTF8)
		{

		}
		else
		{
			printf("ConvertFile2UTF8 Unknouw to do %S\n", pszFile);
		}

		delete [] pInBuf;

		ret = iFileCharset;

	}

	return ret;
}


// 头文件转小写
int ConvertHeaderToLow(const TCHAR* pszFile)
{
	int count = 0;
	long  iLen = 0;
	BYTE* pInBuf = GetFileBuffer(pszFile, iLen);

	if(pInBuf != NULL && iLen >= 3)
	{

 
		char* pStart = (char*)pInBuf;
		char* pEnd = NULL;

		do 
		{
			char* p = strstr(pStart, "#include");

			if(p != NULL)
			{
				pEnd = strstr(p + 1, "\n");
				if(pEnd != NULL)
				{
					transform(p, pEnd, p, ::tolower);
					pStart = pEnd;
					count ++;
				}
			}

			if(p == NULL || pEnd == NULL)
			{
				break;
			}
 		 

		} while (true);
	
		
	}


	if(count > 0)
	{
		SaveNormalFile(pszFile, (char*)pInBuf, iLen);

	}

	if(pInBuf != NULL)
	{
		delete[] pInBuf;
	}

	
	return count;


}


void ConvertAllHeaders()
{
	KFindingFileNames finding(_T("."), _T(".cpp|.h|.cxx|.c|.java"));

	vector<wstring> vList;
	finding.FindFileList(vList);


	for(int i = 0; i < vList.size(); i++)
	{
		int ret = ConvertHeaderToLow (vList[i].c_str());
		printf ("File header [%d]  %S\n", ret, vList[i].c_str());
		 
	}

	printf ("ConvertHeaderToLow Files %d\n", vList.size());
	//for(int i = 0; i < List.size(); i++)
	//{
	//	printf("File Charset %s : %d\n", CHARSET_NAME[i], result[i]);
	//}

}

void ConvertCharset()
{

	KFindingFileNames finding(_T("."), _T(".cpp|.h|.cxx|.c|.java"));

	vector<wstring> vList;
	finding.FindFileList(vList);


	int result[CHARSET_SIZE] = {0};


	for(int i = 0; i < vList.size(); i++)
	{
		int ret = ConvertFile2UTF8 (vList[i].c_str());
		result[ret]++;
	}

	printf ("========================================\n");
	for(int i = 0; i < CHARSET_SIZE; i++)
	{
		printf("File Charset %s : %d\n", CHARSET_NAME[i], result[i]);
	}
}

// 默认无参数 转换所有文字字符集
//   /a 转换所有文字字符集且把头文件转小写
int _tmain(int argc, _TCHAR* argv[])
{
 
	ConvertCharset();

	if(argc == 2 && _tcsncmp(argv[1], _T("/a"), 2) == 0)
	{
		ConvertAllHeaders();
	}

	getchar();
	return 0;
}


