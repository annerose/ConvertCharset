// ConvertingCharset.cpp : 定义控制台应用程序的入口点。
//

#ifndef CONVERT_CHARSET_H__
#define CONVERT_CHARSET_H__



#include <windows.h>
#include <vector>
#include <string>
#include <queue>   
#include <algorithm>  
using namespace std;

class KFindingFileNames
{
public:

	KFindingFileNames(const TCHAR *szPath, const TCHAR* szExNames);

	virtual ~KFindingFileNames();


	void FindFileList(vector<wstring>& vFiles);



private:

	//  测试扩展名, 小写 ".cpp"
	bool CheckFileExName(const TCHAR * szFileName, const TCHAR * pszExName);

	//  测试扩展名列表
	bool CheckFileExNameList(const TCHAR * szFileName, vector<wstring>& vExNames);



	// 递归查找文件
	void FindFiles(const TCHAR *szPath);

	// 非递归查找文件
	void FindFilesNonRecursion(const TCHAR *szPath);

private:

	// 扩展名列表
	vector<wstring> m_vExNames;

	//	最终列表
	vector<wstring> m_vFileResults;

	// 初始路径
	wstring m_strPathName;


};

 
 


 


 

 



#endif //	 CONVERT_CHARSET_H__