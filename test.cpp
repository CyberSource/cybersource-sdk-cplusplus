#include<iostream>
#include<stdlib.h>

std::string wsToStr(std::wstring ws)
{
  int  size = ws.length();
  char *str = new char[size+1];
  wcstombs(str, ws.c_str(), size+1);
  std::string tmp(str);
  delete[] str;
  return tmp; 
}

std::wstring strToWs(std::string str)
{
   int size = str.length();
   wchar_t* ws = new wchar_t[size+1];
   mbstowcs(ws ,str.c_str(), size+1);
   std::wstring tmp(ws);
   delete[] ws;
   return tmp;
}
int main()
{
   //std::wstring ws(L"<xml>Awe!!yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy#!!#*</xml>");
   //std::cout<<wsToStr(ws)<<"\n";
   //std::wcout<<ws<<"\n";
   //std::wcout<<strToWs(wsToStr(ws))<<"\n";
   //std::cout<<"done";
   const char* ch = "Hello";
   std::string str = ch;
   std::string* str2 = new std::string(ch);
   std::cout<<*str2;
}
