// 100100 1101

#include <bits/stdc++.h>
using namespace std;

// Function to get XOR of 'a' and 'b' (same length)
string xor1(string a, string b)
{
  string result = "";
  int n = b.length();
  for (int i = 1; i < n; i++)
    if (a[i] == b[i])
      result += "0";
    else
      result += "1";
  return result;
}

// Function to perform modulo-2 division and return remainder
string mod2div(string divident, string divisor)
{
  int n = divident.length();
  int l = divisor.length();
  string temp = divident.substr(0, l);

  while (l < n)
  {
    if (temp[0] == '1')
      temp = xor1(divisor, temp) + divident[l];
    else
      temp = xor1(string(l, '0'), temp) + divident[l];
    l += 1;
  }
  if (temp[0] == '1')
    temp = xor1(divisor, temp);
  else
    temp = xor1(string(l, '0'), temp);
  return temp;
}

// Function to encode data using key
string encodeData(string data, string key)
{
  int k = key.length();

  // Appends n-1 zeroes at end of data
  string appended_data = (data + string(k - 1, '0'));
  // Get reminder using modulo-2 division
  string remainder = mod2div(appended_data, key);
  // Append remainder in the original data
  string codeword = data + remainder;
  return codeword;
}

// Function to detect error in code
bool detectError(string codeword, string key)
{
  int k = key.length();
  string remainder = mod2div(codeword, key);
  if (remainder.compare(string(k - 1, '0')) == 0)
    return false;
  else
    return true;
}

int main()
{
  string data, key;
  string cor_encoded_data, act_encoded_data;

  cout << "\nEnter data (in bits): ";
  cin >> data;
  cout << "\nEnter key (in bits): ";
  cin >> key;
  cor_encoded_data = encodeData(data, key);
  cout << "\nEncoded Data: " << cor_encoded_data;

  cout << "\nEnter encoded data received (in bits): ";
  cin >> act_encoded_data;
  // Detect error in encoded data
  if (detectError(act_encoded_data, key))
    cout << "\nError detected.";
  else
    cout << "\nCorrect encoded data.";

  return 0;
}
