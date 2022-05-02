// 1 0 1 0 1 1 0 1 1 1 0

#include <bits/stdc++.h>
using namespace std;

// Function to convert decimal to binary
vector<int> convert_to_binary(int num, int n)
{
  vector<int> bin(n, 0);
  for (int i = 0; num != 0 && i < bin.size(); i++)
  {
    bin[i] = num % 2;
    num /= 2;
  }
  return bin;
}

// Function to convert binary to decimal
int convert_to_decimal(vector<int> bin)
{
  int num = 0;
  for (int i = 0; i < bin.size(); i++)
    num += bin[i] * pow(2, i);
  return num;
}

// Get minimum number of parity bits required
int get_parity_bits(int n)
{
  for (int i = 0;; i++)
    if (pow(2, i) >= n + 1)
      return i;
}

int main()
{
  int n;
  cout << "\nNumber of bits: ";
  cin >> n;
  vector<int> hc(n);
  cout << "\nEnter delivered hamming code: ";
  for (int i = n - 1; i >= 0; i--)
    cin >> hc[i];

  // Calculating parity bits
  int pb = get_parity_bits(n);
  vector<int> parity_bits(pb, 0), data_bits;
  for (int i = 0; i < pb; i++)
  {
    for (int j = 0; j < n; j++)
      if (convert_to_binary(j + 1, n)[i] == 1)
        parity_bits[i] += hc[j];
    parity_bits[i] = parity_bits[i] % 2;
  }

  // Calculating error position using parity bits
  int epos = convert_to_decimal(parity_bits);
  if (epos == 0)
    cout << "\nNo error.";
  else
  {
    cout << "\nError at: " << epos;
    // Correcting error
    hc[epos - 1] = 1 - hc[epos - 1];
  }

  // Calculating data
  for (int i = 0; i < n; i++)
    if (ceil(log2(i + 1)) != floor(log2(i + 1)))
      data_bits.push_back(hc[i]);

  int dpos = convert_to_decimal(data_bits);
  cout << "\nCorrect data: " << dpos;

  return 0;
}
